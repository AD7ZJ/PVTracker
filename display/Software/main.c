#include <htc.h>
#include <pic.h>
#include <string.h>
#include <stdio.h>

#define	_XTAL_FREQ 4000000
#include "adc.h"
#include "lcd.h"
#include "main.h"

__CONFIG(INTIO & WDTDIS & PWRTDIS & BORDIS & BORDIS & LVPDIS & DEBUGEN & DUNPROTECT & UNPROTECT);

/**
 * Initialize the EEPROM to approximate values at programming time
 *
 * First 7 bytes lay in as follows:
 * centerPos(hi),centerPos(lo),centerTime, nightDly(hi),nightDly(lo),updtTime(hi),updtTime(lo)
 * 
 * The next 4 contain the pump's total runtime
 */
__EEPROM_DATA(1, 194, 14, 112, 128, 7, 08, 0);
__EEPROM_DATA(0, 0, 0, 0, 0, 0, 0, 0);

static void (*selFuncPtr)(void);

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

    // set uptime at zero
    uptime = 0;
    LCD_BACKLIGHT = 0;
    buttonPress = NONE;
    backlightOffTick = 0;
}

void main(void) {
    OPTION = 0x00;

    //-- initialize the system --
    init();
    //-- initialise the ADC --
    initAdc();
    //-- initialise the LCD --
    lcd_init();

    _delay(100000);
    lcd_clear();

    // read saved parameters in from EEPROM
    ReadEEProm();

    // start on the home page
    page = MAIN_PAGE;
    trackCounter = updateTime;

    while (1) {
        if(systemTick > backlightOffTick)
            LCD_BACKLIGHT = 0;

        if(LCD_BACKLIGHT || (buttonPress == NONE)) {
            ButtonHandler();
        }
        else {
            LCD_BACKLIGHT = 1;
            backlightOffTick = systemTick + 10000;
            buttonPress = NONE;
        }

        /************ 1s tasks ***********/
        if (secFlag) {
            // update the track time
            if (--trackCounter == 0) {
                // update the track
                //-track();

                // reset the count
                trackCounter = updateTime;
            }

            // update the display
            menu(&page);
            secFlag = 0;
        }
    }
}

void ButtonHandler() {
    // any button press will keep the backlight on
    if (buttonPress != NONE)
        backlightOffTick = systemTick + 10000;

    switch (buttonPress) {
        case SCROLL:
            // clear the modify pointer
            varToModify = 0;
            buttonPress = NONE;

            if (++page >= END) {
                page = 0;
                // save settings to EEPROM
                WriteEEProm();
            }

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

        case SELECT:
            // run whatever the function pointer is pointing at
            //if(selFuncPtr)
            //    selFuncPtr();
            break;
    }
}

/**
 * Reads the various saved parameters in from EEPROM on boot
 */
void ReadEEProm() {
    centerPosition = eeprom_read(0) << 8;
    centerPosition |= eeprom_read(1);

    centerTime = eeprom_read(2);

    nightDelay = eeprom_read(3) << 8;
    nightDelay |= eeprom_read(4);

    updateTime = eeprom_read(5) << 8;
    updateTime |= eeprom_read(6);

    pumpTime = (uint32_t)eeprom_read(7) << 24;
    pumpTime |= (uint32_t)eeprom_read(8) << 16;
    pumpTime |= (uint32_t)eeprom_read(9) << 8;
    pumpTime |= (uint32_t)eeprom_read(10);
}

/**
 * Writes the various saved parameters into EEPROM
 */
void WriteEEProm() {
    eeprom_write(0, (centerPosition >> 8));
    eeprom_write(1, (centerPosition & 0x00FF));

    eeprom_write(2, centerTime);

    eeprom_write(3, (nightDelay >> 8));
    eeprom_write(4, (nightDelay & 0x00FF));

    eeprom_write(5, (updateTime >> 8));
    eeprom_write(6, (updateTime & 0x00FF));

    eeprom_write(7, (pumpTime >> 24));
    eeprom_write(8, (pumpTime >> 16));
    eeprom_write(9, (pumpTime >> 8));
    eeprom_write(10, (pumpTime & 0x000000FF));
}

