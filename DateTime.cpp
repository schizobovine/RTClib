// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#include <Arduino.h>

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#include "RTClib.h"
#include "DateTime.h"

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

DateTime::DateTime (uint32_t t)
{
    t -= RTC_SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff)
    {
        leap = yOff % 4 == 0;
        if (days < 365U + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m)
    {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = min(month,12);
    d = min(day,31);
    hh = min(hour,23);
    mm = min(min,60);
    ss = min(sec,60);
}

// A convenient constructor for using "the compiler's time":
//   DateTime now (__DATE__, __TIME__);
// NOTE: using PSTR would further reduce the RAM footprint
DateTime::DateTime (const char* date, const char* time)
{
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    switch (date[0])
    {
    case 'J':
        if ( date[1] == 'a' )
	    m = 1;
	else if ( date[2] == 'n' )
	    m = 6;
	else
	    m = 7;
        break;
    case 'F':
        m = 2;
        break;
    case 'A':
        m = date[2] == 'r' ? 4 : 8;
        break;
    case 'M':
        m = date[2] == 'r' ? 3 : 5;
        break;
    case 'S':
        m = 9;
        break;
    case 'O':
        m = 10;
        break;
    case 'N':
        m = 11;
        break;
    case 'D':
        m = 12;
        break;
    }
    d = conv2d(date + 4);
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

// Make a DateTime from another DateTime
DateTime::DateTime (const DateTime &dt)
{
    yOff = dt.yOff;
    m    = dt.m;
    d    = dt.d;
    hh   = dt.hh;
    mm   = dt.mm;
    ss   = dt.ss;
}

uint8_t DateTime::dayOfWeek() const
{
    uint16_t day = date2days(yOff, m, d);
    return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

uint32_t DateTime::unixtime(void) const
{
    uint32_t t;
    uint16_t days = date2days(yOff, m, d);
    t = time2long(days, hh, mm, ss);
    t += RTC_SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000

    return t;
}

// as a string
char* DateTime::toString(char* buf, int maxlen) const
{
    snprintf(buf,maxlen,"%s %02u %04u %02u:%02u:%02u",
             months[m-1],
             d,
             2000 + yOff,
             hh,
             mm,
             ss
            );
    return buf;
}

void DateTime::operator+=(uint32_t additional)
{
    DateTime after = DateTime( unixtime() + additional );
    *this = after;
}

////////////////////////////////////////////////////////////////////////////////
// vim:ci:sw=4 sts=4 ft=cpp
