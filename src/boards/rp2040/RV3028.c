#include "RV3028.h"

#include <stdio.h>

#include "pico/stdlib.h"

//#include "hardware/i2c.h"
#include "D:/Pico/pico-sdk/src/rp2_common/hardware_i2c/include/hardware/i2c.h"


#include "i2c-board.h"


uint8_t readFromRegister(uint8_t registerAddress){
    I2c_t obj;
    uint8_t val;

    I2cMcuWriteBuffer( &obj, RV3028_ADDRESS, &registerAddress, 1 );
    uint8_t result = I2cMcuReadBuffer(&obj, RV3028_ADDRESS, &val, 1 );
    return val;
}

void writeToRegister(uint8_t registerAddress, uint8_t value){
    I2c_t obj;
    uint8_t buf[2];

    buf[0] = registerAddress;
    buf[1] = value;

    I2cMcuWriteBuffer( &obj, RV3028_ADDRESS, buf, 2 );
}

void writeToRegisters(uint8_t startAddress, uint8_t *values, uint8_t length){
    I2c_t obj;
    uint8_t msg[length + 1];

    // Append register address to front of data packet
    msg[0] = startAddress;
    for (int i = 0; i < length; i++) {
        msg[i + 1] = values[i];
    }

    // Write data to register(s) over I2C
    I2cMcuWriteBuffer( &obj, RV3028_ADDRESS, msg, (length + 1) );
}

void andOrRegister(uint8_t registerAddress, uint8_t andValue, uint8_t orValue){
    uint8_t regValue = readFromRegister(registerAddress);
    regValue &= andValue;
    regValue |= orValue;
    writeToRegister(registerAddress, regValue);
}

uint8_t BCDtoDEC(uint8_t bcd){
    return (bcd / 0x10 * 10) + (bcd % 0x10);
}

uint8_t DECtoBCD(uint8_t dec){
    return ((dec / 10) << 4) ^ (dec % 10);
}

uint8_t to12HourFormat(uint8_t bcdHours){
    //PM HOURS bit 5 is 1
    if (BCDtoDEC(bcdHours) > 12){
        bcdHours = DECtoBCD(BCDtoDEC(bcdHours) - 12);
        bcdHours |= PM_FLAG;
    }
    return bcdHours;
}

uint8_t to24HourFormat(uint8_t bcdHours){
    if((bcdHours & PM_FLAG) > 0)
        bcdHours = DECtoBCD(BCDtoDEC(bcdHours & ~PM_FLAG) + 12);

    return bcdHours;
}

uint8_t is12HourMode(){
    return (readFromRegister(CONTROL2_REGISTER_ADDRESS) & AMPM_HOUR_FLAG) > 0;
}

void set12HourMode(){
    //avoid converting alarm hours if they are already in 12 hour format
    if (!is12HourMode()){
        uint8_t alarmHours = readFromRegister(HOURS_ALARM_REGISTER_ADDRESS) & HOURS_ONLY_FILTER_FOR_ALARM;
        writeToRegister(HOURS_ALARM_REGISTER_ADDRESS, to12HourFormat(alarmHours));
    }

    writeToRegister(CONTROL2_REGISTER_ADDRESS, (readFromRegister(CONTROL2_REGISTER_ADDRESS) | AMPM_HOUR_FLAG));
}

void set24HourMode(){
    //avoid converting alarm hours if they are already in 24 hour format
    if (is12HourMode()){
        uint8_t alarmHours = readFromRegister(HOURS_ALARM_REGISTER_ADDRESS) & HOURS_ONLY_FILTER_FOR_ALARM;
        writeToRegister(HOURS_ALARM_REGISTER_ADDRESS, to24HourFormat(alarmHours));
    }

    writeToRegister(CONTROL2_REGISTER_ADDRESS, (readFromRegister(CONTROL2_REGISTER_ADDRESS) & (~AMPM_HOUR_FLAG)));
}

