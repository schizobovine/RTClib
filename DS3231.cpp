// Based on Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!
//
// Modified and expanded by Red Byer 7/24/2013 to work with 3231 better
//     www.redstoyland.com      https://github.com/mizraith/RTClib
//
//  See .h file for the additions
//
// Modified by Sean Caulfield 2015-03-06 for ATTiny85 support (contributions
// under GPL2.0)
//

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
#include "DS3231.h"

////////////////////////////////////////////////////////////////////////////////
// RTC_DS3231 implementation

bool RTC_DS3231::begin(void)
{
    return true;
}

bool RTC_DS3231::enable(void)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(DS3231_REG_CONTROL);
    Wire.endTransmission();
    Wire.requestFrom(DS3231_ADDRESS, 1);
    uint8_t ctrl = Wire.read();
    
    // If oscillator is set off, flip it on
    if ((ctrl & DS3231_CTRL_EOSC) != 0) {
        Wire.beginTransmission(DS3231_ADDRESS);
        Wire.write(DS3231_REG_CONTROL);
        Wire.write(ctrl & (~DS3231_CTRL_EOSC));
        Wire.endTransmission();
    }

    return true;
}

bool RTC_DS3231::isrunning(void)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(DS3231_REG_STATUS_CTL);
    Wire.endTransmission();
    Wire.requestFrom(DS3231_ADDRESS, 1);
    uint8_t ctrl = Wire.read() & DS3231_CTRL_OSF;
    return (ctrl == 0);
}

/**
 * Set the datetime of the RTC
**/
void RTC_DS3231::adjust(const DateTime& dt)
{
    //set the address pointer and then start writing
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write( DS3231_REG_SECONDS );           // was just '0'
    Wire.write(bin2bcd(dt.second()));
    Wire.write(bin2bcd(dt.minute()));
    Wire.write(bin2bcd(dt.hour()));
    Wire.write(bin2bcd(0));
    Wire.write(bin2bcd(dt.day()));
    Wire.write(bin2bcd(dt.month()));
    Wire.write(bin2bcd(dt.year() - 2000));
    Wire.write(0);
    Wire.endTransmission();
}


/**
 * Get the datetime from the RTC
 **/
DateTime RTC_DS3231::now()
{
    //set the address pointer in preparation for read
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write( DS3231_REG_SECONDS );           // was just '0'
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 7);
    uint8_t ss = bcd2bin(Wire.read() & 0x7F);
    uint8_t mm = bcd2bin(Wire.read());
    uint8_t hh = bcd2bin(Wire.read());
    Wire.read();
    uint8_t d = bcd2bin(Wire.read());
    uint8_t m = bcd2bin(Wire.read());
    uint16_t y = bcd2bin(Wire.read()) + 2000;

    return DateTime (y, m, d, hh, mm, ss);
}


/**
 * Return temperature as a float in degrees C
 * Data is 10bits provided in 2 bytes (11h and and 2bits in 12h)
 * Resolution of 0.25C
 * e.g.  00011001 01     =    25.25C
 *       00011001 = 25
 *             01 = 0.25 * 1 = .25
 *
 *  This function has not been tested
 */
float RTC_DS3231::getTempAsFloat()
{
    //set the address pointer in preparation for read
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write( DS3231_REG_TEMP_MSB );           // was just '0'
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 2);
    int8_t sig   = Wire.read();                  //signed MSB
    uint8_t fract = Wire.read() & 0b11000000;    //rest should be zeroes anyway.
    
    fract = fract >> 6;                // now in 2 lsb's
    float temp = (float)fract * 0.25;  // total up,  .00, .25, .50
    if(sig < 0) {
        temp = sig - temp;             // calculate the fract correctly
    } else {
        temp = sig + temp;             // add    25 + .25 = 25.25
    }
    return temp;
}