/**
 * 'Increase' button handler
 * 
 * @param value Pointer to variable to be incremented
 */
void IncreaseValue(uint16_t * value) {
    if (value == 0)
        return;

    int i = 0;
    (*value)++;

    // update the display
    menu(&page);

    while (SW_INCREASE && i++ < 100)
        __delay_ms(10);
    i = 0;
    while (SW_INCREASE) {
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
    if (value == 0)
        return;

    int i = 0;
    (*value)--;

    // update the display
    menu(&page);

    while (SW_DECREASE && i++ < 100)
        __delay_ms(10);
    i = 0;
    while (SW_DECREASE) {
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
            lcd_puts((char *) display_out);
            break;

        case ADC_PAGE:
            // read the value on ADC0
            ad_in = readAdc(0);
            lcd_puts("Sensor Value");
            lcd_goto(40);
            sprintf(display_out, "%d", ad_in);
            lcd_puts((char *) display_out);
            break;

        case CENTER_POS:
            // setup the centerPosition variable to be modified
            varToModify = &centerPosition;
            lcd_puts("Center Position");
            lcd_goto(40);
            sprintf(display_out, "%d", centerPosition);
            lcd_puts((char *) display_out);
            break;

        case UPDATE_TIME:
            // setup the updateTime variable to be modified
            varToModify = &updateTime;
            lcd_puts("Update Time:");
            lcd_goto(40);
            sprintf(display_out, "%02d:%02d", updateTime / 60, updateTime % 60);
            lcd_puts((char *) display_out);
            break;

        case NIGHT_DELAY:
            // setup the nightDelay variable to be modified
            varToModify = &nightDelay;
            lcd_puts("Night Delay:");
            lcd_goto(40);
            sprintf(display_out, "%02d:%02d:%02d", (nightDelay / 3600), (nightDelay % 3600)/60, (nightDelay % 3600) % 60);
            lcd_puts((char *) display_out);
            break;

        case CENTER_TIME:
            // setup the centerTime variable to be modified
            varToModify = (uint16_t*)&centerTime;
            lcd_puts("Center Time:");
            lcd_goto(40);
            sprintf(display_out, "%02d:%02d", centerTime / 60, centerTime % 60);
            lcd_puts((char *) display_out);
            break;

        case MOVE_SAFE:
            // setup the centerTime variable to be modified
            lcd_puts("Press 'select'");
            lcd_goto(40);
            lcd_puts("to move safe.");
            break;

        case PUMP_TIME:
            lcd_puts("Total Pump time:");
            lcd_goto(40);
            sprintf(display_out, "%02d:%02d", pumpTime / 60, pumpTime % 60);
            lcd_puts((char *) display_out);
            break;

        case UPTIME:
            lcd_puts("System Uptime:");
            lcd_goto(40);
            sprintf(display_out, "%02d", uptime);
            lcd_puts((char *) display_out);
            break;
    }
}

void DebounceSwScroll() {
    static uint16_t state = 0;
    state = (state << 1) | !SW_SCROLL | 0xE000;
    if (state == 0xF000)
        buttonPress = SCROLL;
}

void DebounceSwIncrease() {
    static uint16_t state = 0;
    state = (state << 1) | !SW_INCREASE | 0xE000;
    if (state == 0xF000)
        buttonPress = INCREASE;
}

void DebounceSwDecrease() {
    static uint16_t state = 0;
    state = (state << 1) | !SW_DECREASE | 0xE000;
    if (state == 0xF000)
        buttonPress = DECREASE;
}

void DebounceSwSelect() {
    static uint16_t state = 0;
    state = (state << 1) | !SW_SELECT | 0xE000;
    if (state == 0xF000)
        buttonPress = SELECT;
}

interrupt isr(void) {
    if (TMR1IF) {
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
        if (msElapsed++ >= 1000) {
            secFlag = 1;
            uptime++;
            msElapsed = 0;
        }
        
        systemTick++;
    }
}