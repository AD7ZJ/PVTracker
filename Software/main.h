/*
 *	main header file
 */

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned long uint32_t;
typedef long int32_t;

// timing flags
unsigned int secFlag;
unsigned int msElapsed;

// display buffer
char display_out[40];

// discrete outputs
#define LCD_BACKLIGHT RB5
#define PUMP_RELAY    RC2
#define EAST_SOLENOID RC1
#define WEST_SOLENOID RC0

// discrete inputs
#define SW_SCROLL RA2
#define SW_INCREASE RA3
#define SW_DECREASE RA4
#define SW_SELECT RA5
#define SW_MANAUTO RD5
#define EASTLIMIT RD4
#define WESTLIMIT RC7

// enumerate the menu pages
typedef enum {
    MAIN_PAGE = 0,
    ADC_PAGE = 1,
    CENTER_POS = 2,
    END
} MENU_PAGE_T;

typedef enum {
    NONE = 0,
    SCROLL = 1,
    SELECT = 2,
    DECREASE = 3,
    INCREASE = 4
} BUTTON_PRESS_T;

MENU_PAGE_T page;
BUTTON_PRESS_T buttonPress;

// track time count in seconds
uint16_t trackCounter;

// center position in raw ADC values (0-1023)
uint16_t centerPosition;

void menu(MENU_PAGE_T * page);
void DebounceSwScroll();
void DebounceSwIncrease();
void DebounceSwDecrease();
void DebounceSwSelect();
void IncreaseValue(uint16_t * value);