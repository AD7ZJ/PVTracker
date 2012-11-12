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
uint8_t secFlag;
uint16_t msElapsed;
uint32_t systemTick;
uint32_t backlightOffTick;

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
    UPDATE_TIME = 3,
    NIGHT_DELAY = 4,
    CENTER_TIME = 5,
    MOVE_SAFE = 6,
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

// pointer to variable that'll be incremented or decremented
uint16_t * varToModify;

// track time count in seconds
uint16_t trackCounter;

// center position in raw ADC values (0-1023)
uint16_t centerPosition;

// time in s to run the pump to return the array to center
uint8_t centerTime;

// time in s to stay in the center before returning east
uint16_t nightDelay;

// time in s to wait between updating the track
uint16_t updateTime;

// total time in s the pump has been running
uint32_t pumpTime;

void menu(MENU_PAGE_T * page);
void ReadEEProm();
void WriteEEProm();
void ButtonHandler();
void DebounceSwScroll();
void DebounceSwIncrease();
void DebounceSwDecrease();
void DebounceSwSelect();
void IncreaseValue(uint16_t * value);
void DecreaseValue(uint16_t * value);
