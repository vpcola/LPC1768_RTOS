/*
 * RTC.c
 *
 *  Created on: 30 May, 2016
 *      Author: tkcov
 */

#include "RTC.h"

RTCUpdateCallback updateCallback = NULL;
RTCAlarmCallback alarmCallback = NULL;

void RTC_IRQHandler(void)
{
	uint32_t secval;

	/* This is increment counter interrupt*/
	if (RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE))
	{
		secval = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_SECOND);

		/* Call the callbacks for second update */
		if (updateCallback)
			updateCallback(secval);

		// Clear pending interrupt
		RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
	}

	/* Continue to check the Alarm match*/
	if (RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM))
	{
		/* Call the callback for the alarm */
		if (alarmCallback)
			alarmCallback();

		// Clear pending interrupt
		RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
	}
}

void Init_RTC(void)
{
	// Initialize the RTC
	RTC_Init(LPC_RTC);

    /* Disable RTC interrupt */
    NVIC_DisableIRQ(RTC_IRQn);
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(RTC_IRQn, ((0x01<<3)|0x01));

	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);
	RTC_CalibCounterCmd(LPC_RTC, DISABLE);

	/* Set current time for RTC */
#if 0
	// Current time is 8:00:00PM, 2016-05-30
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR, 22);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, 5);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, 2016);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 31);
#endif
}

void RTC_Start(void)
{
	/* Set the CIIR for second counter interrupt*/
	RTC_CntIncrIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);
	/* Set the AMR for 10s match alarm interrupt */
	RTC_AlarmIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);

    /* Enable RTC interrupt */
    NVIC_EnableIRQ(RTC_IRQn);
}

void RTC_Stop(void)
{
	// Disable interrupts, RTC still running
	/* Set the CIIR for second counter interrupt*/
	RTC_CntIncrIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, DISABLE);
	/* Set the AMR for 10s match alarm interrupt */
	RTC_AlarmIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, DISABLE);

    /* Enable RTC interrupt */
    NVIC_DisableIRQ(RTC_IRQn);
}



void RTC_SetUpdateCallback(RTCUpdateCallback callback)
{
	updateCallback = callback;
}

void RTC_SetAlarmSeconds(RTCAlarmCallback callback, int seconds)
{
	uint32_t sec;

	alarmCallback = callback;
	// Trigger alarm for the next seconds
	// First get the current time
	sec = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_SECOND);
	// trigger on the next x seconds, handle roll over
	sec = ((sec + seconds ) > 60) ? ((sec + seconds)- 60): (sec + seconds);

	RTC_SetAlarmTime (LPC_RTC, RTC_TIMETYPE_SECOND, sec);
}


