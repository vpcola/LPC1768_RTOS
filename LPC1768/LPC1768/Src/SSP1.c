/*
 * SSP1.c
 *
 *  Created on: May 29, 2016
 *      Author: vpcol
 */

#include "SSP1.h"
#include "FreeRTOS.h"
#include "PINDEFINES.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_exti.h"

#include "semphr.h"

static SemaphoreHandle_t ssp1Semaphore = NULL;

void SSP1_AcquireBus()
{
	if (ssp1Semaphore != NULL)
		xSemaphoreTake( ssp1Semaphore, portMAX_DELAY );
}

void SSP1_ReleaseBus()
{
	if (ssp1Semaphore != NULL)
        xSemaphoreGive( ssp1Semaphore );
}

void SSP1_SharedCSSet(uint32_t port, uint32_t pin, int val)
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

void Init_SSP1_Pins(void)
{

	// Configure SSPI12 P0.7 - SCK (mbed P7)
	//                  P0.8 - MISO (mbed P6)
	//                  P0.9 - MOSI (mbed P5)
	PINSEL_CFG_Type SSPI1Pin;
	SSPI1Pin.Portnum = SSP1_CLK_PORT;
	SSPI1Pin.Pinnum = SSP1_CLK_PIN;	// SCK1
	SSPI1Pin.Pinmode = PINSEL_PINMODE_PULLUP;
	SSPI1Pin.Funcnum = PINSEL_FUNC_2;	// Alternate function 2 - SSPI1
	SSPI1Pin.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&SSPI1Pin);

	SSPI1Pin.Portnum = SSP1_MISO_PORT;
	SSPI1Pin.Pinnum = SSP1_MISO_PIN; // MISO1
	PINSEL_ConfigPin(&SSPI1Pin);

	SSPI1Pin.Portnum = SSP1_MOSI_PORT;
	SSPI1Pin.Pinnum = SSP1_MOSI_PIN; // MOSI1
	PINSEL_ConfigPin(&SSPI1Pin);

	// Configure the Chip Selects under SSP1

	// SD/MMC
	PINSEL_CFG_Type CSPin;
	CSPin.Portnum = SDMMC_CS_PORT;
	CSPin.Pinnum = SDMMC_CS_PIN;
	CSPin.Pinmode = PINSEL_PINMODE_PULLUP;
	CSPin.Funcnum = PINSEL_FUNC_0;	// Alternate function 0 - GPIO
	CSPin.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&CSPin);

	CSPin.Portnum = LCD_CS_PORT;
	CSPin.Pinnum = LCD_CS_PIN;
	PINSEL_ConfigPin(&CSPin);

	// Set all CS pins as output
	GPIO_SetDir(SDMMC_CS_PORT, (1 << SDMMC_CS_PIN), GPIO_DIR_OUTPUT);
	GPIO_SetDir(LCD_CS_PORT, (1 << LCD_CS_PIN), GPIO_DIR_OUTPUT);

	// Set all CS default to 1
	SSP1_SharedCSSet(SDMMC_CS_PORT, SDMMC_CS_PIN, 1);
	SSP1_SharedCSSet(LCD_CS_PORT, LCD_CS_PIN, 1);
}

void SSP1_SetConfig(uint32_t speed, uint32_t polarity, uint32_t phase, uint32_t databit)
{
	SSP_CFG_Type SSP1Cfg;
	SSP_ConfigStructInit(&SSP1Cfg);
	SSP1Cfg.CPHA = phase;
	SSP1Cfg.Databit = databit;
	SSP1Cfg.ClockRate = speed; 
	SSP1Cfg.CPOL = polarity;
	SSP1Cfg.FrameFormat = SSP_FRAME_SPI; // MSB first
	SSP_Init(LPC_SSP1, &SSP1Cfg);
}

