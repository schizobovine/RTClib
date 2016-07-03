// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#ifndef __RTC_DS3234_H__
#define __RTC_DS3234_H__

#include <RTClib.h>

// RTC based on the DS3234 chip connected via SPI and the SPI library
class RTC_DS3234 : public RTC_Base
{
public:
    RTC_DS3234(int _cs_pin): cs_pin(_cs_pin) {}
    bool begin(void);
    void adjust(const DateTime& dt);
    bool enable(void);
    bool isrunning(void);
    DateTime now();

protected:
    void cs(int _value);

private:
    int cs_pin;
};

#endif // __RTC_DS3234_H__

// vim:ai:cin:sw=4 sts=4 ft=cpp
