/*
 * SSP1.h
 *
 *  Created on: May 29, 2016
 *      Author: vpcol
 */

#ifndef SSP1_H_
#define SSP1_H_

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"

#ifdef __cplusplus
extern "C" {
#endif


// SSP1 Configuration
#define SSP1_CLOCK_RATE			25000000 		// 10Mhz, can be faster
#define SSP1_POLARITY			SSP_CPOL_LO		// 0 - Clock Polarit Low
#define SSP1_PHASE				SSP_CPHA_FIRST	// 0 - Sample on first rising/low edge

// some defines for the SSP1 DMA use
#define DMA_CHANNEL_ENABLE      1
#define DMA_TRANSFER_TYPE_M2P   (1UL << 11)
#define DMA_CHANNEL_TCIE        (1UL << 31)
#define DMA_CHANNEL_SRC_INC     (1UL << 26)
#define DMA_MASK_IE             (1UL << 14)
#define DMA_MASK_ITC            (1UL << 15)
#define DMA_SSP1_TX             (1UL << 2)
#define DMA_SSP0_TX             (0)
#define DMA_DEST_SSP1_TX        (2UL << 6)
#define DMA_DEST_SSP0_TX        (0UL << 6)

void Init_SSP1(void);
void SSP1_AcquireBus();
void SSP1_ReleaseBus();
void SSP1_SetConfig(uint32_t speed, uint32_t polarity, uint32_t phase, uint32_t databit);
void SSP1_16(int s);
void SSP1_Write(unsigned short data);
char SSP1_Read(void);
int SSP1_Transfer(unsigned short data);
void SSP1_WaitBusy();
void SSP1_SharedCSSet(uint32_t port, uint32_t pin, int val);
void SSP1_WriteDMA(int val, int numbytes);

#ifdef __cplusplus
}
#endif

#endif /* SSP1_H_ */
