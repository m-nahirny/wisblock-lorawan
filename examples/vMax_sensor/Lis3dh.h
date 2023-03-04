#ifndef LIS3DH_H_
#define LIS3DH_H_

#include <stdio.h>
#include "hardware/i2c.h"
#include "i2c-board.h"

// By default this device is on bus address 0x18

const int LIS3DH_ADDRESS = 0x18;
const uint8_t TEMP_CFG_REG = 0x1F;
const uint8_t CTRL_REG_1 = 0x20;
const uint8_t CTRL_REG_2 = 0x21;
const uint8_t CTRL_REG_3 = 0x22;
const uint8_t CTRL_REG_4 = 0x23;
const uint8_t CTRL_REG_6 = 0x25;
const uint8_t WHO_AM_I = 0x0F;
const uint8_t OUT_T_L = 0x0C; // 16 bit resolution
const uint8_t OUT_T_H = 0x0D; // 16 bit resolution
const uint8_t OUT_T = 0x26; // 8 bit resolution
const uint8_t STATUS_REG = 0x27;
const uint8_t OUT_X_L = 0x28; 
const uint8_t OUT_X_H = 0x29; 
const uint8_t OUT_Y_L = 0x2A; 
const uint8_t OUT_Y_H = 0x2B; 
const uint8_t OUT_Z_L = 0x2C; 
const uint8_t OUT_Z_H = 0x2D; 
//const uint8_t TEMP_CFG_REG = 0xC0;

#define DATARATE_100_HR 0x57;
#define DATARATE_200_HR 0x67;
#define DATARATE_400_HR 0x77;
#define DATARATE_1344_HR 0x97;

#define i2c_rak i2c1

class Lis3dh
{
    public:
        Lis3dh();
        ~Lis3dh();

        void Init();
        float ConvertValue(uint16_t raw_value);
        float ReadData(uint8_t reg);
        uint8_t WhoAmI();
        uint8_t DataAvailable();

    private:
        uint8_t ReadFromRegister(uint8_t registerAddress);
        void WriteToRegister(uint8_t registerAddress, uint8_t value);

        I2c_t i2c_obj;
};

#endif // LIS3DH_H_