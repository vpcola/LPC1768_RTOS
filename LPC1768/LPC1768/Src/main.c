
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpio.h"

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "GPIO.h"
#include "I2C.h"
#include "HTU21D.h"
#include "SEVENSEGMENT.h"
#include "SSP0.h"
#include "SSP1.h"
#include "XPT2046.h"
#include "LCD.h"
#include "EXTI.h"
#include "Arial12x12.h"

static int led2val = 1;
int GUI_STARTED = 0;

void DefaultTask(void * p)
{
	unsigned char num;
	// Init_LCD() uses delays/semaphores
	Init_LCD();
	Init_XPT2046();

	LCD_SetFont(Arial12x12);

	LCD_SetOrientation(2);
	LCD_Background(Black);
	LCD_Foreground(White);

	GUI_STARTED = 1;

	LCD_ClearScreen();
	LCD_FillRect(10, 10, 100, 100, Red);
	LCD_Rect(5, 5, 105, 105, Blue );

	num = 0;
	while(1)
	{
		Led1(0);
		osDelay(500);
		Led1(1);
		osDelay(500);
		SEVENSEG_Set(num);
		num = (num < 10) ? num + 1 : 0;
	}
}

void ReadTemperatureTask(void *p)
{
	float temp;
	char buffer[100];
	do {
		if (HTU21D_ReadTemperature(&temp) == 0)
		{
			if (GUI_STARTED)
			{
				sprintf(buffer,"Temp: %.2f", temp);
				LCD_PutStr(10,200, buffer);
			}
		}
		osDelay(1000);

	}while(1);
}

void Keyboard_Update(void)
{
	if (led2val == 1)
		led2val = 0;
	else if (led2val == 0)
		led2val = 1;

	Led2(led2val);
}

int main(void)
{
    uint8_t ret = pdFALSE;

    Init_GPIO();
	Init_I2C2();
	Init_SevenSeg();
	Init_HTU21D();
	Init_EXTI();
	Init_SSP1();
	Init_SSP0();

	// Create temperature task
	xTaskCreate(ReadTemperatureTask, "TempTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	// Create a task for FreeRTOS
    ret = xTaskCreate(DefaultTask, "Default", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    if (ret == pdTRUE)
        vTaskStartScheduler();  // should never return

    // We should not get here ...
    while(1){}

    return 0 ;
}
