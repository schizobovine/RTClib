// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#ifndef __RTCLIB_H__
#define __RTCLIB_H__

#include <DateTime.h>

//
// Constant lookup tables
//
extern const uint8_t daysInMonth[] PROGMEM;
extern const char* months[];

//
// Utility functions
//

uint16_t date2days(uint16_t y, uint8_t m, uint8_t d);
long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s);
uint8_t conv2d(const char* p);
uint8_t bcd2bin (uint8_t val);
uint8_t bin2bcd (uint8_t val);

//
// Base RTC class to wrangle all the chips into one interface
// 
class RTC_Base
{
    public:
        RTC_Base(void) {};
        bool begin(void) { return true; };
        void adjust(const DateTime& dt) {};
        bool isrunning(void) { return false; };
        DateTime now(void) { return DateTime(); };

};

//
// RTC using the internal millis() clock, has to be initialized before use
// NOTE: this clock won't be correct once the millis() timer rolls over (>49d?)
// TODO This should be fixable?
//
class RTC_Millis : public RTC_Base
{
public:
    RTC_Millis(void);
    bool begin(void);
    bool begin(const DateTime& dt);
    void adjust(const DateTime& dt);
    bool enable(void);
    bool isrunning(void);
    DateTime now();

protected:
    long offset;
};

#endif // __RTCLIB_H__

// vim:ci:sw=4 sts=4 ft=cpp
