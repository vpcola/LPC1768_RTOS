/*
 * LCD.c
 *
 *  Created on: 27 May, 2016
 *      Author: tkcov
 */

#include <string.h>
#include "LCD.h"
#include "cmsis_os.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_exti.h"

#include "SSP1.h"
#include "PINDEFINES.h"
#include "GPIO.h"


// Local variables needed to track
// changes
int orientation;
unsigned int char_x;
unsigned int char_y;
uint16_t foreground;
uint16_t background;

// Pointer to the currently selected font
unsigned char* font;

// Forward declaration of LCD_Reset
void LCD_Reset(void);
static void LCD_CS(int enable);

void Init_LCD_Pins(void)
{
    // Configure other pins used by the ILI9341 LCD
    // P.4 = Reset, P2.3 = DC
	PINSEL_CFG_Type LCDPin;
    LCDPin.Portnum = LCD_RESET_PORT;
    LCDPin.Pinnum = LCD_RESET_PIN;
    LCDPin.Funcnum = PINSEL_FUNC_0; // Use as GPIO
    LCDPin.OpenDrain = PINSEL_PINMODE_NORMAL;
    LCDPin.Pinmode = PINSEL_PINMODE_PULLUP;
    PINSEL_ConfigPin(&LCDPin);

    LCDPin.Portnum = LCD_DC_PORT;
    LCDPin.Pinnum = LCD_DC_PIN;
    PINSEL_ConfigPin(&LCDPin);

    // Declare LCD pins as output, disable pin masking
    GPIO_SetDir(LCD_RESET_PORT, (1 << LCD_RESET_PIN), GPIO_DIR_OUTPUT); 	// Reset
    GPIO_SetDir(LCD_DC_PORT, (1 << LCD_DC_PIN), GPIO_DIR_OUTPUT); 	// DC
}


void Init_LCD()
{
	// We assume SSP1 is already initialized
	Init_LCD_Pins();

    // Reset the LCD
    LCD_Reset();

    // Initialize the local variables
    orientation = 1;
    // First line for an 8x8 font
    LCD_Locate(0, 8);

}

static void LCD_CS(int enable)
{
   	SSP1_SharedCSSet(LCD_CS_PORT, LCD_CS_PIN, enable);
}

static void LCD_RST(int enable)
{
    if (enable)
        GPIO_SetValue(LCD_RESET_PORT, (1 << LCD_RESET_PIN));
    else
        GPIO_ClearValue(LCD_RESET_PORT, (1 << LCD_RESET_PIN));
}

static void LCD_DC(int enable)
{
    if (enable)
        GPIO_SetValue(LCD_DC_PORT, (1 << LCD_DC_PIN));
    else
        GPIO_ClearValue(LCD_DC_PORT, (1 << LCD_DC_PIN));
}

int LCD_Width()
{
    if (orientation == 0 || orientation == 2) return 240;
    else return 320;
}


int LCD_Height()
{
    if (orientation == 0 || orientation == 2) return 320;
    else return 240;
}

// Set command mode and assert CS, also switches to 8 bit mode
// does not set back CS to 1 after returning.
static void LCD_Cmd(unsigned char cmd)
{
    LCD_DC(0); // Set to command mode
    LCD_CS(0); // Assert the CS line

    // Switch to 8 bit mode;
    SSP1_16(0);

    SSP1_Write(cmd);
    // Wait until it sends out correctly
    SSP1_WaitBusy();
    LCD_DC(1);	// Exit command mode
}

void LCD_Pixel(int x, int y, int color)
{
	SSP1_AcquireBus();

    LCD_Cmd(0x2A);   // Set the Horizontal GRAM Address
    SSP1_16(1);		 // 16 bit mode
    SSP1_Write(x);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0x2B);  // Set the Vertical GRAM Address
    SSP1_16(1);		// 16 bit mode
    SSP1_Write(y);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0x2C);   // Send pixel
    SSP1_16(1);
    SSP1_Write(color);
    SSP1_WaitBusy();

    SSP1_16(0);		// Switch back to 8 bit
    LCD_CS(1);		// Before releasing cs line

    SSP1_ReleaseBus();
}


