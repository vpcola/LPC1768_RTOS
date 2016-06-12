/*
 * GPIO.c
 *
 *  Created on: 26 May, 2016
 *      Author: tkcov
 */

#include "GPIO.h"
#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_pinsel.h"

void Init_GPIO_Pins(void)
{
	// Configure GPIO Pins for LED control (RED Led and Blue Led)
	PINSEL_CFG_Type LedPin;
	LedPin.Portnum = LED1_PORT;
	LedPin.Pinnum = LED1_PIN;
	LedPin.Funcnum = PINSEL_FUNC_0; // Default GPIO
	LedPin.Pinmode = PINSEL_PINMODE_PULLUP;
	LedPin.OpenDrain = PINSEL_PINMODE_NORMAL;
	// Configure P0.22 (RED Led) for output
	PINSEL_ConfigPin(&LedPin);
	// Configure P3.26 (BLUE Led) for output
	LedPin.Portnum = LED2_PORT;
	LedPin.Pinnum = LED2_PIN;
	PINSEL_ConfigPin(&LedPin);
	LedPin.Portnum = LED3_PORT;
	LedPin.Pinnum = LED3_PIN;
	PINSEL_ConfigPin(&LedPin);
	LedPin.Portnum = LED4_PORT;
	LedPin.Pinnum = LED4_PIN;
	PINSEL_ConfigPin(&LedPin);

	// Set IO Direction
	GPIO_SetDir(LED1_PORT, (1 << LED1_PIN), GPIO_DIR_OUTPUT); // Red LED
	GPIO_SetDir(LED2_PORT, (1 << LED2_PIN), GPIO_DIR_OUTPUT); // Blue LED
	GPIO_SetDir(LED3_PORT, (1 << LED3_PIN), GPIO_DIR_OUTPUT); // Green LED
	GPIO_SetDir(LED4_PORT, (1 << LED4_PIN), GPIO_DIR_OUTPUT); // Green LED
}

void Init_GPIO(void)
{
    // Turn on peripheral GPIO0
    CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE);

    // Initialize the GPIO Pins
    Init_GPIO_Pins();

    // Set default value for the LEDs
    Led1(1);
    Led2(1);
    Led3(1);
    Led4(1);
}

void Led1(int value)
{
	if (value)
		GPIO_ClearValue(LED1_PORT, (1 << LED1_PIN));
	else
		GPIO_SetValue(LED1_PORT, (1 << LED1_PIN));
}

void Led2(int value)
{
	if (value)
		GPIO_ClearValue(LED2_PORT, (1 << LED2_PIN));
	else
		GPIO_SetValue(LED2_PORT, (1 << LED2_PIN));
}

void Led3(int value)
{
	if (value)
		GPIO_ClearValue(LED3_PORT, (1 << LED3_PIN));
	else
		GPIO_SetValue(LED3_PORT, (1 << LED3_PIN));
}

void Led4(int value)
{
	if (value)
		GPIO_ClearValue(LED4_PORT, (1 << LED4_PIN));
	else
		GPIO_SetValue(LED4_PORT, (1 << LED4_PIN));
}
