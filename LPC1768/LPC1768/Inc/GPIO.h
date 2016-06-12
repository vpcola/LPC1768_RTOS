/*
 * GPIO.h
 *
 *  Created on: 26 May, 2016
 *      Author: tkcov
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_


#include "PINDEFINES.h"

#ifdef __cplusplus
extern "C" {
#endif

void Init_GPIO(void);

void Led1(int value);
void Led2(int value);
void Led3(int value);
void Led4(int value);

#ifdef __cpluplus
}
#endif

#endif /* INC_GPIO_H_ */
