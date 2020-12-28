#include <xc.h>
#include <stdbool.h>
#include "adxl345.h"

#define REG_POWER_CTL   0x2D
#define REG_X_DATA      0x32

static void Adxl345SS(bool level);
void Adxl345WriteReg(uint8_t reg, uint8_t data);

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
    
    // start measurments
    Adxl345WriteReg(REG_POWER_CTL, 0x08);
}

uint8_t Adxl345SpiXfr(uint8_t dat) 
{
    SSPBUF = dat;
    while (!SSPSTATbits.BF);
    return (uint8_t) SSPBUF;
}

void Adxl345WriteReg(uint8_t reg, uint8_t data)
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

uint8_t Adxl345ReadReg(uint8_t reg)
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