void setTime(uint16_t year, uint8_t month, uint8_t weekday, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){
    uint8_t time_array[] = {DECtoBCD(second), DECtoBCD(minute), DECtoBCD(hour), DECtoBCD(weekday), DECtoBCD(date), DECtoBCD(month), DECtoBCD((uint8_t) (year - 2000))};

    if (is12HourMode()){
        set24HourMode();
        writeToRegisters(SECONDS_REGISTER_ADDRESS, time_array, 7);
        set12HourMode();
    }
    else{
        writeToRegisters(SECONDS_REGISTER_ADDRESS, time_array, 7);
    }

}

uint8_t getSecond(){
    return BCDtoDEC(readFromRegister(SECONDS_REGISTER_ADDRESS));
}

uint8_t getMinute(){
    return BCDtoDEC(readFromRegister(MINUTES_REGISTER_ADDRESS));
}

uint8_t getHour(){
    if (is12HourMode()){
        uint8_t bcdHours = readFromRegister(HOURS_REGISTER_ADDRESS);
        return BCDtoDEC(bcdHours & ~PM_FLAG);
    }
    else {
        return BCDtoDEC(readFromRegister(HOURS_REGISTER_ADDRESS));
    }
}

uint8_t isPM(){
    return (readFromRegister(HOURS_REGISTER_ADDRESS) & PM_FLAG) > 0;
}

uint8_t getWeekday(){
    return BCDtoDEC(readFromRegister(WEEKDAY_REGISTER_ADDRESS));
}

uint8_t getDate(){
    return BCDtoDEC(readFromRegister(DATE_REGISTER_ADDRESS));
}

uint8_t getMonth(){
    return BCDtoDEC(readFromRegister(MONTH_REGISTER_ADDRESS));
}

uint16_t getYear(){
    return BCDtoDEC(readFromRegister(YEAR_REGISTER_ADDRESS)) + 2000;
}

uint32_t getUnixTime(){
    uint32_t unixTime = 0;
    uint8_t b1 = readFromRegister(UNIX_TIME_ADDRESS);
    uint8_t b2 = readFromRegister(UNIX_TIME_ADDRESS + 1);
    uint8_t b3 = readFromRegister(UNIX_TIME_ADDRESS + 2);
    uint8_t b4 = readFromRegister(UNIX_TIME_ADDRESS + 3);
    unixTime = (uint32_t) b1;
    unixTime |= ((uint32_t) b2) << 8;
    unixTime |= ((uint32_t) b3) << 16;
    unixTime |= ((uint32_t) b4) << 24;
    return unixTime;
}

void setUnixTime(uint32_t unixTime)
{
    uint8_t time_array[4];
    time_array[0] = (uint8_t) unixTime;
    time_array[1] = (unixTime >> 8) & 0xFF;
    time_array[2] = (unixTime >> 16) & 0xFF;
    time_array[3] = (unixTime >> 24) & 0xFF;
    writeToRegisters(UNIX_TIME_ADDRESS, time_array, 4);
}

uint8_t isDateModeForAlarm(){
    return (readFromRegister(CONTROL1_REGISTER_ADDRESS) & DATE_ALARM_MODE_FLAG) > 0;
}

void setDateModeForAlarm(uint8_t flag){
    if (flag){
        writeToRegister(CONTROL1_REGISTER_ADDRESS, readFromRegister(CONTROL1_REGISTER_ADDRESS) | DATE_ALARM_MODE_FLAG);
    }
    else {
        writeToRegister(CONTROL1_REGISTER_ADDRESS, readFromRegister(CONTROL1_REGISTER_ADDRESS) & ~DATE_ALARM_MODE_FLAG);
    }
}

