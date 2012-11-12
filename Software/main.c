#include <htc.h>
#include <pic.h>
#include <string.h>
#include <stdio.h>

#define	_XTAL_FREQ 4000000
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

    // Only using AN0 - make the rest digital I/O
    ANSELH = 0x00;
    ANSEL = 0x01;

    // enable interrupts
	PEIE = 1;
	TMR1IE = 1;
	GIE = 1;
}




void main(void) {
	char display_out[15];
	OPTION=0x00;

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
    
    LCD_BACKLIGHT = 1;
    // start on the home page
    page = MAIN_PAGE;
    //FIXME: needs to be read from EEPROM
    trackCounter = 1800;

	while (1) {
		switch (buttonPress) {
	    case SCROLL:
            // clear the modify pointer
            varToModify = 0;	
            buttonPress = NONE;

            if(++page >= END)
                page = 0;

            // update the display
            menu(&page);
            break;

        case INCREASE:
            IncreaseValue(varToModify);
            buttonPress = NONE;
            break;     

        case DECREASE:
            DecreaseValue(varToModify);
            buttonPress = NONE;
            break;
        }
            
        /************ 1s tasks ***********/
		if(secFlag) {
            // update the track time
            if(--trackCounter == 0) {
                // update the track
                trackCounter = 1800;
            }

            // update the display
            menu(&page);
			secFlag = 0;
		}
	} 
}

/**
 * 'Increase' button handler
 * 
 * @param value Pointer to variable to be incremented
 */
void IncreaseValue(uint16_t * value) {
    if(value == 0)
        return;

    int i = 0;
    (*value)++;

    // update the display
    menu(&page);

    while(SW_INCREASE && i++ < 100)
        __delay_ms(10);
    i = 0;
    while(SW_INCREASE) {
        __delay_ms(10);
        (*value)++;
        menu(&page);
    }
}

/**
 * 'Decrease' button handler
 * 
 * @param value Pointer to variable to be decremented
 */
void DecreaseValue(uint16_t * value) {
    if(value == 0)
        return;

    int i = 0;
    (*value)--;

    // update the display
    menu(&page);

    while(SW_INCREASE && i++ < 100)
        __delay_ms(10);
    i = 0;
    while(SW_INCREASE) {
        __delay_ms(10);
        (*value)--;
        menu(&page);
    }
}

/**
 * State machine called to update the display
 */
void menu(MENU_PAGE_T * page) {
    unsigned int ad_in;

    lcd_clear();
    switch (*page) {
    case MAIN_PAGE:
        lcd_puts("Auto Track Day");
        lcd_goto(40);
	    sprintf(display_out, "Moves in: %02d:%02d", (trackCounter / 60), (trackCounter % 60));
        lcd_puts((char *)display_out);
        break;
    case ADC_PAGE:
        // read the value on ADC0
        ad_in = readAdc(0);
        lcd_puts("Sensor Value");
        lcd_goto(40);
	    sprintf(display_out, "%d", ad_in);
        lcd_puts((char *)display_out);
        break;
    case CENTER_POS:
        // setup the centerPosition variable to be modified
        varToModify = &centerPosition;

        lcd_puts("Center Position");
        lcd_goto(40);
	    sprintf(display_out, "%d", centerPosition);
        lcd_puts((char *)display_out);
        break;
    }
}

void DebounceSwScroll() {
    static uint16_t state = 0;
    state = (state << 1) | !SW_SCROLL | 0xE000;
    if(state==0xF000)
        buttonPress = SCROLL;
}

void DebounceSwIncrease() {
    static uint16_t state = 0;
    state = (state << 1) | !SW_INCREASE | 0xE000;
    if(state==0xF000) 
        buttonPress = INCREASE;
}

void DebounceSwDecrease() {
    static uint16_t state = 0;
    state = (state << 1) | !SW_DECREASE | 0xE000;
    if(state==0xF000)
        buttonPress = DECREASE;
}

void DebounceSwSelect() {
    static uint16_t state = 0;
    state = (state << 1) | !SW_SELECT | 0xE000;
    if(state==0xF000)
        buttonPress = SELECT;
}

interrupt isr(void) {
	if(TMR1IF) {
		TMR1IF = 0;
        
        // reload to generate a 1ms tick
        TMR1H = 0xFC;
        TMR1L = 0x17;

        // debounce inputs
        DebounceSwScroll();
        DebounceSwIncrease();
        DebounceSwDecrease();
        DebounceSwSelect();        

        // update second counter
        if(msElapsed++ >= 1000) {
            secFlag = 1;
            msElapsed = 0;
        }
	}
}