/**
 * Return temperature as a 2 bytes in degrees C
 *    MSB is the significant  00011001 = 25
 *    LSB is the mantissa     00011001 = .25
 *  Display by writing them out to the display   MSB . LSB
 *
 * Data from the 3231 10bits provided in 2 bytes (11h and and 2bits in 12h)
 * Resolution of 0.25C
 * e.g.  00011001 01     =    25.25C
 *       00011001 = 25
 *             01 = 0.25 * 1 = .25
 *
 *  This function has not been tested
 */
int16_t RTC_DS3231::getTempAsWord()
{
    //set the address pointer in preparation for read
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write( DS3231_REG_TEMP_MSB );           // was just '0'
    Wire.endTransmission();

    Wire.requestFrom(DS3231_ADDRESS, 2);
    int8_t sig   = Wire.read();                  //signed MSB
    uint8_t fract = Wire.read() & 0b11000000;    //rest should be zeroes anyway.
    
    int16_t temp = sig;           //put into lower byte
    temp = temp << 8 ;            //shift to upper byte
    
    fract = fract >> 6;                // shift upper 2 bits to lowest 2 
    fract = fract * 25;                // multiply up
    
    temp = temp + fract;
    
    return temp;
}


/**
 *  Enable or disable the 32kHz pin.  When 1 it
 * will output 32768kHz.
 *
 */
void RTC_DS3231::enable32kHz(uint8_t enable)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write( DS3231_REG_STATUS_CTL );  
    Wire.endTransmission();

    // status register
    Wire.requestFrom(DS3231_ADDRESS, 1);

    uint8_t sreg = bcd2bin(Wire.read());    // do we need to wrap in bcd2bin?

    if (enable == true) {
        sreg |=  0b00001000; // Enable EN32KHZ bit
    } else {
        sreg &= ~0b00001000; // else set EN32KHZ bit to 0
    }

    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write( DS3231_REG_STATUS_CTL );
    Wire.write(sreg);
    Wire.endTransmission();
}


/**
 * Force the temp sensor to convert the temp
 * into digital code and execute the TCXO 
 * algorithm.  The DS3231 normally does
 * this every 64 seconds anyway.
 *
 * NOTE: This is a BLOCKING method.  You have been warned.
 *
 * This method has not been fully debugged.
 */
void RTC_DS3231::forceTempConv(uint8_t block)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write( DS3231_REG_CONTROL );
    Wire.endTransmission();

    // control register
    Wire.requestFrom(DS3231_ADDRESS, 1);

    uint8_t creg = Wire.read();  // do we need the bcd2bin

    creg |= 0b00100000; // Write CONV bit

    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(DS3231_REG_CONTROL);
    Wire.write(creg);
    Wire.endTransmission();

    do
    {
        // Block until CONV is 0
        Wire.beginTransmission(DS3231_ADDRESS);
        Wire.write(DS3231_REG_CONTROL);
        Wire.endTransmission();
        Wire.requestFrom(DS3231_ADDRESS, 1);
    } while ((block && (Wire.read() & 0b00100000) != 0));
}  


/**
 * Enable or disable the output to the SQW pin.
 *  Send 1 or true to enable.
 *  This function does _not_ enable battery backed output
 * which helps conserve battery life.
 *
 */
void RTC_DS3231::SQWEnable(uint8_t enable)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(DS3231_REG_CONTROL);
    Wire.endTransmission();

    // control register
    Wire.requestFrom(DS3231_ADDRESS, 1);

    uint8_t creg = Wire.read();     //do we need the bcd2bin

    
    if (enable == true) {
        creg &= ~0b00000100; // Clear INTCN bit to output the square wave
    } else {
        creg |= 0b00000100;      // Set INTCN to 1 -- disables SQW
    }

    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(DS3231_REG_CONTROL);
    Wire.write(creg);
    Wire.endTransmission();
}


/**
 * Enable or disable the output to the SQW pin.
 *  Send 1 or true to enable.
 * This method enables BOTH the square wave
 * and the battery backed output.
 *
 */
