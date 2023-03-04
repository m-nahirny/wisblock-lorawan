/*****************************************************************************
* | File      	:   Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-07-31
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_Device.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "rak11310/rak11310.h"
#include "pinName-board.h"

EPD_Device::EPD_Device() 
{
    // spi = new SPI();
}

EPD_Device::~EPD_Device() 
{
	Module_Exit();
    // delete spi;
}

/**
 * GPIO read and write
**/
void EPD_Device::Digital_Write(UWORD Pin, UBYTE Value)
{
	gpio_put(Pin, Value);
}

UBYTE EPD_Device::Digital_Read(UWORD Pin)
{
	return gpio_get(Pin);
}

/**
 * SPI
**/
void EPD_Device::SPI_WriteByte(uint8_t Value)
{
    Gpio_t gpio_nss;
    Spi_t spi0_t;
    gpio_nss.pinIndex = RAK11310_DEFAULT_SPI_CSN_PIN;
    spi0_t.Nss = gpio_nss;
// raspberry pi pico spi1
    spi0_t.SpiId = SPI_1;

        uint16_t bufOut = Value;
        uint16_t bufIn;
        // bufIn = spi->SpiInOut( &spi0_t, bufOut);
        bufIn = SpiInOut( &spi0_t, bufOut);
}

void EPD_Device::SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
    Gpio_t gpio_nss;
    Spi_t spi0_t;
    gpio_nss.pinIndex = RAK11310_DEFAULT_SPI_CSN_PIN;
    spi0_t.Nss = gpio_nss;
// raspberry pi pico spi1
    spi0_t.SpiId = SPI_1;

    uint16_t bufIn;
	for (int i = 0; i < Len; i++) {
		uint16_t bufOut = pData[i];
        // bufIn = spi->SpiInOut( &spi0_t, bufOut);
        bufIn = SpiInOut( &spi0_t, bufOut);
	}
}

// void EPD_Device::SPI_Write_Block(uint8_t *pData, uint32_t Len)
// {
// 	UBYTE Data;
//     Digital_Write(EPD_CS_PIN, 0);
// 	for (int i = 0; i < Len; i++) {
// 		Data = pData[i];
// 		int ret = wiringPi->DisplaySPIDataRW(Data);
// 		if (ret < 0) {
// 			printf("Could not send SPI data\n");
// 			printf("Error: %s\n", strerror(errno));
// 		}
// 	}
// 	// int ret = wiringPiSPIDataRW(0, pData, Len);
// 	// if (ret < 0) {
// 	// 	printf("Could not send SPI data\n");
// 	// 	printf("Error: %s\n", strerror(errno));
// 	// }
//     Digital_Write(EPD_CS_PIN, 1);
// }


/**
 * delay x ms
**/
void EPD_Device::Delay_ms(UDOUBLE xms)
{
	sleep_ms(xms);
}

void EPD_Device::GPIO_Init(void)
{

	// EPD_RST_PIN     = 12;
	// EPD_DC_PIN      = 8;
	// EPD_BUSY_PIN    = 13;
	
	// EPD_CS_PIN      = 9;
	// EPD_CLK_PIN		= 10;
	// EPD_MOSI_PIN	= 11;

	// EPD_RST_PIN     = RAK11310_LED_BLUE;
	EPD_RST_PIN     = RAK11310_IO2_PIN;
	EPD_DC_PIN      = RAK11310_IO1_PIN;
	EPD_BUSY_PIN    = RAK11310_IO6_PIN;
	EPD_CS_PIN      = RAK11310_DEFAULT_SPI_CSN_PIN;

	EPD_CLK_PIN		= RAK11310_DEFAULT_SPI_SCK_PIN;
	EPD_MOSI_PIN	= RAK11310_DEFAULT_SPI_TX_PIN;

	// DEV_GPIO_Mode(EPD_RST_PIN, 1);
	// DEV_GPIO_Mode(EPD_DC_PIN, 1);
	// DEV_GPIO_Mode(EPD_CS_PIN, 1);
	// DEV_GPIO_Mode(EPD_BUSY_PIN, 0);

	// DEV_Digital_Write(EPD_CS_PIN, 1);

    // Reset is active-low, so we'll initialise it to a driven-high state
    gpio_init(EPD_RST_PIN);
    gpio_set_dir(EPD_RST_PIN, GPIO_OUT);
    gpio_put(EPD_RST_PIN, 1);

	// chip select pin
    gpio_init(EPD_CS_PIN);
    gpio_set_dir(EPD_CS_PIN, GPIO_OUT);
    gpio_put(EPD_CS_PIN, 1);

   // Data/command selection
    gpio_init(EPD_DC_PIN);
    gpio_set_dir(EPD_DC_PIN, GPIO_OUT);

    gpio_init(EPD_BUSY_PIN);
    gpio_set_dir(EPD_BUSY_PIN, GPIO_IN);
//	gpio_pull_up(EPD_BUSY_PIN);
	gpio_pull_down(EPD_BUSY_PIN);

}
/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE EPD_Device::Module_Init(void)
{
    // stdio_init_all();

	// GPIO Config
	GPIO_Init();
	
    // spi_init(SPI_PORT, 4000 * 1000);
    // gpio_set_function(EPD_CLK_PIN, GPIO_FUNC_SPI);
    // gpio_set_function(EPD_MOSI_PIN, GPIO_FUNC_SPI);
    Gpio_t gpio_nss;
    Spi_t spi0_t;
    gpio_nss.pin = RAK11310_DEFAULT_SPI_CSN_PIN;
    spi0_t.Nss = gpio_nss;
// raspberry pi pico spi1
    spi0_t.SpiId = SPI_1;
    // printf("Initializing SPI0 \r\n");
    // spi->SpiInit( &spi0_t, SPI_1, RAK11310_DEFAULT_SPI_TX_PIN, RAK11310_DEFAULT_SPI_RX_PIN, RAK11310_DEFAULT_SPI_SCK_PIN, NC );
    SpiInit( &spi0_t, SPI_1, RAK11310_DEFAULT_SPI_TX_PIN, RAK11310_DEFAULT_SPI_RX_PIN, RAK11310_DEFAULT_SPI_SCK_PIN, NC );
	
    // printf("DEV_Module_Init OK \r\n");
	return 0;
}


/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void EPD_Device::Module_Exit(void)
{
	Digital_Write(EPD_CS_PIN, 0);
	Digital_Write(EPD_DC_PIN, 0);
	Digital_Write(EPD_RST_PIN, 0);
	Digital_Write(EPD_CS_PIN, 1);
}
