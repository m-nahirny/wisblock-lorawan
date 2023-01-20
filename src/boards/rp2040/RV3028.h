#ifndef RV3028_H_INCLUDED
#define RV3028_H_INCLUDED

#include <stdint.h>

#define RV3028_ADDRESS (uint8_t) 0b1010010

#define SECONDS_REGISTER_ADDRESS 0x00
#define MINUTES_REGISTER_ADDRESS 0x01
#define HOURS_REGISTER_ADDRESS 0x02
#define WEEKDAY_REGISTER_ADDRESS 0x03
#define DATE_REGISTER_ADDRESS 0x04
#define MONTH_REGISTER_ADDRESS 0x05
#define YEAR_REGISTER_ADDRESS 0x06

#define UNIX_TIME_ADDRESS 0x1B

#define AMPM_HOUR_FLAG 0b00000010
#define PM_FLAG 0b00100000
#define HOURS_ONLY_FILTER_FOR_ALARM 0b00111111

#define MINUTES_ALARM_REGISTER_ADDRESS 0x07
#define HOURS_ALARM_REGISTER_ADDRESS 0x08
#define WEEKDAY_DATE_ALARM_REGISTER_ADDRESS 0x09
#define DATE_ALARM_MODE_FLAG 0b00100000
#define ENABLE_ALARM_FLAG 0b01111111
#define ALARM_FLAG 0b00000100
#define ALARM_INTERRUPT_FLAG 0b00001000

#define TIMER_ENABLE_FLAG 0b00000100
#define TIMER_INTERRUPT_ENABLE_FLAG 0b00010000
#define TIMER_EVENT_FLAG 0b00001000
#define TIMER_REPEAT_FLAG 0b10000000
#define TIMER_VALUE_0_ADDRESS 0x0A
#define TIMER_VALUE_1_ADDRESS 0x0B

#define STATUS_REGISTER_ADDRESS 0x0E

#define CONTROL1_REGISTER_ADDRESS 0x0F
#define CONTROL2_REGISTER_ADDRESS 0x10

#define USER_RAM1_ADDRESS 0x1F
#define USER_RAM2_ADDRESS 0x20

#define EEPROM_ADDRESS_ADDRESS 0x25
#define EEPROM_DATA_ADDRESS 0x26
#define EEPROM_COMMAND_ADDRESS 0x27


// enum TimerClockFrequency : uint8_t {
//         Hz4096 = 0x00,
//         Hz64 = 0x01,
//         Hz1 = 0x02,
//         Hz1_60 = 0x03
// };

typedef enum {
    Hz4096, Hz64, Hz1, Hz1_60
} TimerClockFrequency;

        uint8_t getSecond();
        uint8_t getMinute();
        uint8_t getHour();

        uint8_t getWeekday();
        uint8_t getDate();
        uint8_t getMonth();
        uint16_t getYear();

        uint32_t getUnixTime();

        void set24HourMode();
        void set12HourMode();
        uint8_t is12HourMode();
        uint8_t isPM();

        //time is always set as 24 hours time!
        void setTime(uint16_t year, uint8_t month, uint8_t weekday, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
        //automatically sets the time
        //void setTime(); TODO: implement

        /*
        The alarm can be set for minutes, hours and date or weekday. Any combination of those can be
        selected.
        */
        uint8_t isDateModeForAlarm();
        void setDateModeForAlarm(uint8_t flag);
        void enableAlarm(uint8_t weekdayOrDate, uint8_t hour, uint8_t minute,
            uint8_t dateAlarm, uint8_t hourAlarm, uint8_t minuteAlarm, uint8_t generateInterrupt);
        void disableAlarm();

        void enablePeriodicTimer(uint16_t ticks, TimerClockFrequency freq, uint8_t repeat, uint8_t generateInterrupt);  
        void disablePeriodicTimer();

        //everySecond: if True the periodic time update triggers every second. If False it triggers every minute.
        void enablePeriodicTimeUpdate(uint8_t everySecond, uint8_t generateInterrupt);
        void disablePeriodicTimeUpdate();

        void clearInterruptFlags(uint8_t clearTimerFlag, uint8_t clearAlarmFlag, uint8_t clearPeriodicTimeUpdateFlag);

        //TODO:
        //void reset();
        // refactor code to use more andOrRegister instead of a read and a write


        uint8_t readFromRegister(uint8_t registerAddress);
        void writeToRegister(uint8_t registerAddress, uint8_t value);
        void writeToRegisters(uint8_t startAddress, uint8_t *values, uint8_t length);
        void andOrRegister(uint8_t registerAddress, uint8_t andValue, uint8_t orValue);

        // Sets up the device to read/write from/to the eeprom memory. The automatic refresh function has to be disabled.
        void useEEPROM(uint8_t disableRefresh);
        uint8_t isEEPROMBusy();
        uint8_t readEEPROMRegister(uint8_t registerAddress);
        void writeEEPROMRegister(uint8_t registerAddress, uint8_t value);

        uint8_t BCDtoDEC(uint8_t bcd);
        uint8_t DECtoBCD(uint8_t dec);

        uint8_t to12HourFormat(uint8_t bcdHours);
        uint8_t to24HourFormat(uint8_t bcdHours);

#endif // RV3028_H_INCLUDED