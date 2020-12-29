/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111 USA    *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *               (c) Copyright, 2020, AD7ZJ                                *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     adxl345.c                                                 *
 *                                                                         *
 ***************************************************************************/

#include <xc.h>
#include "adxl345.h"

#define REG_POWER_CTL   0x2D
#define REG_DATA_FORMAT 0x31
#define REG_X_DATA      0x32
#define _XTAL_FREQ      16000000

static void Adxl345SS(bool level);
static void Adxl345WriteReg(uint8_t reg, uint8_t data);
static uint8_t Adxl345ReadReg(uint8_t reg);
static uint8_t Adxl345SpiXfr(uint8_t dat);

/**
 * Initialize the ADXL345 driver. Initializes the SPI hardware and required pins. 
 */
void Adxl345Init()
{
    // disable the SPI driver
    SSPCON1bits.SSPEN = 0;
    
    // setup SDO and SCL as outputs
    TRISC &= ~(1u << 7); 
    TRISB &= ~(1u << 6); 
    // setup SDI as an input
    TRISB |= 1u << 4;
    
    // drive SS high
    Adxl345SS(true);
    
    // Master, use Fosc/16 for the clock
    SSPCON1bits.SSPM = 1;

    // setup SPI mode CPOL = 1, CPHA = 1
    SSPCON1bits.CKP = 1;
    SSPSTATbits.CKE = 0;
    SSPSTATbits.SMP = 0;
    
    // enable the SPI driver
    SSPCON1bits.SSPEN = 1;
    
    // Ensure default data format
    Adxl345WriteReg(REG_DATA_FORMAT, 0x00);
    // start measurements
    Adxl345WriteReg(REG_POWER_CTL, 0x08);
}

static uint8_t Adxl345SpiXfr(uint8_t dat) 
{
    SSPBUF = dat;
    while (!SSPSTATbits.BF);
    return (uint8_t) SSPBUF;
}

static void Adxl345SS(bool level)
{
    // RC6 is connected to the CS pin on the adxl345
    if (level)
    {
        PORTC |= 1u << 6;
    }
    else
    {
        PORTC &= ~(1u << 6);
    }
}

static void Adxl345WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t addrByte = 0;
    addrByte = reg & 0x3F;

    // drive SS low
    Adxl345SS(false);
    
    Adxl345SpiXfr(addrByte);
    Adxl345SpiXfr(data);
    
    // drive SS high
    Adxl345SS(true);
}

static uint8_t Adxl345ReadReg(uint8_t reg)
{
    uint8_t addrByte = 0;
    uint8_t dataByte = 0;
    addrByte = reg & 0x3F;
    addrByte |= 0x80;

    // drive SS low
    Adxl345SS(false);
    
    Adxl345SpiXfr(addrByte);
    dataByte = Adxl345SpiXfr(0x00);
    
    // drive SS high
    Adxl345SS(true);
    return dataByte;
}

/**
 * Read the X/Y/Z accel data from the part. 
 *
 * @param xyz Pointer to an array of 3 16 bit values. Result will be stored in order of X-Y-Z. 
 */
void Adxl345ReadData(int16_t* xyz)
{
    uint8_t addrByte = 0;
    uint8_t data[6];
    addrByte = REG_X_DATA;
    // multi-byte read
    addrByte |= 0xC0;
    
    // drive SS low
    Adxl345SS(false);
    
    Adxl345SpiXfr(addrByte);
    for (int i = 0; i < 6; i++)
    {
        data[i] = Adxl345SpiXfr(0x00);
    }
    
    // X data
    xyz[0] = data[1] << 8 | data[0];
    // Y data
    xyz[1] = data[3] << 8 | data[2];
    // Z data
    xyz[2] = data[5] << 8 | data[4];
    
    // drive SS high
    Adxl345SS(true);
}

/**
 * Execute the self-test. Blocks for around 150ms. 
 * 
 * @return True if test passed, false otherwise. 
 */
bool Adxl345SelfTest(void)
{
    bool result = true;
    int16_t initialReading[3];
    int16_t selfTestReading[3];
    // make sure any prior test mode effects are gone
    __delay_ms(50);
    // take initial reading
    Adxl345ReadData(initialReading);
    // enable self test
    Adxl345WriteReg(REG_DATA_FORMAT, 0x80);
    __delay_ms(50);
    // take another reading
    Adxl345ReadData(selfTestReading);
    // turn off self test
    Adxl345WriteReg(REG_DATA_FORMAT, 0x00);
    __delay_ms(50);
    
    // X should have moved approx 1g positive, 512 represents approx 2g on this range 
    if (selfTestReading[0] - initialReading[0] < 100)
        result = false;
    // Y should have moved approx 1g negative
    if (selfTestReading[1] - initialReading[1] > -100)
        result = false;
    // Z should have moved approx 1g positive
    if (selfTestReading[2] - initialReading[2] < 100)
        result = false;
    
    return result;
}