void enableAlarm(uint8_t weekdayOrDate, uint8_t hour, uint8_t minute,uint8_t dateAlarm, uint8_t hourAlarm, uint8_t minuteAlarm, uint8_t generateInterrupt){
    //1. Initialize bits AIE and AF to 0.
    //AF
    uint8_t reg_value = readFromRegister(STATUS_REGISTER_ADDRESS) & ~ALARM_FLAG;
    writeToRegister(STATUS_REGISTER_ADDRESS, reg_value);
    //AIE
    reg_value = readFromRegister(CONTROL2_REGISTER_ADDRESS) & ~ALARM_INTERRUPT_FLAG;
    writeToRegister(CONTROL2_REGISTER_ADDRESS, reg_value);

    if (generateInterrupt){
        reg_value = readFromRegister(CONTROL2_REGISTER_ADDRESS) | ALARM_INTERRUPT_FLAG;
        writeToRegister(CONTROL2_REGISTER_ADDRESS, reg_value);
    }

    //2. Alarm settings
    if (dateAlarm)
        writeToRegister(WEEKDAY_DATE_ALARM_REGISTER_ADDRESS, DECtoBCD(weekdayOrDate) & ENABLE_ALARM_FLAG);
    else
        writeToRegister(WEEKDAY_DATE_ALARM_REGISTER_ADDRESS, DECtoBCD(weekdayOrDate) | ~ENABLE_ALARM_FLAG);

    if (hourAlarm)
        writeToRegister(HOURS_ALARM_REGISTER_ADDRESS, DECtoBCD(hour) & ENABLE_ALARM_FLAG);
    else
        writeToRegister(HOURS_ALARM_REGISTER_ADDRESS, DECtoBCD(hour) | ~ENABLE_ALARM_FLAG);

    if (minuteAlarm)
        writeToRegister(MINUTES_ALARM_REGISTER_ADDRESS, DECtoBCD(minute) & ENABLE_ALARM_FLAG);
    else
        writeToRegister(MINUTES_ALARM_REGISTER_ADDRESS, DECtoBCD(minute) | ~ENABLE_ALARM_FLAG);
}

void disableAlarm(){
    writeToRegister(WEEKDAY_DATE_ALARM_REGISTER_ADDRESS, ~ENABLE_ALARM_FLAG);
    writeToRegister(HOURS_ALARM_REGISTER_ADDRESS, ~ENABLE_ALARM_FLAG);
    writeToRegister(MINUTES_ALARM_REGISTER_ADDRESS, ~ENABLE_ALARM_FLAG);
}

void enablePeriodicTimer(uint16_t ticks, TimerClockFrequency freq, uint8_t repeat, uint8_t generateInterrupt){
    //1. set TE, TIE, TF to 0
    //TE
    writeToRegister(CONTROL1_REGISTER_ADDRESS, readFromRegister(CONTROL1_REGISTER_ADDRESS) & ~TIMER_ENABLE_FLAG);
    //TIE
    writeToRegister(CONTROL2_REGISTER_ADDRESS, readFromRegister(CONTROL2_REGISTER_ADDRESS) & ~TIMER_INTERRUPT_ENABLE_FLAG);
    //TF
    writeToRegister(STATUS_REGISTER_ADDRESS, readFromRegister(STATUS_REGISTER_ADDRESS) & ~TIMER_EVENT_FLAG);

    //2. TRPT to 1 for repeat mode and select freq
    if (repeat)
        writeToRegister(CONTROL1_REGISTER_ADDRESS, readFromRegister(CONTROL1_REGISTER_ADDRESS) | TIMER_REPEAT_FLAG | freq);
    else
        writeToRegister(CONTROL1_REGISTER_ADDRESS, (readFromRegister(CONTROL1_REGISTER_ADDRESS) & ~TIMER_REPEAT_FLAG) | freq);

    //set countdown
    uint8_t lsb = (uint8_t) ticks;
    uint8_t msb = (ticks >> 8) & 0x0F;

    writeToRegister(TIMER_VALUE_0_ADDRESS, lsb);
    writeToRegister(TIMER_VALUE_1_ADDRESS, msb);

    //hardware interrupt
    if (generateInterrupt)
        writeToRegister(CONTROL2_REGISTER_ADDRESS, readFromRegister(CONTROL2_REGISTER_ADDRESS) | TIMER_INTERRUPT_ENABLE_FLAG);
    else
        writeToRegister(CONTROL2_REGISTER_ADDRESS, readFromRegister(CONTROL2_REGISTER_ADDRESS) & ~TIMER_INTERRUPT_ENABLE_FLAG);

    //enable timer
    writeToRegister(CONTROL1_REGISTER_ADDRESS, readFromRegister(CONTROL1_REGISTER_ADDRESS) | TIMER_ENABLE_FLAG);

}

