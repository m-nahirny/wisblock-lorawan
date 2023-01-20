/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * This example uses OTAA to join the LoRaWAN network and then sends a
 * "hello world" uplink message periodically and prints out the
 * contents of any downlink message.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/sleep.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "hardware/clocks.h"
#include "hardware/rosc.h"
#include "hardware/structs/scb.h"

#include "rak11310/lorawan.h"
#include "rak11310/rak11310.h"
#include "rak11310/board-config.h"

#include "eeprom-board.h"
#include "rtc-board.h"
#include "i2c-board.h"

// edit with LoRaWAN Node Region and OTAA settings 
#include "config.h"

#define i2c_rak i2c1

// pin configuration for SX12xx radio module
 const struct lorawan_sx12xx_settings sx12xx_settings = {
    .spi = {
        .inst = spi1,
        .mosi = RADIO_MOSI,
        .miso = RADIO_MISO,
        .sck  = RADIO_SCLK,
        .nss  = RADIO_NSS
    },
    .reset = RADIO_RESET,
    .busy = RADIO_BUSY,
    .dio1  = RADIO_DIO_1
};

// // OTAA settings
const struct lorawan_otaa_settings otaa_settings = {
    .device_eui   = LORAWAN_DEVICE_EUI,
    .app_eui      = LORAWAN_APP_EUI,
    .app_key      = LORAWAN_APP_KEY,
    .channel_mask = LORAWAN_CHANNEL_MASK
};

// variables for receiving data
int receive_length = 0;
uint8_t receive_buffer[242];
uint8_t receive_port = 0;

char print_buf[200];

void internal_temperature_init()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
}

float internal_temperature_get()
{
    const float v_ref = 3.3;

    // select and read the ADC
    adc_select_input(4);
    uint16_t adc_raw = adc_read();

    // convert the raw ADC value to a voltage
    float adc_voltage = adc_raw * v_ref / 4095.0f;

    // convert voltage to temperature, using the formula from 
    // section 4.9.4 in the RP2040 datasheet
    //   https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf
    float adc_temperature = 27.0 - ((adc_voltage - 0.706) / 0.001721);

    return adc_temperature;
}

static bool awake;

static void sleep_callback(void) {
    uart_puts(uart1, "RTC woke us up\n");
    uart_default_tx_wait_blocking();
    awake = true;
    return;
}

static void rtc_sleep(int8_t minute_to_sleep_to, int8_t second_to_sleep_to) {

    datetime_t t_alarm = {
            .year  = 2020,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = minute_to_sleep_to,
            .sec   = second_to_sleep_to
    };

    uart_puts(uart1, "Going to sleep.......\n");
    uart_default_tx_wait_blocking();

    sleep_goto_sleep_until(&t_alarm, &sleep_callback);
}

// from https://ghubcoder.github.io/posts/awaking-the-pico/
void recover_from_sleep(uint scb_orig, uint clock0_orig, uint clock1_orig){

    //Re-enable ring Oscillator control
    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS);

    //reset procs back to default
    scb_hw->scr = scb_orig;
    clocks_hw->sleep_en0 = clock0_orig;
    clocks_hw->sleep_en1 = clock1_orig;

    uart_puts(uart1, "Resetting clocks\n");
    uart_default_tx_wait_blocking();

    //reset clocks
    clocks_init();
    stdio_init_all();

    gpio_init(RAK11310_LED_GREEN);
    gpio_set_dir(RAK11310_LED_GREEN, GPIO_OUT);
    gpio_put(RAK11310_LED_GREEN, 1);

    gpio_init(RAK11310_LED_BLUE);
    gpio_set_dir(RAK11310_LED_BLUE, GPIO_OUT);

    uart_init(uart1, 115200);
    gpio_set_function(RAK11310_UART1TX, GPIO_FUNC_UART);
    gpio_set_function(RAK11310_UART1RX, GPIO_FUNC_UART);

    return;
}