static void LCD_Window (unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    LCD_Cmd(0x2A);
    SSP1_16(1);
    SSP1_Write(x);
    SSP1_Write(x+w-1);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0x2B);
    SSP1_16(1);
    SSP1_Write(y) ;
    SSP1_Write(y+h-1);
    SSP1_WaitBusy();

    SSP1_16(0); // Switch back to 8 bit
    LCD_CS(1);  // before releasing cs line
}

static void LCD_WindowMax (void)
{
    LCD_Window (0, 0, LCD_Width(),  LCD_Height());
}

// LCD Reset Sequence is copied from
// internet sources. There are a number of
// command registers needed to be set, some of
// which I wonder what they are being used for?
void LCD_Reset()
{
	SSP1_AcquireBus();

    //int i;
    LCD_CS(1);        // cs high
    LCD_DC(1);        // dc high
    LCD_RST(0);       // display reset

    // wait_us(50);
    osDelay(1);         // Delay 1 ms
    LCD_RST(1);       // end hardware reset
    osDelay(5);

    LCD_Cmd(0x01);    // SW reset
    osDelay(5);
    LCD_Cmd(0x28);    // display off

    /* Start Initial Sequence */
    LCD_Cmd(0xCF);
    SSP1_Write(0x00);
    SSP1_Write(0x83);
    SSP1_Write(0x30);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xED);
    SSP1_Write(0x64);
    SSP1_Write(0x03);
    SSP1_Write(0x12);
    SSP1_Write(0x81);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xE8);
    SSP1_Write(0x85);
    SSP1_Write(0x01);
    SSP1_Write(0x79);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xCB);
    SSP1_Write(0x39);
    SSP1_Write(0x2C);
    SSP1_Write(0x00);
    SSP1_Write(0x34);
    SSP1_Write(0x02);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xF7);
    SSP1_Write(0x20);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xEA);
    SSP1_Write(0x00);
    SSP1_Write(0x00);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xC0);          // POWER_CONTROL_1
    SSP1_Write(0x26);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xC1);          // POWER_CONTROL_2
    SSP1_Write(0x11);
    SSP1_WaitBusy();
    LCD_CS(1);
    SSP1_ReleaseBus();

    LCD_Cmd(0xC5);          // VCOM_CONTROL_1
    SSP1_Write(0x35);
    SSP1_Write(0x3E);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xC7);          // VCOM_CONTROL_2
    SSP1_Write(0xBE);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0x36);          // MEMORY_ACCESS_CONTROL
    SSP1_Write(0x48);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0x3A);          // COLMOD_PIXEL_FORMAT_SET
    SSP1_Write(0x55);        // 16 bit pixel
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xB1);          // Frame Rate
    SSP1_Write(0x00);
    SSP1_Write(0x1B);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xF2);          // Gamma Function Disable
    SSP1_Write(0x08);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0x26);
    SSP1_Write(0x01);        // gamma set for curve 01/2/04/08
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xE0);          // positive gamma correction
    SSP1_Write(0x1F);
    SSP1_Write(0x1A);
    SSP1_Write(0x18);
    SSP1_Write(0x0A);
    SSP1_Write(0x0F);
    SSP1_Write(0x06);
    SSP1_Write(0x45);
    SSP1_Write(0x87);
    SSP1_Write(0x32);
    SSP1_Write(0x0A);
    SSP1_Write(0x07);
    SSP1_Write(0x02);
    SSP1_Write(0x07);
    SSP1_Write(0x05);
    SSP1_Write(0x00);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xE1);          // negativ gamma correction
    SSP1_Write(0x00);
    SSP1_Write(0x25);
    SSP1_Write(0x27);
    SSP1_Write(0x05);
    SSP1_Write(0x10);
    SSP1_Write(0x09);
    SSP1_Write(0x3A);
    SSP1_Write(0x78);
    SSP1_Write(0x4D);
    SSP1_Write(0x05);
    SSP1_Write(0x18);
    SSP1_Write(0x0D);
    SSP1_Write(0x38);
    SSP1_Write(0x3A);
    SSP1_Write(0x1F);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_WindowMax();

    LCD_Cmd(0xB7);          // entry mode
    SSP1_Write(0x07);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0xB6);          // display function control
    SSP1_Write(0x0A);
    SSP1_Write(0x82);
    SSP1_Write(0x27);
    SSP1_Write(0x00);
    SSP1_WaitBusy();
    LCD_CS(1);

    LCD_Cmd(0x11);          // sleep out
    SSP1_WaitBusy();
    LCD_CS(1);

    // wait_ms(100);
    osDelay(100);

    LCD_Cmd(0x29);          // display on
    SSP1_WaitBusy();
    LCD_CS(1);

    // wait_ms(100);
    osDelay(100);

    SSP1_ReleaseBus();
}