void disablePeriodicTimer(){
    writeToRegister(CONTROL1_REGISTER_ADDRESS, readFromRegister(CONTROL1_REGISTER_ADDRESS) & ~TIMER_ENABLE_FLAG);
}

void enablePeriodicTimeUpdate(uint8_t everySecond, uint8_t generateInterrupt){
    uint8_t orValue = everySecond ? 0 : 0x10;
    andOrRegister(CONTROL1_REGISTER_ADDRESS, 0xFF, orValue);
    if (generateInterrupt) 
        andOrRegister(CONTROL2_REGISTER_ADDRESS, 0xFF, 0x20);
    else 
        andOrRegister(CONTROL2_REGISTER_ADDRESS, 0xDF, 0);
}

void disablePeriodicTimeUpdate(){
    enablePeriodicTimeUpdate(true, false);
}

void clearInterruptFlags(uint8_t clearTimerFlag, uint8_t clearAlarmFlag, uint8_t clearPeriodicTimeUpdateFlag){
    uint8_t mask = 0xFF;
    mask = clearTimerFlag ? mask & 0xF7 : mask;
    mask = clearAlarmFlag ? mask & 0xFB : mask;
    mask = clearPeriodicTimeUpdateFlag ? mask & 0xEF : mask;
    andOrRegister(STATUS_REGISTER_ADDRESS, mask, 0);
}

// Sets up the device to read/write from/to the eeprom memory. The automatic refresh function has to be disabled.
void useEEPROM(uint8_t disableRefresh){
    if (disableRefresh){
        andOrRegister(CONTROL1_REGISTER_ADDRESS, 0xFF, 0x08);
    }
    else
        andOrRegister(CONTROL1_REGISTER_ADDRESS, 0xF7, 0);
}

uint8_t isEEPROMBusy(){
    return (readFromRegister(STATUS_REGISTER_ADDRESS) & 0x80) > 0;
}

/* Reads an eeprom register and returns its content.
 * user eeprom address space : [0x00 - 0x2A]
 * configuration eeprom address space : [0x30 - 0x37] */
uint8_t readEEPROMRegister(uint8_t registerAddress){
    useEEPROM(1);

    while (isEEPROMBusy());
    // data sheet indicates that before any other command a 0 must be written 
    writeToRegister(EEPROM_COMMAND_ADDRESS, 0);

    while (isEEPROMBusy());
    // read a register -> eeprom data = 0x22
    writeToRegister(EEPROM_ADDRESS_ADDRESS, registerAddress);
    writeToRegister(EEPROM_COMMAND_ADDRESS, 0x22);
    while (isEEPROMBusy());
    return readFromRegister(EEPROM_DATA_ADDRESS);
}

/* Writes value to the eeprom register at address register_address.
 * user eeprom address space : [0x00 - 0x2A]
 * configuration eeprom address space : [0x30 - 0x37] */
void writeEEPROMRegister(uint8_t registerAddress, uint8_t value){
    useEEPROM(1);

    while (isEEPROMBusy());
    
    // data sheet indicates that before any other command a 0 must be written 
    writeToRegister(EEPROM_COMMAND_ADDRESS, 0);

    while (isEEPROMBusy());

    writeToRegister(EEPROM_ADDRESS_ADDRESS, registerAddress);
    writeToRegister(EEPROM_DATA_ADDRESS, value);

    // write to a register in eeprom = 0x21
    writeToRegister(EEPROM_COMMAND_ADDRESS, 0x21);
}
