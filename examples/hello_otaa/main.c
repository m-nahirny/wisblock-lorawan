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
#include <string.h>

#include "pico/stdlib.h"
#include "pico/lorawan.h"
//#include "tusb.h"

// edit with LoRaWAN Node Region and OTAA settings 
#include "config.h"

// pin configuration for SX12xx radio module
const struct lorawan_sx12xx_settings sx12xx_settings = {
    .spi = {
        .inst = spi1,
        .mosi = 11,
        .miso = 12,
        .sck  = 10,
        .nss  = 13
    },
    .reset = 14,
    .busy = 15,
    .dio1  = 20
};

// OTAA settings
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

#define LED_GREEN 23
#define LED_BLUE 24

#define UART1TX 4
#define UART1Rx 5

char print_buf[200];

int main( void )
{
    // initialize stdio and wait for USB CDC connect
    stdio_init_all();

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 1);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);

    uart_init(uart1, 115200);
    gpio_set_function(UART1TX, GPIO_FUNC_UART);
    gpio_set_function(UART1Rx, GPIO_FUNC_UART);
    
    sleep_ms(250);
    uart_puts(uart1, "\r\nPico LoRaWAN - Hello OTAA\r\n");
//    gpio_put(LED_GREEN, 0);
    sleep_ms(250);

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

    uint32_t last_message_time = 0;

    // loop forever
    while (1) {
        // printf("blinking!\r\n");
        gpio_put(LED_BLUE, 1);
        sleep_ms(250);
        gpio_put(LED_BLUE, 0);
        sleep_ms(250);
        // uart_puts(uart1, "blinking uart 1!\r\n");
    //    let the lorwan library process pending events
        lorawan_process();

        // get the current time and see if 5 seconds have passed
        // since the last message was sent
        uint32_t now = to_ms_since_boot(get_absolute_time());
            
        if ((now - last_message_time) > 10000) {
            const char* message = "hello world";

            // try to send an unconfirmed uplink message
            sprintf(print_buf, "sending unconfirmed message '%s' ... \r\n", message);
            uart_puts(uart1, print_buf);
            if (lorawan_send_unconfirmed(message, strlen(message), 2) < 0) {
                uart_puts(uart1, "failed!!!\r\n");
            } else {
                uart_puts(uart1, "success!\r\n");
            }

            last_message_time = now;
        }

        // check if a downlink message was received
        receive_length = lorawan_receive(receive_buffer, sizeof(receive_buffer), &receive_port);
        if (receive_length > -1) {
            sprintf(print_buf, "received a %d byte message on port %d: \r\n", receive_length, receive_port);
            uart_puts(uart1, print_buf);

            for (int i = 0; i < receive_length; i++) {
                uart_putc(uart1, receive_buffer[i]);
            }
        }
    }

    return 0;
}
