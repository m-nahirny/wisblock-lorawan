/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include  <math.h>
#include <exception>
#include "pico/stdlib.h"
#include "system/adc.h"
#include "system/i2c.h"
#include "D:\\Pico\\pico-sdk\\src\\rp2_common\\pico_multicore\\include\\pico\\multicore.h"
#include "hardware/adc.h"

#include "Lis3dh.h"
#include "AccelerationSeries.h"
#include "CalculateVmaxDef.h"

#include "boards/i2c-board.h"
#include "boards/uart-board.h"

#include "GUI_Paint.h"
#include "EPD_2in7.h"

#include "rak11310/lorawan.h"
#include "rak11310/rak11310.h"
#include "rak11310/board-config.h"

#include "hardware/clocks.h"
#include "hardware/rosc.h"
#include "hardware/structs/scb.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"

#include "eeprom-board.h"
#include "rtc-board.h"
#include "i2c-board.h"

#include "spi-board.h"

// edit with LoRaWAN Node Region and OTAA settings 
#include "config.h"

#include "LmhpClockSync.h"

using namespace std;

#define INSTRUMENT "VM100"

#define UART1TX 4
#define UART1Rx 5

#define NUM_BASE_SAMPLES 1000

#define LOG_CHECK_A_THRESHOLD 0.08
#define LOG_SEND_THRESHOLD 0.04

#define DEBOUNCE_TIME_SEC 5 // time period to prevent multiple values reported in short time period

EventStatus status;
// this needs to be in static memory with dual core
uint8_t pageImage[EPD_2IN7_WIDTH * EPD_2IN7_HEIGHT / 8];

// current time - read by core 1 but used by both cores
struct tm now;

struct tm GetDateTime()
{
    struct tm t_now;
    uint16_t year;
    uint8_t month, date, hour, minute, second;

    RtcGetTime(&year, &month, &date, &hour, &minute, &second);
    t_now.tm_year = year - 1900;
    t_now.tm_mon = month;
    t_now.tm_mday = date;
    t_now.tm_hour = hour;
    t_now.tm_min = minute;
    t_now.tm_sec = second;

    return t_now;
}