int main( void )
{
    I2c_t i2c_obj;
    I2cId_t i2c_id;
    PinNames scl, sda;

    // initialize stdio and wait for USB CDC connect
    stdio_init_all();

    gpio_init(RAK11310_LED_GREEN);
    gpio_set_dir(RAK11310_LED_GREEN, GPIO_OUT);
    gpio_put(RAK11310_LED_GREEN, 1);

    gpio_init(RAK11310_LED_BLUE);
    gpio_set_dir(RAK11310_LED_BLUE, GPIO_OUT);

    uart_init(uart1, 115200);
    gpio_set_function(RAK11310_UART1TX, GPIO_FUNC_UART);
    gpio_set_function(RAK11310_UART1RX, GPIO_FUNC_UART);
    
    sleep_ms(250);
    uart_puts(uart1, "\r\nWisblock LoRaWAN - Send Temperature\r\n");
//    gpio_put(LED_GREEN, 0);
//    sleep_ms(250);

    I2cMcuInit(&i2c_obj, i2c_id, scl, sda);

    // do this once
    // uint16_t tempDevNonce = 0;
    // RtcBkupWrite(tempDevNonce, tempDevNonce);


    //crudely reset the clock each time
    //to the value below
    datetime_t t = {
            .year  = 2020,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = 45,
            .sec   = 00
    };

    // Start the Real time clock
    rtc_init();

    //save values for later
    uint scb_orig = scb_hw->scr;
    uint clock0_orig = clocks_hw->sleep_en0;
    uint clock1_orig = clocks_hw->sleep_en1;

    // uncomment next line to enable debug
     //lorawan_debug(true);

    // initialize the LoRaWAN stack
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

    // // Start the join process and wait
    uart_puts(uart1, "Joining LoRaWAN network ... \r\n");
    lorawan_join();

    while (!lorawan_is_joined()) {
        lorawan_process();
    }
    uart_puts(uart1, "joined successfully!\r\n");

    uint8_t channel = 0;

    internal_temperature_init();

    // loop forever
    while (1) {
        gpio_put(RAK11310_LED_GREEN, 0);
        // get the internal temperature
       float adc_temperature = internal_temperature_get();

        sprintf(print_buf, "Temperature is %f\r\n", adc_temperature);
        uart_puts(uart1, print_buf);
        int status = lorawan_send_cayenne_temperature(adc_temperature, 2, channel);
        if (status == 0) {
            uart_puts(uart1, "Message sent!\r\n");
            // Switch LED 1 ON
            gpio_put(RAK11310_LED_BLUE, 1);
            sleep_ms(250);
            gpio_put(RAK11310_LED_BLUE, 0);
        }
        else
            uart_puts(uart1, "Message send failed!\r\n");

        // make the RP2040 dormant
        awake = false;
        uart_puts(uart1, "Sleep from xosc\r\n");
        uart_default_tx_wait_blocking();

        sleep_run_from_xosc();
        //Reset real time clock to a value
        rtc_set_datetime(&t);
        //sleep here, in this case for 20 seconds
        rtc_sleep(45, 20);

        //will return here and awake should be true
        while (!awake) {
            //This should not fire
            printf("Should be sleeping\n");
            uart_default_tx_wait_blocking();
        }

        //reset processor and clocks back to defaults
        uart_puts(uart1, "Recovering from sleep\n");
        uart_default_tx_wait_blocking();
        recover_from_sleep(scb_orig, clock0_orig, clock1_orig);

        gpio_put(RAK11310_LED_GREEN, 1);

        sleep_ms(2000);

        // wait for up to 30 seconds for an event
        // if (lorawan_process_timeout_ms(30000) == 0) {
        //     // check if a downlink message was received
        //     receive_length = lorawan_receive(receive_buffer, sizeof(receive_buffer), &receive_port);
        //     if (receive_length > -1) {
        //         sprintf(print_buf, "received a %d byte message on port %d: \r\n", receive_length, receive_port);
        //         uart_puts(uart1, print_buf);

        //         for (int i = 0; i < receive_length; i++) {
        //             sprintf(print_buf, "%02x", receive_buffer[i]);
        //             uart_puts(uart1, print_buf);
        //         }
        //         uart_puts(uart1, "\r\n");
        //     }
        // }
    }

    return 0;
}
