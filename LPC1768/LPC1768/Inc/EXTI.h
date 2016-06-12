/*
 * EXTI.h
 *
 *  Created on: May 29, 2016
 *      Author: vpcol
 */

#ifndef EXTI_H_
#define EXTI_H_


#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"


#ifdef __cpluplus
extern "C" {
#endif


#define EXTI3_INT_PRIORITY	0x10
#define TOUCH_TIMEOUT 10000

void Init_EXTI();
int Touch_ReadIntPin();
int Keyboard_ReadIntPin();
void EXTI3_Enable();
void EXTI3_Disable();

#ifdef __cplusplus
}
#endif


#endif /* EXTI_H_ */