void LCD_SetOrientation(unsigned int o)
{
	SSP1_AcquireBus();

    orientation = o;
    LCD_Cmd(0x36);                     // MEMORY_ACCESS_CONTROL
    switch (orientation) {
        case 0:
            SSP1_Write(0x48);
            break;
        case 1:
            SSP1_Write(0x28);
            break;
        case 2:
            SSP1_Write(0x88);
            break;
        case 3:
            SSP1_Write(0xE8);
            break;
    }
    SSP1_WaitBusy();    // wait for end of transfer
    LCD_CS(1);

    LCD_WindowMax();

    SSP1_ReleaseBus();
}

void LCD_Vline(int x, int y0, int y1, int color)
{
    int h,y;

    SSP1_AcquireBus();

    h = y1 - y0 + 1;
    LCD_Window(x,y0,1,h);
    LCD_Cmd(0x2C);
    SSP1_16(1);
    // switch to 16 bit
    for (y=0; y<h; y++) {
        SSP1_Write(color);
    }
    SSP1_WaitBusy();
    SSP1_16(0);
    LCD_CS(1);
    LCD_WindowMax();

    SSP1_ReleaseBus();

    return;
}

void LCD_Hline(int x0, int x1, int y, int color)
{
    int w,j;

    SSP1_AcquireBus();

    w = x1 - x0 + 1;
    LCD_Window(x0,y,w,1);
    LCD_Cmd(0x2C); // Send pixel command
    SSP1_16(1);	// Switch to 16 bit
    for (j=0; j<w; j++) {
        SSP1_Write(color);
    }
    SSP1_WaitBusy();
    SSP1_16(0);
    LCD_CS(1);
    LCD_WindowMax();

    SSP1_ReleaseBus();

    return;
}

