/*
 * EXTI.c
 *
 *  Created on: May 29, 2016
 *      Author: vpcol
 */

#include "EXTI.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "GPIO.h"

#define WEAK __attribute__ ((weak))

// Called internally by the EXTI2 (Touch Interrupt)
WEAK extern void Touch_Update(void);
// Called internally by the EXTI3 (Pushbutton Key)
WEAK extern void Keyboard_Update(void) WEAK;

// Sets the interrupt on the GPIO pin
// level - 0 Falling
//       - 1 Rising
static void Set_InterruptPin(  uint32_t port, uint32_t pin, uint32_t level );
static uint32_t Check_InterruptPin ( uint32_t port, uint32_t pin, uint32_t level);
static void Clear_InterruptPin( uint32_t port, uint32_t pin );

// The EIN3 external interrupt handler,
// Nothing here for the moment
void EINT3_IRQHandler()
{
	// Determine the source of the interrupt
	if ( Check_InterruptPin(TOUCH_INT_PORT, TOUCH_INT_PIN, 0))
	{
		Touch_Update();
		Clear_InterruptPin(TOUCH_INT_PORT, TOUCH_INT_PIN);
	}

	if ( Check_InterruptPin(PUSHBUTTON_INT_PORT, PUSHBUTTON_INT_PIN, 0))
	{
		Keyboard_Update();
		Clear_InterruptPin(PUSHBUTTON_INT_PORT, PUSHBUTTON_INT_PIN);
	}

	// Clear the EXTI Flag so that further interrupts can
	// be generated.
	EXTI_ClearEXTIFlag(EXTI_EINT3);
}

void EXTI3_Enable()
{
	// Touch is triggered on the falling edge
	Set_InterruptPin( TOUCH_INT_PORT, TOUCH_INT_PIN, 0);
	Set_InterruptPin( PUSHBUTTON_INT_PORT, PUSHBUTTON_INT_PIN, 0);

	/* Install interrupt for EINT2 interrupt */
	NVIC_SetPriority(EINT3_IRQn, EXTI3_INT_PRIORITY);
	/* Enable the EINT3 interrupt*/
	NVIC_EnableIRQ(EINT3_IRQn);
}

void EXTI3_Disable()
{
	NVIC_DisableIRQ(EINT3_IRQn);
}

void Init_Exti_Pins(void)
{
    PINSEL_CFG_Type ExtIntPins;
    ExtIntPins.Portnum = PUSHBUTTON_INT_PORT;
    ExtIntPins.Pinnum = PUSHBUTTON_INT_PIN;
    ExtIntPins.Pinmode = PINSEL_PINMODE_PULLUP;
    ExtIntPins.OpenDrain = PINSEL_PINMODE_NORMAL;
    // External Interrupt pin - Alternate function 0x01
    ExtIntPins.Funcnum = PINSEL_FUNC_0; // GPIO
    PINSEL_ConfigPin(&ExtIntPins);

    // External Interrupt pin - Alternate function 0x01
    ExtIntPins.Portnum = TOUCH_INT_PORT;
    ExtIntPins.Funcnum = PINSEL_FUNC_0; // GPIO
    ExtIntPins.Pinnum = TOUCH_INT_PIN;
    PINSEL_ConfigPin(&ExtIntPins);

    // Configure PushButton INT as input
    GPIO_SetDir(PUSHBUTTON_INT_PORT, (1 << PUSHBUTTON_INT_PIN), GPIO_DIR_INPUT);
    // Configure TOUCH INT as input
    GPIO_SetDir(TOUCH_INT_PORT, (1 << TOUCH_INT_PIN), GPIO_DIR_INPUT);
}

void Init_EXTI(void)
{
	Init_Exti_Pins();

    // Initialize external interrupts
    EXTI_Init();
    // Enable GPIO external interrupts
    EXTI3_Enable();
}

int Touch_ReadIntPin()
{
	return ((LPC_GPIO2->FIOPIN & (1 << TOUCH_INT_PIN))>> TOUCH_INT_PIN);
}

int Keyboard_ReadIntPin()
{
	return ((LPC_GPIO2->FIOPIN & (1 << PUSHBUTTON_INT_PIN))>> PUSHBUTTON_INT_PIN);
}

static void Set_InterruptPin(  uint32_t port, uint32_t pin, uint32_t level )
{
	//  Dir is 0 for falling edge interrupt and 1 for rising edge interrupt
	if (port == 0)
	{
		if ( level == 0)
		{
			LPC_GPIOINT->IO0IntEnF |= ( 1 << pin );
		}
		else if ( level == 1)
		{
			LPC_GPIOINT->IO0IntEnR |= ( 1 << pin );
		}
	}
	else if ( port == 2)
	{
		if ( level == 0)
		{
			LPC_GPIOINT->IO2IntEnF |= ( 1 << pin );
		}
		else if ( level == 1)
		{
			LPC_GPIOINT->IO2IntEnR |= ( 1 << pin );
		}
	}
}

static uint32_t Check_InterruptPin( uint32_t port, uint32_t pin, uint32_t level)
{
	uint32_t intstatus;

	//  Dir is 0 for falling edge interrupt and 1 for rising edge interrupt
	if (port == 0)
	{
		if (level == 0)
		{
			intstatus =  LPC_GPIOINT->IO0IntStatF;
		}
		else if (level == 1)
		{
			intstatus = LPC_GPIOINT->IO0IntStatR;
		}
	}
	else if (port == 2)
	{
		if ( level == 0 )
		{
			intstatus = LPC_GPIOINT->IO2IntStatF;
		}
		else if ( level == 1 )
		{
			intstatus = LPC_GPIOINT->IO2IntStatR;
		}
	}

	if(intstatus & (1 << pin ))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	return 0;
}

static void Clear_InterruptPin( uint32_t port, uint32_t pin )
{
	if ( port == 0 )
	{
		LPC_GPIOINT->IO0IntClr = ( 1 << pin); // 0x7FFF8FFF;
	}
	else if ( port == 2 )
	{
		LPC_GPIOINT->IO2IntClr = ( 1 << pin); // 0x3fff;
	}
}



void Touch_Update(void)
{
	// Nothing here ... this function needs override
}

void Keyboard_Update(void)
{
	// Nothing here ... this function needs override
}
