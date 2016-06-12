/*
 * XPT2046.c
 *
 *  Created on: 30 May, 2016
 *      Author: tkcov
 */

#include <stdio.h>

#include "XPT2046.h"
#include "SSP0.h"
#include "EXTI.h"
#include "PINDEFINES.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_ssp.h"


// Flag to indicate touch panel hit
// Interrupt sets this flag, user must reset
// after acknowledgement
__IO uint32_t HasTouched;

__IO uint16_t xraw, yraw, zraw;

void Touch_Update(void)
{
	HasTouched = 1;
}

void TOUCH_CS(int enable)
{
   	SSP0_SharedCSSet(TOUCH_CS_PORT, TOUCH_CS_PIN, enable);
}

uint16_t XPT2046_Read()
{
	unsigned int Xresult = 0;
	unsigned int Yresult = 0;
	char rdata[3];

	// Write control byte to touch
	// controller if TX Fifo is not
	// full and not busy
	TOUCH_CS(0);
	SSP0_Write(0x80 | 0x50 );
	//Xresult = SSP0_Read() << 8;
	//Xresult |= SSP0_Read();
	//Xresult >>= 3;
	rdata[0] = SSP0_Read();
	rdata[1] = SSP0_Read();
	Xresult =  (( rdata[0] << 8 ) | rdata[1] ) >> 3;
	SSP0_WaitBusy();
	TOUCH_CS(1);

	TOUCH_CS(0);
	rdata[0] = SSP0_Transfer( 0x80 | 0x10 );
	rdata[1] = SSP0_Read();
	rdata[2] = SSP0_Read();
	Yresult = (( rdata[1] << 8) | rdata[2] ) >> 3;
	SSP0_WaitBusy();
	TOUCH_CS(1);

	xraw = Xresult;
	yraw = Yresult;
	printf("X = %d, Y = %d\n", Xresult, Yresult);
	return 0;
}

void Init_XPT2046(void)
{
	// Initialize the value of the flag
	HasTouched = 0;

	// Get sample reading, leaving PENIRQ up
	TOUCH_CS(0);
    while(((LPC_SSP0->SR) & 0x02) == 0);
    LPC_SSP0->DR = 0x80;
    TOUCH_CS(1);

}

// This function waits until the HasTouched flag
// is set and updates the touch coordinates.
int  XPT2046_Wait(int * x, int * y, int timeout)
{
	int temp = timeout;
	while((HasTouched == 0) && (temp > 0)) temp--;

	if (HasTouched)
	{
		XPT2046_Read(); // get coordinates
		*x = xraw;
		*y = yraw;
		// Reset the flag
		HasTouched = 0;
		return 1;
	}
	// Timeout
	return 0;
}
