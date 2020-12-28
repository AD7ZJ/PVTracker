#include <stdint.h>
#include <xc.h>
#include <stdio.h>

#include "serial.h"

/*
 * Config settings
 */

uint32_t sysTick = 0;

/*
                         Main application
 */
void main(void)
{
    // SCS0 INTOSC; IDLEN disabled; IRCF 16MHz_HF; 
    OSCCON = 0x70;
    // LFIOFS not stable; PRI_SD ON; HFIOFL not locked; 
    OSCCON2 = 0x04;
    // INTSRC INTRC; SPLLEN disabled; TUN 0; 
    OSCTUNE = 0x00;
    
    TRISC = 0x00;
    TRISB = 0x00;
    
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

    // put the 485 driver in tx
    PORTC |= 1u << 0;
    
    // GIE, PEIE Interrupts
    INTCON = 0b11000000;
    //RCIE = 0x01;
    
    SerialInit();
    
    SerialPutst("Booted\r\n");
    
    while (1)
    {
        if (sysTick >= 10)
        {
           PORTC ^= 1u << 3;
           sysTick = 0;
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
        sysTick++;
    }
}

/**
 End of File
*/