void LCD_Line(int x0, int y0, int x1, int y1, int color)
{
    int   dx = 0, dy = 0;
    int   dx_sym = 0, dy_sym = 0;
    int   dx_x2 = 0, dy_x2 = 0;
    int   di = 0;

    dx = x1-x0;
    dy = y1-y0;

    if (dx == 0) {        /* vertical line */
        if (y1 > y0) LCD_Vline(x0,y0,y1,color);
        else LCD_Vline(x0,y1,y0,color);
        return;
    }

    if (dx > 0) {
        dx_sym = 1;
    } else {
        dx_sym = -1;
    }
    if (dy == 0) {        /* horizontal line */
        if (x1 > x0) LCD_Hline(x0,x1,y0,color);
        else  LCD_Hline(x1,x0,y0,color);
        return;
    }

    if (dy > 0) {
        dy_sym = 1;
    } else {
        dy_sym = -1;
    }

    dx = dx_sym*dx;
    dy = dy_sym*dy;

    dx_x2 = dx*2;
    dy_x2 = dy*2;

    if (dx >= dy) {
        di = dy_x2 - dx;
        while (x0 != x1) {

            LCD_Pixel(x0, y0, color);
            x0 += dx_sym;
            if (di<0) {
                di += dy_x2;
            } else {
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }
        LCD_Pixel(x0, y0, color);
    } else {
        di = dx_x2 - dy;
        while (y0 != y1) {
            LCD_Pixel(x0, y0, color);
            y0 += dy_sym;
            if (di < 0) {
                di += dx_x2;
            } else {
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }
        LCD_Pixel(x0, y0, color);
    }
    return;
}

void LCD_Rect(int x0, int y0, int x1, int y1, int color)
{

    if (x1 > x0) LCD_Hline(x0,x1,y0,color);
    else  LCD_Hline(x1,x0,y0,color);

    if (y1 > y0) LCD_Vline(x0,y0,y1,color);
    else LCD_Vline(x0,y1,y0,color);

    if (x1 > x0) LCD_Hline(x0,x1,y1,color);
    else  LCD_Hline(x1,x0,y1,color);

    if (y1 > y0) LCD_Vline(x1,y0,y1,color);
    else LCD_Vline(x1,y1,y0,color);

    return;
}


void LCD_FillRect(int x0, int y0, int x1, int y1, int color)
{
    int h = y1 - y0 + 1;
    int w = x1 - x0 + 1;
    int pixels = h * w;

    SSP1_AcquireBus();

    //unsigned int dma_count;
    LCD_Window(x0,y0,w,h);

    LCD_Cmd(0x2C);  // send pixel
    SSP1_16(1);     // Use 16 bit transfers

    // Write to LCD using DMA
    SSP1_WriteDMA(color, pixels);
    SSP1_WaitBusy();    // wait for end of transfer

    SSP1_16(0);
    LCD_CS(1);
    LCD_WindowMax();

    SSP1_ReleaseBus();

    return;
}

void LCD_ClearScreen ()
{
    // we can use the fillrect function
    LCD_FillRect(0,0,LCD_Width()-1, LCD_Height()-1, background);
}

void LCD_Circle(int x0, int y0, int r, int color)
{

    int x = -r, y = 0, err = 2-2*r, e2;
    do {
        LCD_Pixel(x0-x, y0+y,color);
        LCD_Pixel(x0+x, y0+y,color);
        LCD_Pixel(x0+x, y0-y,color);
        LCD_Pixel(x0-x, y0-y,color);
        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);
}

void LCD_FillCircle(int x0, int y0, int r, int color)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do {
        LCD_Vline(x0-x, y0-y, y0+y, color);
        LCD_Vline(x0+x, y0-y, y0+y, color);
        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);
}

void LCD_Locate(int x, int y)
{
    char_x = x;
    char_y = y;
}

int LCD_Columns()
{
	// The second char in the font array
	// identifies the with of each font
    return LCD_Width() / font[1];
}


int LCD_Rows()
{
	// The third char in the font array
	// identifies the height of each font
    return LCD_Height() / font[2];
}

void LCD_Character(int x, int y, int c)
{
    unsigned int hor,vert,offset,bpl,j,i,b;
    unsigned char* charbit;
    unsigned char z,w;
#ifdef use_ram
    unsigned int pixel;
    unsigned int p;
    unsigned int dma_count,dma_off;
    uint16_t *buffer;
#endif

    SSP1_AcquireBus();

    // Position characters
    LCD_Locate(x, y);

    if ((c < 31) || (c > 127)) return;   // test char range

    // read font parameter from start of array
    offset = font[0];                    // bytes / char
    hor = font[1];                       // get hor size of font
    vert = font[2];                      // get vert size of font
    bpl = font[3];                       // bytes per line

    if (((int) (char_x + hor)) > LCD_Width()) {
        char_x = 0;
        char_y = char_y + vert;
        if (((int) char_y) >= LCD_Height() - font[2]) {
            char_y = 0;
        }
    }
    LCD_Window(char_x, char_y,hor,vert);           // setup char box
    LCD_Cmd(0x2C);	// Send pixel command

    SSP1_16(1);   // switch to 16 bit Mode

#ifdef LCD_USE_RAM
    pixel = hor * vert;                        // calculate buffer size
    buffer = (uint16_t *) malloc (2*pixel);    // we need a buffer for the font
    if(buffer != NULL) {                       // there is memory space -> use dma
        charbit = &font[((c -32) * offset) + 4]; // start of char bitmap
        w = charbit[0];                          // width of actual char
        p = 0;
        // construct the font into the buffer
        for (j=0; j<vert; j++) {            //  vert line
            for (i=0; i<hor; i++) {         //  horz line
                z =  charbit[bpl * i + ((j & 0xF8) >> 3)+1];
                b = 1 << (j & 0x07);
                if (( z & b ) == 0x00) {
                    buffer[p] = background;
                } else {
                    buffer[p] = foreground;
                }
                p++;
            }
        }
        // copy the buffer with DMA SPI to display
        dma_off = 0;  // offset for DMA transfer

        LPC_GPDMACH0->DMACCDestAddr = (uint32_t)&LPC_SSP1->DR; // we send to SSP1
        LPC_SSP1->DMACR = 0x2;

        // start DMA
        do {
            if (pixel > 4095) {         // this is a giant font !
                dma_count = 4095;
                pixel = pixel - 4095;
            } else {
                dma_count = pixel;
                pixel = 0;
            }
            LPC_GPDMA->DMACIntTCClear = 0x1;
            LPC_GPDMA->DMACIntErrClr = 0x1;
            LPC_GPDMACH0->DMACCSrcAddr = (uint32_t) (buffer + dma_off);
            LPC_GPDMACH0->DMACCControl = dma_count | (1UL << 18) | (1UL << 21) | (1UL << 31) |  DMA_CHANNEL_SRC_INC ; // 16 bit transfer , address increment, interrupt
            LPC_GPDMACH0->DMACCConfig  = DMA_CHANNEL_ENABLE | DMA_TRANSFER_TYPE_M2P | DMA_DEST_SSP1_TX);
            LPC_GPDMA->DMACSoftSReq = 0x1;
            do {
            } while ((LPC_GPDMA->DMACRawIntTCStat & 0x01) == 0); // DMA is running
            dma_off = dma_off + dma_count;
        } while (pixel > 0);


        SSP1_WaitBusy();
        free ((uint16_t *) buffer);
        SSP1_16(0);
    }

    else {
#endif
        charbit = &font[((c -32) * offset) + 4]; // start of char bitmap
        w = charbit[0];                          // width of actual char
        for (j=0; j<vert; j++) {  //  vert line
            for (i=0; i<hor; i++) {   //  horz line
                z =  charbit[bpl * i + ((j & 0xF8) >> 3)+1];
                b = 1 << (j & 0x07);
                if (( z & b ) == 0x00) {
                    SSP1_Write(background);
                } else {
                    SSP1_Write(foreground);
                }
            }
        }
        SSP1_WaitBusy();
        // LCD_CS(1);
        SSP1_16(0);
#ifdef LCD_USE_RAM
    }
#endif

    LCD_CS(1);
    LCD_WindowMax();

    SSP1_ReleaseBus();

    if (((unsigned int)(w + 2)) < hor) {                   // x offset to next char
        char_x += w + 2;
    } else char_x += hor;
}


