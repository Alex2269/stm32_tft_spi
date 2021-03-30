#ifndef ST7735_H
#define ST7735_H

#include <stdbool.h>
#include <stdint.h>

#include "glcdfont.h"
#include "stm32f1xx_ll_gpio.h"

#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT 160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_RDMODE  0x0A
#define ST7735_RDMADCTL  0x0B
#define ST7735_RDPIXFMT  0x0C
#define ST7735_RDIMGFMT  0x0D
#define ST7735_RDSELFDIAG  0x0F

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_GAMMASET 0x26
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29

#define ST7735_CASET   0x2A
#define ST7735_PASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_MADCTL  0x36
#define ST7735_PIXFMT  0x3A

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DFUNCTR 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_VMCTR2  0xC7

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1
/*
#define ST7735_PWCTR6  0xFC

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

void lcd_reset(void);
void ST7735_begin(void);
void ST7735_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void ST7735_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void ST7735_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void ST7735_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void ST7735_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void ST7735_fillScreen(uint16_t color);
void ST7735_invertDisplay(bool i);
void ST7735_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void ST7735_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void ST7735_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void ST7735_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void ST7735_setTextSize(uint8_t s);
uint16_t ST7735_getTextColor(void);
void ST7735_setTextColor(uint16_t c);
uint16_t ST7735_getTextBgColor(void);
void ST7735_setTextBgColor(uint16_t c);
void ST7735_setTextWrap(bool w);
void ST7735_setCursor(int16_t x, int16_t y);
uint16_t ST7735_getCursorX(void);
uint16_t ST7735_getCursorY(void);
void ST7735_draw_value(int64_t value);
void ST7735_draw_fvalue(float flt, uint8_t afterpoint);
void ST7735_writeString(uint8_t *s);
void ST7735_write(uint8_t c);
void ST7735_getTextBounds(char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
void ST7735_cp437(bool x);
uint16_t ST7735_width(void);
uint16_t ST7735_height(void);
void ST7735_pushColor(uint16_t color);
void ST7735_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void ST7735_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
uint16_t ST7735_color565(uint8_t r, uint8_t g, uint8_t b);
void ST7735_setRotation(uint8_t m);
uint16_t ST7735_getRotation(void);
 
#endif
