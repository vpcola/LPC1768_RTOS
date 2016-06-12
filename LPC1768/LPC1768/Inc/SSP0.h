/*
 * SSP0.h
 *
 *  Created on: May 29, 2016
 *      Author: vpcol
 */

#ifndef SSP0_H_
#define SSP0_H_

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"

#ifdef __cplusplus
extern "C" {
#endif


// SSP0 Configuration
#define SSP0_CLOCK_RATE			2000000 		// 2Mhz, can be faster
#define SSP0_POLARITY			SSP_CPOL_LO		// 0 - Clock Polarity Low
#define SSP0_PHASE				SSP_CPHA_FIRST	// 0 - Sample on first rising/low edge


void Init_SSP0(void);
void SSP0_AcquireBus(void);
void SSP0_ReleaseBus(void);
void SSP0_SetConfig(uint32_t speed, uint32_t polarity, uint32_t phase, uint32_t databit);
void SSP0_16(int s);
void SSP0_Write(unsigned short data);
char SSP0_Read(void);
int SSP0_Transfer(unsigned short data);
void SSP0_WaitBusy();
void SSP0_SharedCSSet(uint32_t port, uint32_t pin, int val);

#ifdef __cplusplus
}
#endif

#endif /* SSP0_H_ */
