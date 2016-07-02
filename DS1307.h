// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#ifndef __RTC_DS1307_H__
#define __RTC_DS1307_H__

#include <RTClib.h>

//I2C Slave Address  0b1101000 per the datasheet
#define DS1307_ADDRESS 0x68

//
// DS1307 i2C register locations
//
#define DS1307_REG_SECONDS      0x00 // Top bit is clock halt (CH)
#define DS1307_REG_MINUTES      0x01
#define DS1307_REG_HOURS        0x02
#define DS1307_REG_DAYOFWEEK    0x03
#define DS1307_REG_DAYOFMONTH   0x04
#define DS1307_REG_MONTH        0x05
#define DS1307_REG_YEAR         0x06
#define DS1307_REG_CONTROL      0x07

// Mask for clock halt bit -- when set, the clock is halted, and when cleared,
// the oscillator is started
#define DS1307_CLOCK_HALT       0x80
#define DS1307_CH_MASK          (~(DS1307_CLOCK_HALT))

//
// Chip provides 56b of NVRAM
//
#define DS1307_REG_NVRAM_START  0x08
#define DS1307_REG_NVRAM_END    0x3f

//
// DS1307 Control Register values
// 

// Enable square wave output
#define DS1307_CTRL_SQW_ENABLE       0x10

// Frequence of square wave when enabled
#define DS1307_CTRL_SQW_FREQ_1HZ     0x00 // 0b00000000 // 1Hz
#define DS1307_CTRL_SQW_FREQ_4KHZ    0x01 // 0b00000001 // 4096Hz
#define DS1307_CTRL_SQW_FREQ_8KHZ    0x02 // 0b00000010 // 8192Hz
#define DS1307_CTRL_SQW_FREQ_32KHZ   0x03 // 0b00000011 // 8192Hz

// If the square wave output is disabled, you can set the pin to be either high
// or low based on bit 7 of the control register. Neat, I guess?
#define DS1307_CTRL_OUT_LOW          0x00
#define DS1307_CTRL_OUT_HIGH         0x80

// RTC based on the DS1307 chip connected via I2C and the Wire library
class RTC_DS1307 : public RTC
{
public:
    bool begin(void);
    void adjust(const DateTime& dt);
    bool enable(void);
    bool isrunning(void);
    DateTime now();
protected:
    uint8_t _getSecondsReg(void);
};

#endif // __RTC_DS1307_H__

// vim:ci:sw=4 sts=4 ft=cpp
