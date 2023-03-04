#include "Lis3dh.h"

#include <math.h>

Lis3dh::Lis3dh()
{

}

Lis3dh::~Lis3dh()
{

}

uint8_t Lis3dh::ReadFromRegister(uint8_t registerAddress){
    uint8_t val;

    I2cMcuWriteBuffer( &i2c_obj, LIS3DH_ADDRESS, &registerAddress, 1 );
    uint8_t result = I2cMcuReadBuffer(&i2c_obj, LIS3DH_ADDRESS, &val, 1 );
    return val;
}

void Lis3dh::WriteToRegister(uint8_t registerAddress, uint8_t value){
    uint8_t buf[2];

    buf[0] = registerAddress;
    buf[1] = value;

    I2cMcuWriteBuffer( &i2c_obj, LIS3DH_ADDRESS, buf, 2 );
}

void Lis3dh::Init()
{
    uint8_t buf[2];

    // buf[0] = TEMP_CFG_REG;
    // // enable temperature sensor
    // buf[1] = 0xC0;
    // i2c_write_blocking(i2c_rak, ADDRESS, buf, 2, false);

    buf[0] = CTRL_REG_1;
    // 200 Hz high performance mode
    buf[1] = DATARATE_200_HR;
    // 1.344 kHz HR mode
    // buf[1] = 0x97;
    // i2c_write_blocking(i2c_rak, ADDRESS, buf, 2, false);
    I2cMcuWriteBuffer( &i2c_obj, LIS3DH_ADDRESS, buf, 2 );

    buf[0] = CTRL_REG_4;
    // +/- 2g, high resolution
    // Turn block data update on (for temperature sensing)
    buf[1] = 0x88;
    // i2c_write_blocking(i2c_rak, ADDRESS, buf, 2, false);
    I2cMcuWriteBuffer( &i2c_obj, LIS3DH_ADDRESS, buf, 2 );

    // Configure ADC on RP2040 to read its temperature sensor
//    adc_init();
//    adc_set_temp_sensor_enabled(true);
//    adc_select_input(4);
}

float Lis3dh::ConvertValue(uint16_t raw_value)
{
    float finalValue;
    float scaling;
    float senstivity = 0.004f; // g per unit

    scaling = 64 / senstivity;

    // raw_value is signed
    finalValue = (float) ((int16_t) raw_value) / scaling;
    finalValue = round(finalValue * 100) / 100;
    // // in HR 2G mode 1 digit = 1/1024 mg, full scale is 4096 = 4000 mg
    // finalValue = 1000 * raw_value;
    return finalValue;
}

float Lis3dh::ReadData(uint8_t reg)
{
    // Read two bytes of data and store in a 16 bit data structure
    uint8_t lsb;
    uint8_t msb;
    uint16_t raw_accel;
    // i2c_write_blocking(i2c_rak, ADDRESS, &reg, 1, true);
    // int numRead = i2c_read_blocking(i2c_rak, ADDRESS, &lsb, 1, false);
    // if (numRead == PICO_ERROR_GENERIC)
    //     puts("ReadData - i2c_read_blocking error");

    lsb = ReadFromRegister(reg);

    reg |= 0x01;
    // i2c_write_blocking(i2c_rak, ADDRESS, &reg, 1, true);
    // numRead = i2c_read_blocking(i2c_rak, ADDRESS, &msb, 1, false);
    // if (numRead == PICO_ERROR_GENERIC)
    //     puts("ReadData - i2c_read_blocking error 2");
    msb = ReadFromRegister(reg);

    raw_accel = (msb << 8) | lsb;
    short val = raw_accel;
    float g = (float)val / 1024;
    // printf("Raw data %d, converted %f\r\n", val, g / 16);
    // return ConvertValue(raw_accel);
    return g / 16;
}

uint8_t Lis3dh::WhoAmI()
{
    uint8_t val;
    // uint8_t reg = WHO_AM_I;

    // int numWritten = i2c_write_blocking(i2c_rak, ADDRESS, &reg, 1, true);
    // if (numWritten == PICO_ERROR_GENERIC)
    //     puts("WhoAmI - i2c_write_blocking error");
    // int numRead = i2c_read_blocking(i2c_rak, ADDRESS, &val, 1, false);
    // if (numRead == PICO_ERROR_GENERIC)
    //     puts("WhoAmI - i2c_read_blocking error");

    val = ReadFromRegister(WHO_AM_I);
    return val;
}

uint8_t Lis3dh::DataAvailable()
{
    uint8_t val;
    // uint8_t reg = STATUS_REG;

    // int numWritten = i2c_write_blocking(i2c_rak, ADDRESS, &reg, 1, true);
    // if (numWritten == PICO_ERROR_GENERIC)
    //     puts("DataAvailable - i2c_write_blocking error");
    // int numRead = i2c_read_blocking(i2c_rak, ADDRESS, &val, 1, false);
    // if (numRead == PICO_ERROR_GENERIC)
    //     puts("DataAvailable - i2c_read_blocking error");

    val = ReadFromRegister(STATUS_REG);
    val = val >> 3;
    return val & 0x01;
}