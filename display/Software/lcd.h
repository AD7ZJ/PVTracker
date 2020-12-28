/*
 *	LCD interface header file
 *	See lcd.c for more info
 */

/* write a byte to the LCD in 4 bit mode */

void lcd_write(unsigned char);

/* Clear and home the LCD */

void lcd_clear(void);

/* write a string of characters to the LCD */

void lcd_puts(const char * s);

/* Go to the specified position */

void lcd_goto(unsigned char pos);

/* intialize the LCD - call before anything else */

void lcd_init(void);
