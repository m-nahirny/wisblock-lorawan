/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// -----------------------------------------------------
// NOTE: THIS HEADER IS ALSO INCLUDED BY ASSEMBLER SO
//       SHOULD ONLY CONSIST OF PREPROCESSOR DIRECTIVES
// -----------------------------------------------------

// This header may be included by other board headers as "boards/rak11310.h"

#ifndef _BOARDS_RAK11310_H
#define _BOARDS_RAK11310_H

// For board detection
#define WISBLOCK_RAK11310

// --- UART ---
#ifndef RAK11310_UART0TX
#define RAK11310_UART0TX GPIO_0
#endif
#ifndef RAK11310_UART0RX
#define RAK11310_UART0RX GPIO_1
#endif
#ifndef RAK11310_UART1TX
#define RAK11310_UART1TX GPIO_4
#endif
#ifndef RAK11310_UART1RX
#define RAK11310_UART1RX GPIO_5
#endif

// --- LED ---
#ifndef RAK11310_LED_GREEN
#define RAK11310_LED_GREEN GPIO_23
#endif
#ifndef RAK11310_LED_BLUE
#define RAK11310_LED_BLUE GPIO_24
#endif
// no RAK11310_DEFAULT_WS2812_PIN

// --- I2C ---
#ifndef RAK11310_DEFAULT_I2C
#define RAK11310_DEFAULT_I2C 0
#endif
#ifndef RAK11310_DEFAULT_I2C_SDA_PIN
#define RAK11310_DEFAULT_I2C_SDA_PIN GPIO_20
#endif
#ifndef RAK11310_DEFAULT_I2C_SCL_PIN
#define RAK11310_DEFAULT_I2C_SCL_PIN GPIO_21
#endif

// --- SPI ---
#ifndef RAK11310_DEFAULT_SPI
#define RAK11310_DEFAULT_SPI 0
#endif
#ifndef RAK11310_DEFAULT_SPI_SCK_PIN
#define RAK11310_DEFAULT_SPI_SCK_PIN GPIO_18
#endif
#ifndef RAK11310_DEFAULT_SPI_TX_PIN
#define RAK11310_DEFAULT_SPI_TX_PIN GPIO_19
#endif
#ifndef RAK11310_DEFAULT_SPI_RX_PIN
#define RAK11310_DEFAULT_SPI_RX_PIN GPIO_16
#endif
#ifndef RAK11310_DEFAULT_SPI_CSN_PIN
#define RAK11310_DEFAULT_SPI_CSN_PIN GPIO_17
#endif

// --- I/O ---
#ifndef RAK11310_IO1_PIN
#define RAK11310_IO1_PIN GPIO_6
#endif
#ifndef RAK11310_IO2_PIN
#define RAK11310_IO2_PIN GPIO_22
#endif
#ifndef RAK11310_IO3_PIN
#define RAK11310_IO3_PIN GPIO_7
#endif
#ifndef RAK11310_IO4_PIN
#define RAK11310_IO4_PIN GPIO_28
#endif
#ifndef RAK11310_IO5_PIN
#define RAK11310_IO5_PIN GPIO_9
#endif
#ifndef RAK11310_IO6_PIN
#define RAK11310_IO6_PIN GPIO_8
#endif


// --- FLASH ---

#define RAK11310_BOOT_STAGE2_CHOOSE_W25Q080 1

#ifndef RAK11310_FLASH_SPI_CLKDIV
#define RAK11310_FLASH_SPI_CLKDIV 2
#endif

#ifndef RAK11310_FLASH_SIZE_BYTES
#define RAK11310_FLASH_SIZE_BYTES (2 * 1024 * 1024)
#endif

// Drive high to force power supply into PWM mode (lower ripple on 3V3 at light loads)
#define RAK11310_SMPS_MODE_PIN GPIO_23

#ifndef RAK11310_RP2040_B0_SUPPORTED
#define RAK11310_RP2040_B0_SUPPORTED 1
#endif

#endif
