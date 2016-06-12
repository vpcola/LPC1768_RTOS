/*
 * LCD.h
 *
 *  Created on: 27 May, 2016
 *      Author: tkcov
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"

#ifdef __cpluplus
extern "C" {
#endif

#define RGB(r,g,b)  (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //5 red | 6 green | 5 blue


/* some RGB color definitions                                                 */
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255,   0 */
#define White           0xFFFF      /* 255, 255, 255 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */



// Initialization Routine
// Must be called once
void Init_LCD(void);

// Graphics Primitives
int LCD_Width();
int LCD_Height();
void LCD_Pixel(int x, int y, int color);
void LCD_SetOrientation(unsigned int o);
void LCD_Vline(int x, int y0, int y1, int color);
void LCD_Hline(int x0, int x1, int y, int color);
void LCD_Line(int x0, int y0, int x1, int y1, int color);
void LCD_Rect(int x0, int y0, int x1, int y1, int color);
void LCD_FillRect(int x0, int y0, int x1, int y1, int color);
void LCD_ClearScreen();
void LCD_Circle(int x0, int y0, int r, int color);
void LCD_FillCircle(int x0, int y0, int r, int color);

// Text/Font display functions
void LCD_SetFont(const unsigned char* font);
void LCD_Locate(int x, int y);
int LCD_Columns(void);
int LCD_Rows(void);
void LCD_Character(int x, int y, int character);
int LCD_Putc(int value);
void LCD_PutString(const char * string);
void LCD_PutStr(int x, int y, const char * string);
int LCD_GetStringExtentX(const char * string);
int LCD_GetStringExtentY();
void LCD_GetStringExtent(const char * string, const unsigned char * cfont, int * extwidth, int * extheight);
void LCD_Foreground(uint16_t colour);
void LCD_Background(uint16_t colour);

// Bitmap functions
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int w, unsigned int h,unsigned char *bitmap);


#ifdef __cplusplus
}
#endif

#endif /* INC_LCD_H_ */