void RTC_DS3231::BBSQWEnable(uint8_t enable)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(DS3231_REG_CONTROL);
    Wire.endTransmission();

    // control register
    Wire.requestFrom(DS3231_ADDRESS, 1);

    uint8_t creg = Wire.read();     //do we need the bcd2bin


    if (enable == true) {
        creg |=  0b01000000; // Enable BBSQW if required so SQW continues to output (battery life reduction).
        creg &= ~0b00000100; // Clear INTCN bit to output the square wave
    } else {
        creg &= ~0b01000000;    // Set BBSQW 0 to disable battery back
        //creg |=  0b00000100;      // Set INTCN to 1 -- disables SQW
    }

    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(DS3231_REG_CONTROL);
    Wire.write(creg);
    Wire.endTransmission();
}

/**
 *  Set the output frequence of the squarewave SQW pin
 *
 *  HINT:  Use the defined frequencies in the .h file
 *  
 */
void RTC_DS3231::SQWFrequency(uint8_t freq)
{
    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(DS3231_REG_CONTROL);
    Wire.endTransmission();

    // control register
    Wire.requestFrom(DS3231_ADDRESS, 1);

    uint8_t creg = Wire.read();

    creg &= ~0b00011000; // Set to 0
    creg |= freq; // Set freq bits

    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write(0x0E);
    Wire.write(creg);
    Wire.endTransmission();
}

void RTC_DS3231::getControlRegisterData(char &datastr) {

    Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write( DS3231_REG_CONTROL );
    Wire.endTransmission();

    // control registers
    Wire.requestFrom(DS3231_ADDRESS, 2);
    uint8_t creg = Wire.read();  // do we need the bcd2bin
    uint8_t sreg = Wire.read(); 
    
    char cregstr[] = "00000000";
    char sregstr[] = "00000000";
    getBinaryString(creg, cregstr);
    getBinaryString(sreg, sregstr);
    
    strcpy(&datastr, "\n----- DS3231 Information -----\ncreg: ");
    strcat(&datastr, cregstr);
    strcat(&datastr, "\nsreg: ");
    strcat(&datastr, sregstr);
    strcat(&datastr, "\n------------------------------\n");     

    return;
}

void RTC_DS3231::clearControlRegisters() {

  //
  // Write to control register to clear extra crap to save power.
  //
  // a) enable oscillator (/EOSC), which is set to 0 to enable it
  // b) battery backed square wave is off (BBSQW, bit 6)
  // c) don't ask for temp correct to run right now (it will keep going in the
  // background which is good enough)
  // d) set square wave output to 1Hz (RS1 & RS2, bits 3-4)
  // e) disable interrupt (INTCN, bit 2)
  // f) disable alarms (A1IE & A2IE, bits 0-1)
  //
  // ...which all boils down to setting the damn thing to 0. Woo!
  //
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(DS3231_REG_CONTROL);
  Wire.write(0x00);
  Wire.endTransmission();

  //
  // Clear out last other control thingy, the 32kHz output (EN32K, bit 3)
  //
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(DS3231_REG_STATUS_CTL);
  Wire.write(0x00);
  Wire.endTransmission();

}


/********************************************************
* UTILITY and WORKER METHODS
*********************************************************/

/**
 * Takes one byte and loads up bytestr[8] with the value
 *  eg  "8" loads bytestr with "00000111"
 */
void RTC_DS3231::getBinaryString(uint8_t byteval, char bytestr[]) 
{
    uint8_t bitv;
    int i = 0;
    
    for (i = 0; i < 8; i++) {                    
           bitv = (byteval >> i) & 1;
           if (bitv == 0) {
               bytestr[7-i] = '0';
           } else {
               bytestr[7-i] = '1';
           }
    }
}

// vim:ci:sw=4 sts=4 ft=cpp
