#include <stdint.h>
#include <stdbool.h>
#include <xc.h>
#include <stdio.h>

#include "serial.h"
#include "adxl345.h"

/*
 * Config settings
 */
// CONFIG1L
#pragma config CPUDIV = NOCLKDIV    // CPU System Clock Selection bits->No CPU System Clock divide
#pragma config USBDIV = OFF         // USB Clock Selection bit->USB clock comes directly from the OSC1/OSC2 oscillator block; no divide

// CONFIG1H
#pragma config FOSC = IRCCLKOUT     // Oscillator Selection bits->Internal RC oscillator, CLKOUT function on OSC2
#pragma config PLLEN = OFF          // 4 X PLL Enable bit->PLL is under software control
#pragma config PCLKEN = ON          // Primary Clock Enable bit->Primary clock enabled
#pragma config FCMEN = OFF          // Fail-Safe Clock Monitor Enable->Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF           // Internal/External Oscillator Switchover bit->Oscillator Switchover mode disabled

// CONFIG2L
#pragma config PWRTEN = OFF         // Power-up Timer Enable bit->PWRT disabled
#pragma config BOREN = SBORDIS      // Brown-out Reset Enable bits->Brown-out Reset enabled in hardware only (SBOREN is disabled)
#pragma config BORV = 19            // Brown-out Reset Voltage bits->VBOR set to 1.9 V nominal

// CONFIG2H
#pragma config WDTEN = OFF          // Watchdog Timer Enable bit->WDT is controlled by SWDTEN bit of the WDTCON register
#pragma config WDTPS = 32768        // Watchdog Timer Postscale Select bits->1:32768

// CONFIG3H
#pragma config HFOFST = ON          // HFINTOSC Fast Start-up bit->HFINTOSC starts clocking the CPU without waiting for the oscillator to stablize.
#pragma config MCLRE = ON           // MCLR Pin Enable bit->MCLR pin enabled; RA3 input pin disabled

// CONFIG4L
#pragma config STVREN = ON          // Stack Full/Underflow Reset Enable bit->Stack full/underflow will cause Reset
#pragma config LVP = OFF            // Single-Supply ICSP Enable bit->Single-Supply ICSP enabled
#pragma config BBSIZ = OFF          // Boot Block Size Select bit->1kW boot block size
#pragma config XINST = OFF          // Extended Instruction Set Enable bit->Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
#pragma config DEBUG = OFF          // Background Debugger Enable bit->Background debugger disabled, RA0 and RA1 configured as general purpose I/O pins

// CONFIG5L
#pragma config CP0 = OFF            // Code Protection bit->Block 0 not code-protected
#pragma config CP1 = OFF            // Code Protection bit->Block 1 not code-protected

// CONFIG5H
#pragma config CPB = OFF            // Boot Block Code Protection bit->Boot block not code-protected
#pragma config CPD = OFF            // Data EEPROM Code Protection bit->Data EEPROM not code-protected

// CONFIG6L
#pragma config WRT0 = OFF           // Table Write Protection bit->Block 0 not write-protected
#pragma config WRT1 = OFF           // Table Write Protection bit->Block 1 not write-protected

// CONFIG6H
#pragma config WRTC = OFF           // Configuration Register Write Protection bit->Configuration registers not write-protected
#pragma config WRTB = OFF           // Boot Block Write Protection bit->Boot block not write-protected
#pragma config WRTD = OFF           // Data EEPROM Write Protection bit->Data EEPROM not write-protected

// CONFIG7L
#pragma config EBTR0 = OFF          // Table Read Protection bit->Block 0 not protected from table reads executed in other blocks
#pragma config EBTR1 = OFF          // Table Read Protection bit->Block 1 not protected from table reads executed in other blocks

// CONFIG7H
#pragma config EBTRB = OFF          // Boot Block Table Read Protection bit->Boot block not protected from table reads executed in other blocks

/****** Global Vars *******/
uint32_t gSysTick = 0;
bool gSelfTestResult = false;


void main(void)
{
    // SCS0 INTOSC; IDLEN disabled; IRCF 16MHz_HF; 
    OSCCON = 0x70;
    // LFIOFS not stable; PRI_SD ON; HFIOFL not locked; 
    OSCCON2 = 0x04;
    // INTSRC INTRC; SPLLEN disabled; TUN 0; 
    OSCTUNE = 0x00;
    
    TRISC = 0x00;
    
    // no analog inputs in use
    ANSEL = 0x00;
    ANSELH = 0x00;

    // Enable timer 1
    T1CONbits.TMR1ON = 0x1;
    // System clock is not derived from Timer1 Osc
    T1CONbits.T1RUN = 0x0;
    // Timer1 Osc is disabled
    T1CONbits.T1OSCEN = 0x0;
    // Use Fosc/4 for the clock source
    T1CONbits.TMR1CS = 0x0;
    // prescaler of 1:4
    T1CONbits.T1CKPS = 0x2;
    // enable its interrupt
    TMR1IE = 0x01;

    // put the 485 driver in tx mode
    PORTC |= 1u << 0;
    
    // Global interrupt, peripheral interrupt enable
    INTCON = 0b11000000;
    //RCIE = 0x01;
    
    SerialInit();
    Adxl345Init();
    
    int16_t xyz[3];
    
    gSelfTestResult = Adxl345SelfTest();
    
    if (gSelfTestResult)
    {
        printf("Sensor passed self test\r\n");
    }
    else
    {
        printf("Sensor failed self test\r\n");
    }
    
    while (1)
    {
        if (gSysTick >= 10)
        {
           PORTC ^= 1u << 3;
           gSysTick = 0;
           Adxl345ReadData(xyz);
           printf("X:%d Y:%d Z:%d\r\n", xyz[0], xyz[1], xyz[2]);
        }
    }
}

void __interrupt() intVector(void) 
{
#if 0
    // Serial receive interrupt
    if (RCIF) {
        serbuff = RCREG;
        if (serMode != STARTUP) {
            FifoWrite(serbuff);
        }

        // clear any overrun errors
        if (OERR)
        {
            CREN=0;
            CREN=1;
        }
    }
#endif

    // Timer 1 interrupt every 50ms
    if (TMR1IF) 
    {
        // Clear interrupt flag & reload timer
        TMR1IF = 0x00;
        // we need to divide 1 Mhz by 50,000 to get a 20ms tick. So, reload 65535-50000=0x3CAF
        TMR1H = 0x3C;
        TMR1L = 0xAF;
        gSysTick++;
    }
}

/**
 End of File
*/