/*
 * SSP0.c
 *
 *  Created on: May 29, 2016
 *      Author: vpcol
 */

#include "SSP0.h"
#include "FreeRTOS.h"
#include "PINDEFINES.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_exti.h"

#include "semphr.h"

static SemaphoreHandle_t ssp0Semaphore = NULL;

void SSP0_AcquireBus()
{
	if (ssp0Semaphore != NULL)
		xSemaphoreTake( ssp0Semaphore, portMAX_DELAY );
}

void SSP0_ReleaseBus()
{
	if (ssp0Semaphore != NULL)
        xSemaphoreGive( ssp0Semaphore );
}

void SSP0_SharedCSSet(uint32_t port, uint32_t pin, int val)
{
    if (val == 0)
    {
       GPIO_ClearValue(port, (1 << pin));
    }
    else
    {
       GPIO_SetValue(port, (1 << pin));
    }
}

void Init_SSP0_Pins(void)
{

	// Configure SSPI12 P0.15 - SCK (mbed P13)
	//                  P0.17 - MISO (mbed P12)
	//                  P0.18 - MOSI (mbed P11)
	PINSEL_CFG_Type SSPI1Pin;
	SSPI1Pin.Portnum = SSP0_CLK_PORT;
	SSPI1Pin.Pinnum = SSP0_CLK_PIN;	// SCK1
	SSPI1Pin.Pinmode = PINSEL_PINMODE_PULLUP;
	SSPI1Pin.Funcnum = PINSEL_FUNC_2;	// Alternate function 2 - SSP0
	SSPI1Pin.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&SSPI1Pin);

	SSPI1Pin.Portnum = SSP0_MISO_PORT;
	SSPI1Pin.Pinnum = SSP0_MISO_PIN; // MISO1
	PINSEL_ConfigPin(&SSPI1Pin);

	SSPI1Pin.Portnum = SSP0_MOSI_PORT;
	SSPI1Pin.Pinnum = SSP0_MOSI_PIN; // MOSI1
	PINSEL_ConfigPin(&SSPI1Pin);

	// Configure the Chip Selects under SSP0

	// TOUCH XPT2046
	PINSEL_CFG_Type CSPin;
	CSPin.Portnum = TOUCH_CS_PORT;
	CSPin.Pinnum = TOUCH_CS_PIN;
	CSPin.Pinmode = PINSEL_PINMODE_PULLUP;
	CSPin.Funcnum = PINSEL_FUNC_0;	// Alternate function 0 - GPIO
	CSPin.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&CSPin);

	// Set all CS pins as output
	GPIO_SetDir(TOUCH_CS_PORT, (1 << TOUCH_CS_PIN), GPIO_DIR_OUTPUT);

	// Set all CS default to 1
	SSP0_SharedCSSet(TOUCH_CS_PORT, TOUCH_CS_PIN, 1);
}

void SSP0_SetConfig(uint32_t speed, uint32_t polarity, uint32_t phase, uint32_t databit)
{
	SSP_CFG_Type SSP0Cfg;
	SSP_ConfigStructInit(&SSP0Cfg);
	SSP0Cfg.CPHA = phase;
	SSP0Cfg.Databit = databit;
	SSP0Cfg.ClockRate = speed;
	SSP0Cfg.CPOL = polarity;
	SSP0Cfg.FrameFormat = SSP_FRAME_SPI; // MSB first
	SSP_Init(LPC_SSP0, &SSP0Cfg);
}

void Init_SSP0(void)
{
    // Turn on peripheral SPI0
    CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCSSP0, ENABLE);

	// Initialize SSP0 pins
	Init_SSP0_Pins();

	// Initialize SPI0 Clocks, Polarity, Phase, etc
	// CPOL = 0, CPHA = 1, Freq = 10MHz
	// Databit = 8,
	SSP0_SetConfig(SSP0_CLOCK_RATE, SSP0_POLARITY, SSP0_PHASE, SSP_DATABIT_8);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP0, ENABLE);

    // Setup DMA channel 0 for enhanced SSP transfers
    CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPDMA, ENABLE);
    LPC_GPDMA->DMACConfig = 1;          // enable DMA controller
    LPC_GPDMA->DMACIntTCClear = 0x1;    // Reset the Interrupt status
    LPC_GPDMA->DMACIntErrClr = 0x1;
    LPC_GPDMACH0->DMACCLLI   = 0;

    // Create a semaphore to guard the SSP0 peripheral
    /* As we are using the semaphore for mutual exclusion we
     * create a mutex semaphore rather than a binary semaphore.
     **/
    ssp0Semaphore = xSemaphoreCreateMutex();
}


// A quick way to switch between 8-bit and 16 bit mode
void SSP0_16(int s)
{
    if(s)
        LPC_SSP0->CR0 |= 0x08;  // switch to 16 bit Mode
    else
        LPC_SSP0->CR0 &= ~(0x08);  // switch to 8  bit Mode
}


void SSP0_Write(unsigned short data)
{
    // wait untill fifo is empty
    while(((LPC_SSP0->SR) & 0x02) == 0);
    LPC_SSP0->DR = data;
    //while((LPC_SSP0->SR & (1<<2)) == 0)
    //LPC_SSP0->DR;
}

char SSP0_Read(void)
{
	char byteread;

	while((LPC_SSP0->SR & (1 << 1)) == 0);
	// Send Data (0) to flush
	LPC_SSP0->DR = 0x00;
	while((LPC_SSP0->SR & (1 << 2)) == 0); // Wait until RFF is full
	//SSP0_WaitBusy();
	byteread = (LPC_SSP0->DR) & 0xFF ;

	return byteread;
}
int SSP0_Transfer(unsigned short data)
{
	while((LPC_SSP0->SR & (1 << 1)) == 0);
	// Send Data (0) to flush
	LPC_SSP0->DR = data;
	//while((LPC_SSP0->SR & (1 << 3)) == 0);
	SSP0_WaitBusy();
	return (LPC_SSP0->DR);
}
void SSP0_WaitBusy()
{
    while ((LPC_SSP0->SR & 0x10) == 0x10); // Wait while busy
}
