/*
 * XPT2046.h
 *
 *  Created on: 30 May, 2016
 *      Author: tkcov
 */

#ifndef INC_XPT2046_H_
#define INC_XPT2046_H_

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"

#ifdef __cplusplus
extern "C" {
#endif


#define XPT2046_READ_X 0xD0
#define XPT2046_READ_Y 0x90
#define XPT2046_NOP	   0x0

// Touch uses SSP0, make sure its initialized first
void Init_XPT2046(void);
int  XPT2046_Wait(int * x, int * y, int timeout);



#ifdef __cplusplus
}
#endif


#endif /* INC_XPT2046_H_ */
