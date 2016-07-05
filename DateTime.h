// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#ifndef __DATETIME_H
#define __DATETIME_H

#include <time.h>

#define UNIX_EPOCH_OFFSET 946684800

//
// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
//

class DateTime {
    public:

        DateTime (uint32_t t =0);
        DateTime (uint16_t year, uint8_t month, uint8_t day,
                  uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
        DateTime (const char* date, const char* time);
        DateTime (const DateTime &dt);

        uint16_t year() const
        {
            return 2000 + yOff;
        }

        uint8_t month() const
        {
            return m;
        }

        uint8_t day() const
        {
            return d;
        }

        uint8_t hour() const
        {
            return hh;
        }

        uint8_t minute() const
        {
            return mm;
        }

        uint8_t second() const
        {
            return ss;
        }

        uint8_t dayOfWeek() const;

        // 32-bit times as seconds since 1/1/2000
        long secondstime() const;

        // 32-bit times as seconds since 1/1/1970
        uint32_t unixtime(void) const;

        // as a string
        char* toString(char* buf, size_t maxlen) const;
        String toString();
        String toString(String &s);
        String iso8601();
        String iso8601(String &s);

        // add additional time
        void operator+=(uint32_t);

    protected:
        uint8_t yOff, m, d, hh, mm, ss;

};

#endif

// vim:ci:sw=4 sts=4 ft=cpp
