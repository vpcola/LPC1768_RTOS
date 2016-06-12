/*
 * UART.c
 *
 *  Created on: 26 May, 2016
 *      Author: tkcov
 */
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "UART.h"
#include "PINDEFINES.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_clkpwr.h"

#include <string.h>
#include <ctype.h>
#include "semphr.h"

static SemaphoreHandle_t uart3Semaphore = NULL;

void UART3_AcquireBus()
{
    if( uart3Semaphore != NULL )
    	xSemaphoreTake( uart3Semaphore, portMAX_DELAY );
}

void UART3_ReleaseBus()
{
    if( uart3Semaphore != NULL )
        xSemaphoreGive( uart3Semaphore );
}

void Init_UART3_Pins(void)
{
	// Configure UART Pins TXD3 and RXD3
	// Configure P0.0 (TXD3)
	PINSEL_CFG_Type UartPin;
	UartPin.Portnum = UART3_TX_PORT;
	UartPin.Pinnum = UART3_TX_PIN; 		// TX
	UartPin.Funcnum = PINSEL_FUNC_2; 	// TXD3
	UartPin.Pinmode = PINSEL_PINMODE_TRISTATE; // Neither pull-up or pull-down
	UartPin.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&UartPin);

	// Configure P0.1 (RXD3)
	UartPin.Portnum = UART3_RX_PORT;
	UartPin.Pinnum = UART3_RX_PIN;		// RX
	UartPin.Funcnum = PINSEL_FUNC_2; 	// RXD3
	PINSEL_ConfigPin(&UartPin);
}

void Init_UART3(int baudrate)
{
	UART_CFG_Type UartCfg;
	UART_FIFO_CFG_Type FifoConfig;

    // Turn on peripheral UART3
    CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCUART3, ENABLE);

    // Initialize UART3 Pins
	Init_UART3_Pins();

	// Initialize Baudrate
	UART_ConfigStructInit(&UartCfg);
	// Only set the baudrate for now,
	// leave others untouched (8 bits, parity none, 1 stop bit)
	UartCfg.Baud_rate = baudrate;
	UART_Init(LPC_UART3, &UartCfg);

	// Setup UART FIFO, Enable Rx and Tx Fifo
	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&FifoConfig);
	UART_FIFOConfig(LPC_UART3, &FifoConfig);
	// Enable UART Transmit
	UART_TxCmd(LPC_UART3, ENABLE);

	/* Initialize Semaphore */
    uart3Semaphore = xSemaphoreCreateMutex();
}

unsigned int UART3_SendBytes(char *data, int len)
{
	int retval;

	UART3_AcquireBus();
	retval = UART_Send(LPC_UART3, (uint8_t *)data, len, BLOCKING);
	UART3_ReleaseBus();

	return retval;
}

unsigned int UART3_ReceiveBytes(char * data, int len)
{
	int retval;

	if (data == NULL) return 0;
	UART3_AcquireBus();
	retval = UART_Receive(LPC_UART3, (uint8_t *) data, len, BLOCKING);
	UART3_ReleaseBus();

	return retval;
}

