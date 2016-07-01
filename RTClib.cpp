// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#include <Arduino.h>

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#include "RTClib.h"

////////////////////////////////////////////////////////////////////////////////
//
// TODO OMG this date handling is sorta embarassing. Internal representation
// should be unix epoch only and conversion to year/month/day should be on the
// fly and not cached.
//
////////////////////////////////////////////////////////////////////////////////

const uint8_t daysInMonth[] PROGMEM = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
const char* months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

// number of days since 2000/01/01, valid for 2001..2099
uint16_t date2days(uint16_t y, uint8_t m, uint8_t d)
{
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s)
{
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

uint8_t conv2d(const char* p)
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

uint8_t bcd2bin (uint8_t val)
{
    return val - 6 * (val >> 4);
}
uint8_t bin2bcd (uint8_t val)
{
    return val + 6 * (val / 10);
}

////////////////////////////////////////////////////////////////////////////////
// RTC_Millis implementation

RTC_Millis::RTC_Millis()
{
    adjust(DateTime(2000, 1, 1, 0, 0, 0));
}

bool RTC_Millis::begin()
{
    return 1;
}

bool RTC_Millis::begin(const DateTime& dt)
{
    adjust(dt);
    return 1;
}

//
// Set time
//
void RTC_Millis::adjust(const DateTime& dt)
{
    offset = dt.unixtime() - millis() / 1000;
}

//
// Enable RTC if not already enabled. No-op for this RTC.
//
bool RTC_Millis::enable(void)
{
    return true;
}

//
// Is RTC running? Always true for this pseudo-RTC.
//
bool RTC_Millis::isrunning(void)
{
    return true;
}

DateTime RTC_Millis::now()
{
    return (uint32_t)(offset + millis() / 1000);
}

////////////////////////////////////////////////////////////////////////////////
// vim:ci:sw=4 sts=4 ft=cpp
