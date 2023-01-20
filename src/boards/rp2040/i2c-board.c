/**
 * @file      i2c-board.c
 *
 * @brief     Target board I2C driver implementation
 *
 * @copyright Copyright (c) 2020 The Things Industries B.V.
 *
 * Revised BSD License
 * Copyright The Things Industries B.V 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Things Industries B.V nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE THINGS INDUSTRIES B.V BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "D:/Pico/pico-sdk/src/rp2_common/hardware_i2c/include/hardware/i2c.h"
//#include "hardware/i2c.h"

#include <stdio.h>

#include "pico/stdlib.h"

#include "board.h"
#include "i2c-board.h"

const uint I2C0SDA = 20;
const uint I2C0SCL = 21;
const uint I2C1SDA = 2;
const uint I2C1SCL = 3;

#define i2c_rak i2c1

void I2cMcuInit( I2c_t* obj, I2cId_t i2cId, PinNames scl, PinNames sda )
{
    gpio_set_function(I2C1SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C1SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C1SDA);
    gpio_pull_up(I2C1SCL);

    uint res = i2c_init(i2c_rak, 400 * 1000);
    char print_buf[200];
    sprintf(print_buf, "I2C rate is %d\r\n", res);
    uart_puts(uart1, print_buf);
}

void I2cMcuDeInit( I2c_t* obj )
{
    // Left empty
}

void I2cMcuFormat( I2c_t* obj, I2cMode mode, I2cDutyCycle dutyCycle, bool I2cAckEnable, I2cAckAddrMode AckAddrMode,
                   uint32_t I2cFrequency )
{
    // configured via hpl_sercom_config.h
    return;
}

uint8_t I2cMcuWriteBuffer( I2c_t *obj, uint8_t deviceAddr, uint8_t *buffer, uint16_t size )
{
    // Write data to register(s) over I2C
    i2c_write_blocking(i2c_rak, deviceAddr, buffer, size, false);

    return 1;  // ok
}

uint8_t I2cMcuReadBuffer( I2c_t *obj, uint8_t deviceAddr, uint8_t *buffer, uint16_t size )
{
    int numRead = i2c_read_blocking(i2c_rak, deviceAddr, buffer, size, false);
    if (numRead == PICO_ERROR_GENERIC)
    {
        return 0;  // something went wrong
    }

    return 1;  // ok
}

uint8_t I2cMcuWriteMemBuffer( I2c_t* obj, uint8_t deviceAddr, uint16_t addr, uint8_t* buffer, uint16_t size )
{
    return 0;  // something went wrong
}

uint8_t I2cMcuReadMemBuffer( I2c_t* obj, uint8_t deviceAddr, uint16_t addr, uint8_t* buffer, uint16_t size )
{
    return 0;  // something went wrong
}