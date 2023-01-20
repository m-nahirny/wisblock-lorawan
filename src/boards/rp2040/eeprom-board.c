/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

//#include <stdio.h>
//#include <stdlib.h>

#include "utilities.h"
#include "eeprom-board.h"
//#include "pico/stdlib.h"
//#include "hardware/flash.h"

// taken from Pico SDK example flash_program

// We're going to erase and reprogram a region 256k from the start of flash.
// Once done, we can access this at XIP_BASE + 256k.
//#define FLASH_TARGET_OFFSET (256 * 1024)

//const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

uint8_t EepromMcuReadBuffer( uint16_t addr, uint8_t *buffer, uint16_t size )
{
    /*
    char print_buf[200];

    for (int i = 0; i < size; i++) {
        buffer[i] = flash_target_contents[addr + i];
        sprintf(print_buf, "%02x", buffer[i]);
        uart_puts(uart1, print_buf);
    }

    sprintf(print_buf, "\r\nEepromMcuReadBuffer read %d bytes at offset %d\r\n", size, addr);
    uart_puts(uart1, print_buf);

    return SUCCESS;
    */
   return FAIL;
}

uint8_t EepromMcuWriteBuffer( uint16_t addr, uint8_t *buffer, uint16_t size )
{
    /*
    char print_buf[200];

    // number of bytes to program must be a multiple of 256 bytes
    int num_blocks = 1 + size / 256;
    uint8_t* temp_buf;

    // sprintf(print_buf, "EepromMcuWriteBuffer erasing 1 sector %d bytes at offset %d\r\n", FLASH_SECTOR_SIZE, FLASH_TARGET_OFFSET);
    // uart_puts(uart1, print_buf);
    // flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    // uart_puts(uart1, "Erased 1 sector\n");
    temp_buf = malloc(num_blocks * 256);
    for (int i = 0; i < num_blocks * 256; i++)
        temp_buf[i] = 0;
    memcpy1(temp_buf, buffer, size);

    sprintf(print_buf, "EepromMcuWriteBuffer attempting to write %d bytes into %d blocks\r\n", size, num_blocks);
    uart_puts(uart1, print_buf);

    flash_range_program(FLASH_TARGET_OFFSET, temp_buf, num_blocks * 256);

    free(temp_buf);

    sprintf(print_buf, "EepromMcuWriteBuffer wrote %d bytes\r\n", size);
    uart_puts(uart1, print_buf);

    return size;
    */
   return FAIL;
}
