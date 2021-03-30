#ifndef ILI9488_H
#define ILI9488_H

#include <stdbool.h>
#include <stdint.h>

#include "glcdfont.h"
// #include "stm32f1xx_ll_gpio.h"

#define ILI9488_TFTWIDTH  320
#define ILI9488_TFTHEIGHT 480

#define ILI9488_NOP     0x00
#define ILI9488_SWRESET 0x01
#define ILI9488_RDDID   0x04
#define ILI9488_RDDST   0x09

#define ILI9488_SLPIN   0x10
#define ILI9488_SLPOUT  0x11
#define ILI9488_PTLON   0x12
#define ILI9488_NORON   0x13

#define ILI9488_RDMODE  0x0A
#define ILI9488_RDMADCTL  0x0B
#define ILI9488_RDPIXFMT  0x0C
#define ILI9488_RDIMGFMT  0x0D
#define ILI9488_RDSELFDIAG  0x0F

#define ILI9488_INVOFF  0x20
#define ILI9488_INVON   0x21
#define ILI9488_GAMMASET 0x26
#define ILI9488_DISPOFF 0x28
#define ILI9488_DISPON  0x29

#define ILI9488_CASET   0x2A
#define ILI9488_PASET   0x2B
#define ILI9488_RAMWR   0x2C
#define ILI9488_RAMRD   0x2E

#define ILI9488_PTLAR   0x30
#define ILI9488_MADCTL  0x36
#define ILI9488_PIXFMT  0x3A

#define ILI9488_FRMCTR1 0xB1
#define ILI9488_FRMCTR2 0xB2
#define ILI9488_FRMCTR3 0xB3
#define ILI9488_INVCTR  0xB4
#define ILI9488_DFUNCTR 0xB6

#define ILI9488_PWCTR1  0xC0
#define ILI9488_PWCTR2  0xC1
#define ILI9488_PWCTR3  0xC2
#define ILI9488_PWCTR4  0xC3
#define ILI9488_PWCTR5  0xC4
#define ILI9488_VMCTR1  0xC5
#define ILI9488_VMCTR2  0xC7

#define ILI9488_RDID1   0xDA
#define ILI9488_RDID2   0xDB
#define ILI9488_RDID3   0xDC
#define ILI9488_RDID4   0xDD

#define ILI9488_GMCTRP1 0xE0
#define ILI9488_GMCTRN1 0xE1
/*
#define ILI9488_PWCTR6  0xFC

*/

// Color definitions
#define ILI9488_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9488_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9488_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9488_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9488_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9488_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9488_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9488_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9488_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9488_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9488_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9488_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9488_RED         0xF800      /* 255,   0,   0 */
#define ILI9488_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9488_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9488_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9488_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9488_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9488_PINK        0xF81F

void lcd_led(bool on);
void lcd_reset(void);
void ILI9488_begin(void);
void ILI9488_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void ILI9488_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void ILI9488_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void ILI9488_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void ILI9488_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void ILI9488_fillScreen(uint16_t color);
void ILI9488_invertDisplay(bool i);
void ILI9488_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void ILI9488_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void ILI9488_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void ILI9488_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void ILI9488_setTextSize(uint8_t s);
uint16_t ILI9488_getTextColor(void);
void ILI9488_setTextColor(uint16_t c);
uint16_t ILI9488_getTextBgColor(void);
void ILI9488_setTextBgColor(uint16_t c);
void ILI9488_setTextWrap(bool w);
void ILI9488_setCursor(int16_t x, int16_t y);
uint16_t ILI9488_getCursorX(void);
uint16_t ILI9488_getCursorY(void);
void ILI9488_draw_value(int64_t value);
void ILI9488_draw_fvalue(float flt, uint8_t afterpoint);
void ILI9488_writeString(uint8_t *s);
void ILI9488_write(uint8_t c);
void ILI9488_getTextBounds(char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
void ILI9488_cp437(bool x);
uint16_t ILI9488_width(void);
uint16_t ILI9488_height(void);
void ILI9488_pushColor(uint16_t color);
void ILI9488_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void ILI9488_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
uint16_t ILI9488_color565(uint8_t r, uint8_t g, uint8_t b);
void ILI9488_setRotation(uint8_t m);
uint16_t ILI9488_getRotation(void);
 
#endif
