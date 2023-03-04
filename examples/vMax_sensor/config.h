/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

// LoRaWAN region to use, full list of regions can be found at: 
//   http://stackforce.github.io/LoRaMac-doc/LoRaMac-doc-v4.5.1/group___l_o_r_a_m_a_c.html#ga3b9d54f0355b51e85df8b33fd1757eec
#define LORAWAN_REGION          LORAMAC_REGION_US915

// LoRaWAN Device EUI (64-bit), NULL value will use Default Dev EUI
#define LORAWAN_DEVICE_EUI      "D3AFA8E7F553596C"

// LoRaWAN Application / Join EUI (64-bit)
#define LORAWAN_APP_EUI         "B827EBFFFE390000"

// LoRaWAN Application Key (128-bit)
#define LORAWAN_APP_KEY         "9A3098588393454588F854978BA0E5A1"

// LoRaWAN Channel Mask, NULL value will use the default channel mask 
// for the region
#define LORAWAN_CHANNEL_MASK    NULL

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