//this function will execute on core1
void core1_readSensor() {
    I2c_t i2c_obj;
    I2cId_t i2c_id;
    PinNames scl, sda;

    Lis3dh accelerometer;
    double base[3];
    uint8_t accel_reg[3] = {0x28, 0x2A, 0x2C};
    int baseX = 0, baseY = 0, baseZ = 0;
    double baseX_scaled = 0, baseY_scaled = 0, baseZ_scaled = 0;
    int base_counter = 0;
    uint8_t x_axis, y_axis, z_axis;
    float accel_x, accel_y, accel_z;
    char msg_buffer[200];
    double last_accel_x, last_accel_y, last_accel_z;
    double event[3];
    int num_readings, last_sec = 0;
    bool first_x = true, first_y = true, first_z = true;
    double last_sec_x, last_sec_y, last_sec_z;
    int num_values_x = 0, num_values_y = 0, num_values_z = 0;
    double sum_x = 0, sum_y = 0, sum_z = 0;
    double adjustment_x = 0, adjustment_y = 0, adjustment_z = 0;
    uint64_t sys_now;
    double time_now;

	double Ta = 0, Tb = 0;
    float Agb_x, Agb_y, Agb_x_prev = 0, Agb_y_prev = 0;
    float Vgx, Vgy, Vgx_log, Vgy_log, Vmax_log = 0;
	unsigned long current_minute = 0, current_second = 0, Vmax_sec = 0;
    float lastVmax;
    float debounceVmax = 0;
    bool saChanged = false;

    status.LastEventX = 0;
    status.LastEventXTime = 0;
    status.LastEventY = 0;
    status.LastEventYTime = 0;
    status.MaxEvent = 0;
    status.MaxEventTime = 0;
    status.clear_events = false;

    AccelerationSeries accel_series_x(LOG_CHECK_A_THRESHOLD), accel_series_y(LOG_CHECK_A_THRESHOLD);

    I2cMcuInit(&i2c_obj, i2c_id, scl, sda);

    // RTC
    RtcInit();

    // uart_puts(uart1, "Initializing accelerometer core 1\r\n");
    sleep_ms(250);
    accelerometer.Init();
    uint8_t accelId = accelerometer.WhoAmI();
    sprintf(msg_buffer, "Accelerometer: I am %x\r\n", accelId);
    uart_puts(uart1, msg_buffer);

    // float x, y, z, lp_sum_x, lp_sum_y, lp_sum_z;
    // int numSamples = 0;
    // uint32_t systime;
    // uint32_t last_second = 0;
    // while(1) {
    //     while (!accelerometer.DataAvailable());
    //     x = accelerometer.ReadData(OUT_X_L);
    //     y = accelerometer.ReadData(OUT_Y_L);
    //     z = accelerometer.ReadData(OUT_Z_L);
    //     numSamples++;
    //     sum_x += x;
    //     sum_y += y;
    //     sum_z += z;
    //     systime = time_us_32();
    //     if (systime / 1000000 != last_second)
    //     {
    //         printf("Second: %d, Num samples: %d, X: %f, Y: %f, Z: %f\r\n", last_second, numSamples, sum_x / numSamples, sum_y / numSamples, sum_z / numSamples);
    //         last_second = systime / 1000000;
    //         numSamples = 0;
    //         sum_x = 0;
    //         sum_y = 0;
    //         sum_z = 0;
    //     }
    // }
 
    base[0] = 0;
    base[1] = 0;
    base[2] = 0;

    for (int i = 0; i < NUM_BASE_SAMPLES; i++)
    {
        // printf("Waiting for data...");
        /* Ask for acceleration */
        while (!accelerometer.DataAvailable());
        accel_x = accelerometer.ReadData(OUT_X_L);
        accel_y = accelerometer.ReadData(OUT_Y_L);
        accel_z = accelerometer.ReadData(OUT_Z_L);
        // printf("Got data #%d X: %f, Y: %f, Z: %f\r\n", i, accel_x, accel_y, accel_z);
        base[0] += accel_x;
        base[1] += accel_y;
        base[2] += accel_z;
    }

    base[0] /= NUM_BASE_SAMPLES;
    base[1] /= NUM_BASE_SAMPLES;
    base[2] /= NUM_BASE_SAMPLES;

    // determine which axis is Z (vertical) based on acceleration due to gravity being more than 0.5 (1.0 if vertical)
    if (fabs(base[0]) > 0.5)
    {
        z_axis = 0;
        x_axis = 1;
        y_axis = 2;
    }
    if (fabs(base[1]) > 0.5)
    {
        z_axis = 1;
        x_axis = 0;
        y_axis = 2;
    }
    if (fabs(base[2]) > 0.5)
    {
        z_axis = 2;
        x_axis = 0;
        y_axis = 1;
    }
    // sprintf(msg_buffer, "Base X scaled: %f, base Y scaled: %f, base Z scaled: %f\r\n", base[x_axis], base[y_axis], base[z_axis]);
    // uart_puts(uart1, msg_buffer);

    // check if the static values need to be adjusted - if all accelerometer reading in an axis are in one direction. 
    // Note that there are 253 or 254 readings every 2 seconds so if all are the same sign it's very likely that the orienntation has changed

    last_accel_x = 0;
    last_accel_y = 0;
    last_accel_z = 0;

    event[z_axis] = 0;
    num_readings = 0;

    while(1) {

        sys_now = time_us_64();

        /* Ask for acceleration */
        while (!accelerometer.DataAvailable());
        accel_x = accelerometer.ReadData(accel_reg[x_axis]);
        accel_y = accelerometer.ReadData(accel_reg[y_axis]);

        // num_readings++;
        // if (last_sec != now.tv_sec)
        // {
        //   printf ("%d readings per second\n", num_readings);
        //   num_readings = 0;
        //   last_sec = now.tv_sec;
        // }

        event[x_axis] = accel_x - base[x_axis];
        event[y_axis] = accel_y - base[y_axis];

        // round to nearest 1 %g
        event[x_axis] = round(event[x_axis] * 100) / 100;
        event[y_axis] = round(event[y_axis] * 100) / 100;

        if (first_x)
        {
            first_x = false;
            last_sec_x = sys_now / 1000000;
            sum_x = 0;
            num_values_x = 0;
        }
        else
        {
            if (last_accel_x <= 0 && event[x_axis] > 0 || last_accel_x >= 0 && event[x_axis] < 0)
            {
                // keep track of the last time there was a change in direction. If this becomes excessive modify the offsets
                last_sec_x = sys_now / 1000000;
                sum_x = 0;
                num_values_x = 0;
            }
        }
        last_accel_x = event[x_axis];
        sum_x += event[x_axis];
        num_values_x++;

        if (first_y)
        {
            first_y = false;
            last_sec_y = sys_now / 1000000;
            sum_y = 0;
            num_values_y = 0;
        }
        else
        {
            if (last_accel_y <= 0 && event[y_axis] > 0 || last_accel_y >= 0 && event[y_axis] < 0)
            {
                // keep track of the last time there was a change in direction. If this becomes excessive modify the offsets
                last_sec_y = sys_now / 1000000;
                sum_y = 0;
                num_values_y = 0;
            }
        }
        last_accel_y = event[y_axis];
        sum_y += event[y_axis];
        num_values_y++;

        time_now = sys_now / 1000000;

        // if the direction hasn't changed in 2 seconds adjust the base offsets
        if (time_now - last_sec_x > 2)
        {
            adjustment_x = sum_x / num_values_x;
            base[x_axis] += adjustment_x;
            event[x_axis] += adjustment_x;
            first_x = true;
            // sprintf(msg_buffer, "Adjustment X: %f, Base X scaled: %f, base Y scaled: %f at %f\r\n", adjustment_x, base[x_axis], base[y_axis], time_now);
            // uart_puts(uart1, msg_buffer);
        }

        if (time_now - last_sec_y > 2)
        {
            adjustment_y = sum_y / num_values_y;
            base[y_axis] += adjustment_y;
            event[y_axis] += adjustment_y;
            first_y = true;
            // sprintf(msg_buffer, "Adjustment Y: %f, Base X scaled: %f, base Y scaled: %f at %f\r\n", adjustment_y, base[x_axis], base[y_axis], time_now);
            // uart_puts(uart1, msg_buffer);
        }

        // sprintf(msg_buffer, "%ld\t%f\t%f\r\n",  now, event[x_axis], event[y_axis]);
        // printf(msg_buffer);

        Agb_x = AG * event[x_axis];
        Agb_y = AG * event[y_axis];

        Tb = time_now;
        if (Ta == 0)
        {
            accel_series_x.Clear(Tb);
            accel_series_y.Clear(Tb);
        }
        else
        {
            if (Agb_x < 0 && Agb_x_prev < 0 || Agb_x > 0 && Agb_x_prev > 0 || Agb_x == 0 && Agb_x_prev == 0)
            {
                accel_series_x.AddEvent(Agb_x, Tb);
                Agb_x_prev = Agb_x;
            }
            else
            {
                // direction change detected
                Vgx_log = accel_series_x.CalculateSeriesForLog();
                Vgx_log = fabs(Vgx_log) / 10; 
                if (Vgx_log > LOG_SEND_THRESHOLD && Vgx_log > Vmax_log)
                {
                    Vmax_log = Vgx_log;
                    Vmax_sec = Tb;
                }
                Vgx = accel_series_x.CalculateSeriesForDisplay();
                if (fabs(Vgx) > 0)
                {
                    now = GetDateTime();
                    lastVmax = fabs(Vgx) / 10;   // convert to cm/sec
                    if (Tb - status.LastEventXTime > DEBOUNCE_TIME_SEC && Tb - status.MaxEventTime > DEBOUNCE_TIME_SEC)
                    {
                        status.LastEventX = lastVmax;
                        status.LastEventXTime = Tb;
                        status.LastEventXDateTime = now;
                        debounceVmax = 0;
                    }
                    else
                    {
                        if (lastVmax > debounceVmax)
                        {
                            status.LastEventX = lastVmax;
                            status.LastEventXTime = Tb;
                            status.LastEventXDateTime = now;
                            debounceVmax = lastVmax;
                        }
                    }

                    if (lastVmax > 99)
                    {
                        status.LastEventX = 0;
                    }
                    else
                    {
                        if (lastVmax > status.MaxEvent)
                        {
                            status.MaxEvent = lastVmax;
                            status.MaxEventTime = Tb;
                            status.MaxEventDateTime = now;
                            // cout << "New max X exent " << lastVmax << " at " << Tb << endl;
                        }
                        saChanged = true;
                    }
                }

                accel_series_x.Clear(Ta);
                accel_series_x.AddEvent(Agb_x, Tb);
                Agb_x_prev = Agb_x;
            }

            if (Agb_y < 0 && Agb_y_prev < 0 || Agb_y > 0 && Agb_y_prev > 0 || Agb_y == 0 && Agb_y_prev == 0)
            {
                accel_series_y.AddEvent(Agb_y, Tb);
                Agb_y_prev = Agb_y;
            }
            else
            {
                // direction change detected
                Vgy_log = accel_series_y.CalculateSeriesForLog();
                Vgy_log = fabs(Vgy_log) / 10; 
                if (Vgy_log > LOG_SEND_THRESHOLD && Vgy_log > Vmax_log)
                {
                    Vmax_log = Vgy_log;
                    Vmax_sec = Tb;
                }
                Vgy = accel_series_y.CalculateSeriesForDisplay();
                if (fabs(Vgy) > 0)
                {
                    now = GetDateTime();
                    lastVmax = fabs(Vgy) / 10;    // convert to cm/sec
                    if (Tb - status.LastEventYTime > DEBOUNCE_TIME_SEC && Tb - status.MaxEventTime > DEBOUNCE_TIME_SEC)
                    {
                        status.LastEventY = lastVmax;
                        status.LastEventYTime = Tb;
                        status.LastEventYDateTime = now;
                    }
                    else
                    {
                        if (lastVmax > debounceVmax)
                        {
                            status.LastEventY = lastVmax;
                            status.LastEventYTime = Tb;
                            status.LastEventYDateTime = now;
                            debounceVmax = lastVmax;
                        }
                    }

                    if (lastVmax > 99)
                    {
                        status.LastEventY = 0;
                    }
                    else
                    {
                        if (lastVmax > status.MaxEvent)
                        {
                            status.MaxEvent = lastVmax;
                            status.MaxEventTime = Tb;
                            status.MaxEventDateTime = now;
//                            cout << "New max Y exent " << lastVmax << " at " << Tb << endl;
                        }
                        saChanged = true;
                    }
                }

                accel_series_y.Clear(Ta);
                accel_series_y.AddEvent(Agb_y, Tb);
                Agb_y_prev = Agb_y;
            }
        }
            
        Ta = Tb;
        
        if (Tb != current_second)
        {
            current_second = Tb;
    
            if (saChanged)
            {
                saChanged = false;

                // pthread_mutex_lock( &mutexDb );
                // db.SaveStatus(&status);
                // pthread_mutex_unlock( &mutexDb );

                // udp.SendEvent(&status, &config, &broadcast_DNS, &broadcast_IP, ip_buffer);
            }

            // send the largenst vMax log event that happened in the past second
            if (Vmax_sec != 0)
            {
                // udp.SendLogEvent(Vmax_log, &config, &broadcast_DNS, &broadcast_IP, ip_buffer);
                Vmax_sec = 0;
                Vmax_log = 0;
            }
            // need to check in case UI cleared the Sa
            // pthread_mutex_lock( &mutexDb );
            // error = db.GetEventStatus(&status);
            // pthread_mutex_unlock( &mutexDb );

            if (status.clear_events == 1)
            {
                Tb = 0;
                accel_series_x.Clear(Tb);
                accel_series_y.Clear(Tb);
            }
        }
    }

        // gpio_put(LED_BLUE, 1);
        // sleep_ms(333);
        // gpio_put(LED_BLUE, 0);
        // sleep_ms(333);

        // gpio_put(LED_BLUE, 1);
        // sleep_ms(333);
        // gpio_put(LED_BLUE, 0);
        // sleep_ms(1000);
    // }
}

