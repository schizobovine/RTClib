// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#include <Arduino.h>

#ifdef __AVR_ATtiny85__
#include <TinyWireM.h>
#define Wire TinyWireM
#else
#include <Wire.h>
#endif

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#include "RTClib.h"
#include "DS1307.h"

#define SEND(x) write(static_cast<uint8_t>(x))
#define RECEIVE(x) read(x)

////////////////////////////////////////////////////////////////////////////////
// RTC_DS1307 implementation

bool RTC_DS1307::begin(void)
{
    return 1;
}

bool RTC_DS1307::enable(void)
{
    uint8_t sec_reg = _getSecondsReg();

    // If nonzero, clock is halted, so issue command to start it up
    if (sec_reg & DS1307_CLOCK_HALT) {
        Wire.beginTransmission(DS1307_ADDRESS);
        Wire.SEND(DS1307_REG_SECONDS);
        Wire.SEND(sec_reg & DS1307_CH_MASK);
        Wire.endTransmission();
    }

    return true;
}

bool RTC_DS1307::isrunning(void)
{
    return (_getSecondsReg() & DS1307_CLOCK_HALT) == 0;
}

uint8_t RTC_DS1307::_getSecondsReg(void)
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.SEND(DS1307_REG_SECONDS);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_ADDRESS, 1);
    uint8_t ch = Wire.RECEIVE();
    return ch;
}

void RTC_DS1307::adjust(const DateTime& dt)
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.SEND(DS1307_REG_SECONDS);
    Wire.SEND(bin2bcd(dt.second()));
    Wire.SEND(bin2bcd(dt.minute()));
    Wire.SEND(bin2bcd(dt.hour()));
    Wire.SEND(bin2bcd(0)); // day of week set to zero b/c it doesn't matter
    Wire.SEND(bin2bcd(dt.day()));
    Wire.SEND(bin2bcd(dt.month()));
    Wire.SEND(bin2bcd(dt.year() - 2000));
    // XXX Is this just clobbering the control register for no reason?
    // Commenting out for now.
    //Wire.SEND(0);
    Wire.endTransmission();
}

DateTime RTC_DS1307::now()
{

    // Move read pointer to start of time data
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.SEND(DS1307_REG_SECONDS);
    Wire.endTransmission();

    // Grab next 7 bytes
    Wire.requestFrom(DS1307_ADDRESS, 7);
    uint8_t ss = bcd2bin(Wire.RECEIVE() & DS1307_CH_MASK);
    uint8_t mm = bcd2bin(Wire.RECEIVE());
    uint8_t hh = bcd2bin(Wire.RECEIVE());
    Wire.RECEIVE(); // ignore day of week; we dn't need it
    uint8_t d = bcd2bin(Wire.RECEIVE());
    uint8_t m = bcd2bin(Wire.RECEIVE());
    uint8_t y = bcd2bin(Wire.RECEIVE());

    return DateTime (y + 2000, m, d, hh, mm, ss);
}

// vim:ci:sw=4 sts=4 ft=cpp
