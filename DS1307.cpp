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

uint8_t RTC_DS1307::begin(void)
{
    return 1;
}

uint8_t RTC_DS1307::isrunning(void)
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.SEND(0);
    Wire.endTransmission();

    Wire.requestFrom(DS1307_ADDRESS, 1);
    uint8_t ss = Wire.RECEIVE();
    return !(ss>>7);
}

void RTC_DS1307::adjust(const DateTime& dt)
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.SEND(0);
    Wire.SEND(bin2bcd(dt.second()));
    Wire.SEND(bin2bcd(dt.minute()));
    Wire.SEND(bin2bcd(dt.hour()));
    Wire.SEND(bin2bcd(0));
    Wire.SEND(bin2bcd(dt.day()));
    Wire.SEND(bin2bcd(dt.month()));
    Wire.SEND(bin2bcd(dt.year() - 2000));
    Wire.SEND(0);
    Wire.endTransmission();
}

DateTime RTC_DS1307::now()
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.SEND(0);
    Wire.endTransmission();

    Wire.requestFrom(DS1307_ADDRESS, 7);
    uint8_t ss = bcd2bin(Wire.RECEIVE() & 0x7F);
    uint8_t mm = bcd2bin(Wire.RECEIVE());
    uint8_t hh = bcd2bin(Wire.RECEIVE());
    Wire.RECEIVE();
    uint8_t d = bcd2bin(Wire.RECEIVE());
    uint8_t m = bcd2bin(Wire.RECEIVE());
    uint16_t y = bcd2bin(Wire.RECEIVE()) + 2000;

    return DateTime (y, m, d, hh, mm, ss);
}

// vim:ci:sw=4 sts=4 ft=cpp
