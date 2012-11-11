#include <htc.h>
#include <pic.h>
#include <string.h>
#include <stdio.h>

#define	XTAL_FREQ	4MHZ
#include "adc.h"
#include "lcd.h"
#include "main.h"

__CONFIG (INTIO & WDTDIS & PWRTDIS & BORDIS & BORDIS & LVPDIS & DEBUGEN & DUNPROTECT & UNPROTECT);

void init(void) {
	// port directions: 1=input, 0=output
	TRISA = 0b00111111;
	TRISB = 0x00;
	TRISC = 0b10000000;
	TRISD = 0b00110000;

    // 4 MHz internal oscillator
	OSCCON = 0b01101000; 

    // Enable Timer1 and use Fosc/4
	T1OSCEN = 0; 
	TMR1CS = 0;
	TMR1ON = 1;

    // enable interrupts
	PEIE = 1;
	TMR1IE = 1;
	GIE = 1;
}




void main(void) {
	unsigned int ad_in;
	char display_out[15];
	OPTION=0x00;
 	GIE=0;
	init();
	initAdc();	// initialise the A2D module

	//-- initialise LCD --
	lcd_init();

	_delay(100000);
	lcd_clear();
	//sprintf(display_out, "Howdy %d", 1);
	//lcd_puts((char *)display_out);
 	//lcd_puts("Testing..");
 	//lcd_goto(40);


	while (1) {
		//PORTB = counter;
		ad_in = readAdc(0);		// sample the analog value on RA0
	
		if(secFlag) {
			lcd_clear();
			sprintf(display_out, "ADC0: %d", ad_in);
			lcd_puts((char *)display_out);
			secFlag = 0;
		}
	} 
}

interrupt isr(void) {
	if(TMR1IF) {
		flag = 1;
		TMR1IF = 0;
        
        // reload to generate a 1ms tick
        TMR1H = 0xFC;
        TMR1L = 0x17;
        
        if(msElapsed++ >= 1000) {
            secFlag = 1;
            msElapsed = 0;
        }
	}
}