EPD_Device *epd;
Display *display;

int main() {
    EventStatus lastStatus;
    int lastMinute = 0;
    char print_buf[200];

    string device_name = INSTRUMENT;

    stdio_init_all();

    gpio_init(RAK11310_LED_GREEN);
    gpio_set_dir(RAK11310_LED_GREEN, GPIO_OUT);

    // push button
    gpio_init(RAK11310_LED_BLUE);
    gpio_set_dir(RAK11310_LED_BLUE, GPIO_IN);
	gpio_pull_down(RAK11310_LED_BLUE);

    // gpio_set_function(RAK11310_UART0TX, GPIO_FUNC_UART);
    // gpio_set_function(RAK11310_UART0RX, GPIO_FUNC_UART);
    // uart_init(uart0, 115200);

    Uart_t uart;
    UartId_t uartId;

    UartMcuInit(&uart, uartId, RAK11310_UART1TX, RAK11310_UART1RX);

    sleep_ms(1250);
    uint8_t buf[200] = "vMax Sensor UartMcuPutBuffer\r\n";
        uart_puts(uart1, "\r\n\r\nvMax Sensor uart_puts\r\n");
        UartMcuPutBuffer(&uart, buf, 0);

    // multicore_launch_core1(core1_readSensor);

    adc_init();

    // battery is connected to ADC0
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / (1 << 12);
    // printf("Conversion factor: %f\r\n", conversion_factor);

    // // initialize the LoRaWAN stack
    uart_puts(uart1, "Initilizating LoRaWAN ... \r\n");
    sleep_ms(250);
    int res = lorawan_init_otaa(&sx12xx_settings, LORAWAN_REGION, &otaa_settings);
    sprintf(print_buf, "Initilizating LoRaWAN - result = %d\r\n", res);
    if (res < 0) {
        uart_puts(uart1, "failed!!!\r\n");
        while (1) {
            tight_loop_contents();
        }
    } else {
        uart_puts(uart1, "success!\r\n");
    }

    // // // Start the join process and wait
    uart_puts(uart1, "Joining LoRaWAN network ... \r\n");
    lorawan_join();

    while (!lorawan_is_joined()) {
        lorawan_process();
    }
    uart_puts(uart1, "joined successfully!\r\n");

    epd = new EPD_Device();
    display = new Display();

     if(epd->Module_Init()!=0){
        uart_puts(uart1, "Initialization error\r\n");
        return -1;
    }

    // printf("e-Paper Init...\r\n");
    display->Init(epd);

    while (1) {
        if (status.MaxEvent != lastStatus.MaxEvent || status.LastEventX != lastStatus.LastEventX || status.LastEventY != lastStatus.LastEventY) {
            lastStatus = status;
            // printf("last X: %f at %d, last Y %f at %d, max %f at %d\r\n", status.LastEventX, status.LastEventXTime, status.LastEventY, status.LastEventYTime, status.MaxEvent, status.MaxEventTime);

            display->DrawDisplay(pageImage, &lastStatus, device_name.c_str(), now);
        }

        if (time_us_32() / 60000000 != lastMinute) {
            lastMinute = time_us_32() / 60000000;

            uint16_t result = adc_read();
            // printf("Raw value: 0x%03x\r\n", result);
            float convertedValue = (int)result * conversion_factor;
            // printf("Battery Converted Value: %f\r\n", convertedValue);
        }

        gpio_put(RAK11310_LED_GREEN, 1);

        sleep_ms(250);
        gpio_put(RAK11310_LED_GREEN, 0);
        sleep_ms(750);
    }
}