int LCD_Putc(int value)
{
    if (value == '\n') {    // new line
        char_x = 0;
        char_y = char_y + font[2];
        if (((int) char_y) >= LCD_Height() - font[2]) {
            char_y = 0;
        }
    } else {
        LCD_Character(char_x, char_y, value);
    }
    return value;
}

void LCD_PutString(const char * string)
{
	char * str = (char *)string;
	while (*str)
		LCD_Putc(*str++);
}

void LCD_PutStr(int x, int y, const char * string)
{
	LCD_Locate(x, y);
	char * str = (char *)string;
	while (*str != 0)
		LCD_Putc(*str++);
}

int LCD_GetStringExtentX(const char * string)
{
	unsigned int strlength = strlen(string);
	unsigned int offset, totallen, w, i;
	unsigned char * charbit;
	unsigned char c;


    // read font parameter from start of array
    offset = font[0];

    totallen = 0;
    for (i = 0; i < strlength; i++)
    {
    	c = string[i];
    	charbit = &font[((c -32) * offset) + 4]; // start of char bitmap
    	w = charbit[0]; // actual size of the font
    	totallen += w + 2;
    }
    return totallen;
}

int LCD_GetStringExtentY()
{
	return font[2];
}

 void LCD_GetStringExtent(const char * string, const unsigned char * cfont, int * extwidth, int * extheight)
{
	unsigned int strlength = strlen(string);
	unsigned int offset, totallen, vert, w, i;
	unsigned char * charbit, c;


    // read font parameter from start of array
    // read font parameter from start of array
    offset = cfont[0];
    vert = cfont[2];                      // get vert size of font
    totallen = 0;
    for (i = 0; i < strlength; i++)
    {
    	c = string[i];
    	charbit = &cfont[((c -32) * offset) + 4]; // start of char bitmap
    	w = charbit[0];
    	totallen += w + 2;
    }
    *extwidth = totallen;
    *extheight = vert;
}

void LCD_Foreground(uint16_t colour)
{
    foreground = colour;
}

void LCD_Background(uint16_t colour)
{
    background = colour;
}

