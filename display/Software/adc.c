/*
 * Read the ADC on a 16C71.
 */


#include	<pic.h>
#include	"adc.h"


/*
 * Read the adc on the specified channel
 *
 * @param channel Desired ADC channel to read
 * @return uint containing the result
 */
unsigned int readAdc(unsigned char channel) {
	// Right justify, channel select, ADON
	ADCON0 = ((channel & 0b00001111) << 2) | 0b10000001;
	// Aquisition time of 20uS
	_delay(200);
	ADCON0 |= 0b00000010;  // Start Conversion
	while(ADCON0 & 0b00000010)
		continue;	// wait for conversion complete

    return ADRESH << 8 | ADRESL;
}

void initAdc(void) {
	ADCON0=0;	// select Fosc/2
	//ADCON1=0;	// select left justify result. A/D port configuration 0
	ADFM = 1;
	ADON=1;		// turn on the A2D conversion module
}