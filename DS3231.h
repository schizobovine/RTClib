// Original Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!
//
// Modified and expanded by Red Byer 7/24/2013 to work with 3231 better
//     www.redstoyland.com      Find the code under "mizraith" on github
//
//   Now includes code to read the temperature back form the 3231
//  Also allows for easy access to the two status registers.
//


#ifndef __RTC_DS3231_H__
#define __RTC_DS3231_H__

#include <RTClib.h>

//I2C Slave Address  0b1101000 per the datasheet
#define DS3231_ADDRESS 0x68  

//DS3231 REGISTER DEFINITIONS
#define DS3231_REG_SECONDS    0x00
#define DS3231_REG_MINUTES    0x01
#define DS3231_REG_HOURS      0x02
#define DS3231_REG_DAYOFWEEK  0x03
#define DS3231_REG_DAYOFMONTH 0x04
#define DS3231_REG_MONTH      0x05
#define DS3231_REG_YEAR       0x06

#define DS3231_REG_A1SECONDS  0x07
#define DS3231_REG_A1MINUTES  0x08
#define DS3231_REG_A1HOURS    0x09
#define DS3231_REG_A1DAYDATE  0x0A

#define DS3231_REG_A2MINUTES  0x0B
#define DS3231_REG_A2HOURS    0x0C
#define DS3231_REG_A2DAYDATE  0x0D

#define DS3231_REG_CONTROL    0x0E
#define DS3231_REG_STATUS_CTL 0x0F
#define DS3231_REG_AGING      0x10
#define DS3231_REG_TEMP_MSB   0x11
#define DS3231_REG_TEMP_LSB   0x12

//FREQUENCY SETTINGS FOR THE SQW PIN
#define DS3231_SQW_FREQ_1    0b00000000 // 1Hz
#define DS3231_SQW_FREQ_1024 0b00001000 // 1024Hz
#define DS3231_SQW_FREQ_4096 0b00010000 // 4096Hz
#define DS3231_SQW_FREQ_8192 0b00011000 // 8192Hz

#define DS3231_CTRL_EOSC      0x80 // Oscillator enable
#define DS3231_CTRL_OSF       0x80 // Oscillator stopped flag

// RTC based on the DS3231 chip connected via I2C and the Wire library
class RTC_DS3231 : public RTC
{

public:
    bool begin(void);
    void adjust(const DateTime& dt);
    bool enable(void);
    bool isrunning(void);
    DateTime now();

    float getTempAsFloat();
    int16_t getTempAsWord();
    void enable32kHz(uint8_t enable);
    void forceTempConv(uint8_t block);
    void SQWEnable(uint8_t enable);
    void BBSQWEnable(uint8_t enable);
    void SQWFrequency(uint8_t freq);
    void getControlRegisterData(char &datastr);
    void clearControlRegisters();
    
private:
    void getBinaryString(uint8_t byteval, char bytestr[]);

};

#endif // __RTC_DS3231_H__

// vim:ci:sw=4 sts=4 ft=cpp