void LCD_SetFont(const unsigned char* f)
{
	font = f;
}

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int w, unsigned int h,unsigned char *bitmap)
{
    unsigned int  j;
    int padd;
    unsigned short *bitmap_ptr = (unsigned short *)bitmap;

    unsigned int i;

    SSP1_AcquireBus();

    // the lines are padded to multiple of 4 bytes in a bitmap
    padd = -1;
    do {
        padd ++;
    } while (2*(w + padd)%4 != 0);
    LCD_Window(x, y, w, h);
    bitmap_ptr += ((h - 1)* (w + padd));
    LCD_Cmd(0x2C);  // send pixel
    SSP1_16(1);
    for (j = 0; j < h; j++) {         //Lines
        for (i = 0; i < w; i++) {     // one line
            SSP1_Write(*bitmap_ptr);    // one line
            bitmap_ptr++;
        }
        bitmap_ptr -= 2*w;
        bitmap_ptr -= padd;
    }
    SSP1_WaitBusy();
    SSP1_16(0);
    LCD_CS(1);
    LCD_WindowMax();

    SSP1_ReleaseBus();

}

#ifdef FATFS_SUPPORTED

int LCD_DisplaBMP(unsigned int x, unsigned int y, const char *Name_BMP)
{

#define OffsetPixelWidth    18
#define OffsetPixelHeigh    22
#define OffsetFileSize      34
#define OffsetPixData       10
#define OffsetBPP           28

    char filename[50];
    unsigned char BMP_Header[54];
    unsigned short BPP_t;
    unsigned int PixelWidth,PixelHeigh,start_data;
    unsigned int    i,off;
    int padd,j;
    unsigned short *line;

    // get the filename
    i=0;
    while (*Name_BMP!='\0') {
        filename[i++]=*Name_BMP++;
    }
    filename[i] = 0;

    FILE *Image = fopen((const char *)&filename[0], "rb");  // open the bmp file
    if (!Image) {
        return(0);      // error file not found !
    }

    fread(&BMP_Header[0],1,54,Image);      // get the BMP Header

    if (BMP_Header[0] != 0x42 || BMP_Header[1] != 0x4D) {  // check magic byte
        fclose(Image);
        return(-1);     // error no BMP file
    }

    BPP_t = BMP_Header[OffsetBPP] + (BMP_Header[OffsetBPP + 1] << 8);
    if (BPP_t != 0x0010) {
        fclose(Image);
        return(-2);     // error no 16 bit BMP
    }

    PixelHeigh = BMP_Header[OffsetPixelHeigh] + (BMP_Header[OffsetPixelHeigh + 1] << 8) + (BMP_Header[OffsetPixelHeigh + 2] << 16) + (BMP_Header[OffsetPixelHeigh + 3] << 24);
    PixelWidth = BMP_Header[OffsetPixelWidth] + (BMP_Header[OffsetPixelWidth + 1] << 8) + (BMP_Header[OffsetPixelWidth + 2] << 16) + (BMP_Header[OffsetPixelWidth + 3] << 24);
    if (PixelHeigh > height() + y || PixelWidth > width() + x) {
        fclose(Image);
        return(-3);      // to big
    }

    start_data = BMP_Header[OffsetPixData] + (BMP_Header[OffsetPixData + 1] << 8) + (BMP_Header[OffsetPixData + 2] << 16) + (BMP_Header[OffsetPixData + 3] << 24);

    line = (unsigned short *) malloc (2 * PixelWidth); // we need a buffer for a line
    if (line == NULL) {
        return(-4);         // error no memory
    }

    // the bmp lines are padded to multiple of 4 bytes
    padd = -1;
    do {
        padd ++;
    } while ((PixelWidth * 2 + padd)%4 != 0);

    LCD_Window(x, y,PixelWidth ,PixelHeigh);
    LCD_Cmd(0x2C);  // send pixel
    SSP1_16(1);
    for (j = PixelHeigh - 1; j >= 0; j--) {               //Lines bottom up
        off = j * (PixelWidth  * 2 + padd) + start_data;   // start of line
        fseek(Image, off ,SEEK_SET);
        fread(line,1,PixelWidth * 2,Image);       // read a line - slow
        for (i = 0; i < PixelWidth; i++) {        // copy pixel data to TFT
            SSP1_Write(line[i]);                  // one 16 bit pixel
        }
    }
    SSP1_WaitBusy();
    LCD_CS(1);
    SSP1_16(0);
    free (line);
    fclose(Image);
    LCD_WindowMax();
    return(1);
}

#endif