void Init_SSP1(void)
{
    // Turn on peripheral SPI0
    CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCSSP1, ENABLE);

	// Initialize SSP0 pins
	Init_SSP1_Pins();

	// Initialize SPI0 Clocks, Polarity, Phase, etc
	// CPOL = 0, CPHA = 1, Freq = 25 Mhz 
	// Databit = 8,
	SSP1_SetConfig(SSP1_CLOCK_RATE, SSP1_POLARITY, SSP1_PHASE, SSP_DATABIT_8);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);

    // Setup DMA channel 0 for enhanced SSP transfers
    CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPDMA, ENABLE);
    LPC_GPDMA->DMACConfig = 1;          // enable DMA controller
    LPC_GPDMA->DMACIntTCClear = 0x1;    // Reset the Interrupt status
    LPC_GPDMA->DMACIntErrClr = 0x1;
    LPC_GPDMACH0->DMACCLLI   = 0;

    ssp1Semaphore = xSemaphoreCreateMutex();
}


// A quick way to switch between 8-bit and 16 bit mode
void SSP1_16(int s)
{
    if(s)
        LPC_SSP1->CR0 |= 0x08;  // switch to 16 bit Mode
    else
        LPC_SSP1->CR0 &= ~(0x08);  // switch to 8  bit Mode
}

// Sets the SSP1 clock speed
void SSP1_Clock(unsigned int speed)
{
    SSP1_SetConfig(speed, SSP1_POLARITY, SSP1_PHASE, SSP_DATABIT_8);  
}

void SSP1_Write(unsigned short data)
{
    // wait untill fifo is empty
    while(((LPC_SSP1->SR) & 0x02) == 0);
    LPC_SSP1->DR = data;
    //while((LPC_SSP1->SR & (1<<2)) == 0)
    //LPC_SSP1->DR;
}

char SSP1_Read(void)
{
	char byteread;

	while((LPC_SSP1->SR & (1 << 1)) == 0);
	// Send Data (0) to flush
	LPC_SSP1->DR = 0x00;
	while((LPC_SSP1->SR & (1 << 2)) == 0); // Wait until RFF is full
	//SSP1_WaitBusy();
	byteread = (LPC_SSP1->DR) & 0xFF ;

	return byteread;
}

int SSP1_Transfer(unsigned short data)
{
	while((LPC_SSP1->SR & (1 << 1)) == 0);
	// Send Data (0) to flush
	LPC_SSP1->DR = data;
	//while((LPC_SSP1->SR & (1 << 3)) == 0);
	SSP1_WaitBusy();
	return (LPC_SSP1->DR);
}
void SSP1_WaitBusy()
{
    while ((LPC_SSP1->SR & 0x10) == 0x10); // Wait while busy
}

void SSP1_WriteDMA(int val, int numbytes)
{
    unsigned int dma_count;

    LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)&val;
    LPC_GPDMACH0->DMACCDestAddr = (uint32_t)&LPC_SSP1->DR; // we send to SSP1
    LPC_SSP1->DMACR = 0x2;

    // start DMA
    // Based on specs, DMA can transfer arnd 4k (4095) chunks
    // at a time, so we do it in batches
    do {
        if (numbytes > 4095) {
            dma_count = 4095;
            numbytes = numbytes - 4095;
        } else {
            dma_count = numbytes;
            numbytes = 0;
        }
        LPC_GPDMA->DMACIntTCClear = 0x1;
        LPC_GPDMA->DMACIntErrClr = 0x1;
        // Configure DMA to use 16bit transfers
        // no address increment, interrupt
        LPC_GPDMACH0->DMACCControl = dma_count | (1UL << 18) | (1UL << 21) | (1UL << 31) ;
        LPC_GPDMACH0->DMACCConfig  = DMA_CHANNEL_ENABLE | DMA_TRANSFER_TYPE_M2P | DMA_DEST_SSP1_TX;
        LPC_GPDMA->DMACSoftSReq = 0x1;   // DMA request
        // Wait until DMA is finished
        do {
        } while ((LPC_GPDMA->DMACRawIntTCStat & 0x01) == 0); // DMA is running
    } while (numbytes > 0);

}
