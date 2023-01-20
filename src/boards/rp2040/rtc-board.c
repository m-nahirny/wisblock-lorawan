/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include <stdio.h>

#include "pico/time.h"
#include "pico/stdlib.h"

#include "rtc-board.h"

#include "RV3028.h"

static alarm_pool_t* rtc_alarm_pool = NULL;
static absolute_time_t rtc_timer_context;
static alarm_id_t last_rtc_alarm_id = -1;

void RtcInit( void )
{
    // disable automatic refresh on the RTC NVM
    useEEPROM(1);

    rtc_alarm_pool = alarm_pool_create(2, 16);

    RtcSetTimerContext();
}

uint32_t RtcGetCalendarTime( uint16_t *milliseconds )
{
    uint32_t now = to_ms_since_boot(get_absolute_time());

    *milliseconds = (now % 1000);

    return (now / 1000);
}

uint32_t RtcGetTimerElapsedTime( void )
{
    int64_t delta = absolute_time_diff_us(rtc_timer_context, get_absolute_time());

    return delta;
}

uint32_t RtcSetTimerContext( void )
{
    rtc_timer_context = get_absolute_time();

    uint64_t ticks = to_us_since_boot(rtc_timer_context);

    return ticks;
}

uint32_t RtcGetTimerContext( void )
{
    uint64_t ticks = to_us_since_boot(rtc_timer_context);

    return ticks;
}

uint32_t RtcGetMinimumTimeout( void )
{
    return 1;
}

static int64_t alarm_callback(alarm_id_t id, void *user_data) {
    TimerIrqHandler( );

    return 0;
}

void RtcSetAlarm( uint32_t timeout )
{
    if (last_rtc_alarm_id > -1) {
        alarm_pool_cancel_alarm(rtc_alarm_pool, last_rtc_alarm_id);
    }

    last_rtc_alarm_id = alarm_pool_add_alarm_at(rtc_alarm_pool, delayed_by_us(rtc_timer_context, timeout), alarm_callback, NULL, true);
}

void RtcStopAlarm( void )
{
    if (last_rtc_alarm_id > -1) {
        alarm_pool_cancel_alarm(rtc_alarm_pool, last_rtc_alarm_id);
    }
}

uint32_t RtcMs2Tick( TimerTime_t milliseconds )
{
    return milliseconds * 1000;
}

uint32_t RtcGetTimerValue( void )
{
    uint64_t now = to_us_since_boot(get_absolute_time());

    return now;
}

TimerTime_t RtcTick2Ms( uint32_t tick )
{
    return us_to_ms(tick);
}

void RtcBkupRead( uint32_t *data0, uint32_t *data1 )
{
    uint8_t val1 = readEEPROMRegister(0);
    uint8_t val2 = readEEPROMRegister(1);
    uint8_t val3 = readEEPROMRegister(2);
    uint8_t val4 = readEEPROMRegister(3);
    *data0 = val1 << 8 + val2;
    *data1 = val3 << 8 + val4;
}

void RtcBkupWrite( uint32_t data0, uint32_t data1 )
{
    uint8_t val1 = data0 >> 8 & 0xFF;
    uint8_t val2 = data0 & 0xFF;
    uint8_t val3 = data1 >> 8 & 0xFF;
    uint8_t val4 = data1 & 0xFF;
    writeEEPROMRegister(0, val1);
    writeEEPROMRegister(1, val2);
    writeEEPROMRegister(2, val3);
    writeEEPROMRegister(3, val4);
}

// addr sould be past 3 since 0 to 3 are used for temp time backup
void RtcEEPROMWrite( uint8_t addr, uint8_t data )
{
    writeEEPROMRegister(addr, data);
}

uint8_t RtcEEPROMRead( uint8_t addr )
{
    return readEEPROMRegister(addr);
}

void RtcProcess( void )
{
    // Not used on this platform.
}
