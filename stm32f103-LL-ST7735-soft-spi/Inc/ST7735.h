#ifndef ST7735_H
#define ST7735_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "stm32f1xx_ll_gpio.h"
#include "fonts.h"

/*
#define ST7735_NOP       0x00
#define ST7735_SWRESET   0x01
#define ST7735_RDDID     0x04
#define ST7735_RDDST     0x09

#define ST7735_SLPIN     0x10
#define ST7735_SLPOUT    0x11
#define ST7735_PTLON     0x12
#define ST7735_NORON     0x13

#define ST7735_RDMODE    0x0A
#define ST7735_RDMADCTL  0x0B
#define ST7735_RDPIXFMT  0x0C
#define ST7735_RDIMGFMT  0x0D
#define ST7735_RDSELFDIAG 0x0F

#define ST7735_INVOFF    0x20
#define ST7735_INVON     0x21
#define ST7735_GAMMASET  0x26
#define ST7735_DISPOFF   0x28
#define ST7735_DISPON    0x29

#define ST7735_CASET     0x2A
#define ST7735_PASET     0x2B
#define ST7735_RAMWR     0x2C
#define ST7735_RAMRD     0x2E

#define ST7735_PTLAR     0x30
#define ST7735_MADCTL    0x36
#define ST7735_PIXFMT    0x3A

#define ST7735_FRMCTR1   0xB1
#define ST7735_FRMCTR2   0xB2
#define ST7735_FRMCTR3   0xB3
#define ST7735_INVCTR    0xB4
#define ST7735_DFUNCTR   0xB6

#define ST7735_PWCTR1    0xC0
#define ST7735_PWCTR2    0xC1
#define ST7735_PWCTR3    0xC2
#define ST7735_PWCTR4    0xC3
#define ST7735_PWCTR5    0xC4
#define ST7735_VMCTR1    0xC5
#define ST7735_VMCTR2    0xC7

#define ST7735_RDID1     0xDA
#define ST7735_RDID2     0xDB
#define ST7735_RDID3     0xDC
#define ST7735_RDID4     0xDD

#define ST7735_GMCTRP1   0xE0
#define ST7735_GMCTRN1   0xE1
*/

/*
#define ST7735_PWCTR6    0xFC
*/

// Color definitions
#define ST7735_BLACK       0x0000      /*   0,   0,   0 */
#define ST7735_NAVY        0x000F      /*   0,   0, 128 */
#define ST7735_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ST7735_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ST7735_MAROON      0x7800      /* 128,   0,   0 */
#define ST7735_PURPLE      0x780F      /* 128,   0, 128 */
#define ST7735_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ST7735_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ST7735_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ST7735_BLUE        0x001F      /*   0,   0, 255 */
#define ST7735_GREEN       0x07E0      /*   0, 255,   0 */
#define ST7735_CYAN        0x07FF      /*   0, 255, 255 */
#define ST7735_RED         0xF800      /* 255,   0,   0 */
#define ST7735_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ST7735_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ST7735_WHITE       0xFFFF      /* 255, 255, 255 */
#define ST7735_ORANGE      0xFD20      /* 255, 165,   0 */
#define ST7735_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ST7735_PINK        0xF81F

// Memory Access Control
#define ST7735_MADCTL     0x36
#define ST7735_MADCTL_MY  0x80  // < Bottom to top
#define ST7735_MADCTL_MX  0x40  // < Right to left
#define ST7735_MADCTL_MV  0x20  // < Reverse Mode
#define ST7735_MADCTL_ML  0x10  // < LCD refresh Bottom to top
#define ST7735_MADCTL_RGB 0x00  // < Red-Green-Blue pixel order
#define ST7735_MADCTL_BGR 0x08  // < Blue-Green-Red pixel order
#define ST7735_MADCTL_MH  0x04  // < LCD refresh right to left
#define ST7735_MADCTL_SWAP_XY_Mirror_Y  0xa0  // < X-Y Exchange,Y-Mirror
#define ST7735_MADCTL_SWAP_XY_Mirror_X  0x60  // < X-Y Exchange,X-Mirror
#define ST7735_MADCTL_Mirror_XY  0xc0         // < X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB
#define ST7735_ROTATION    (ST7735_MADCTL_MX | ST7735_MADCTL_BGR)

#define swap(a,b) {int16_t t=a;a=b;b=t;}

void ST7735_SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7735_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawPixel(int x, int y, uint16_t color);
void ST7735_DrawLine(uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ST7735_DrawRect(uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ST7735_DrawCircle(uint16_t x0, uint16_t y0, int r, uint16_t color);
void ST7735_SetTextColor(uint16_t color);
void ST7735_SetBackColor(uint16_t color);
void ST7735_SetFont(sFONT *pFonts);
void ST7735_DrawChar(uint16_t x, uint16_t y, uint8_t c);
void ST7735_String(uint16_t x,uint16_t y, char *str);
void ST7735_SetRotation(uint8_t r);
void ST7735_FontsIni(void);
void ST7735_ini(uint16_t w_size, uint16_t h_size);

#endif
