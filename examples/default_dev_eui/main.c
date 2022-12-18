/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * 
 * This example displays devices default LoRaWAN Dev EUI which is based
 * on the Pico SDK's pico_get_unique_board_id(...) API which uses the
 * on board NOR flash device 64-bit unique ID.
 * 
 * https://raspberrypi.github.io/pico-sdk-doxygen/group__pico__unique__id.html
 * 
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/lorawan.h"
#include "tusb.h"

#define LED_GREEN 23
#define LED_BLUE 24

#define UART1TX 4
#define UART1Rx 5

char print_buf[200];

int main(void)
{
    char devEui[17];

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

    // while (!tud_cdc_connected()) {
    //     tight_loop_contents();
    // }
    
    // get the default Dev EUI as a string and print it out
    sprintf(print_buf, "Pico LoRaWAN - Default Dev EUI = %s\r\n", lorawan_default_dev_eui(devEui));

    // do nothing
    while (1) {
        gpio_put(LED_BLUE, 1);
        sleep_ms(250);
        gpio_put(LED_BLUE, 0);
        sleep_ms(250);
        uart_puts(uart1, "blinking uart 1!\r\n");
        uart_puts(uart1, print_buf);

//        tight_loop_contents();
    }

    return 0;
}
