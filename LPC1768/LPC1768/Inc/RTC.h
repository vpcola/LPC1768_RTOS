/*
 * RTC.h
 *
 *  Created on: 30 May, 2016
 *      Author: tkcov
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "lpc17xx.h"
#include "lpc17xx_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*RTCAlarmCallback)(void);
typedef void (*RTCUpdateCallback)(uint32_t secval);

void Init_RTC(void);
// Start RTC/Interrupts
void RTC_Start(void);
// Stop interrupts
void RTC_Stop(void);
void RTC_SetUpdateCallback(RTCUpdateCallback callback);
void RTC_SetAlarmSeconds(RTCAlarmCallback callback, int seconds);

#ifdef __cplusplus
}
#endif


#endif /* INC_RTC_H_ */
