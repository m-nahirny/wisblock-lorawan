#ifndef DEV_CONFIG_H_
#define DEV_CONFIG_H_

#include "stdio.h"
#include "spi.h"

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

class EPD_Device
{
    public:
        EPD_Device();
        ~EPD_Device();

        void Digital_Write(UWORD Pin, UBYTE Value);
        UBYTE Digital_Read(UWORD Pin);

        void SPI_WriteByte(UBYTE Value);
        void SPI_Write_nByte(uint8_t *pData, uint32_t Len);
        // void SPI_Write_Block(uint8_t *pData, uint32_t Len);

        UBYTE Module_Init(void);
        void Module_Exit(void);

        void Delay_ms(UDOUBLE xms);

        UBYTE EPD_RST_PIN;
        UBYTE EPD_DC_PIN;
        UBYTE EPD_CS_PIN;
        UBYTE EPD_BUSY_PIN;
        UBYTE EPD_CLK_PIN;
        UBYTE EPD_MOSI_PIN;

    private:
        void GPIO_Mode(UWORD Pin, UWORD Mode);
        void GPIO_Init(void);

        // SPI *spi;
};

#endif
