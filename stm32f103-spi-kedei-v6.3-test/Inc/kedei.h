#ifndef lcd_H
#define lcd_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pins_ext.h"
#include "soft_spi.h"

#include "glcdfont.h"
#include "stm32f1xx_hal.h"

#define lcd_TFTWIDTH  320
#define lcd_TFTHEIGHT 480

#define lcd_NOP     0x00
#define lcd_SWRESET 0x01
#define lcd_RDDID   0x04
#define lcd_RDDST   0x09

#define lcd_SLPIN   0x10
#define lcd_SLPOUT  0x11
#define lcd_PTLON   0x12
#define lcd_NORON   0x13

#define lcd_RDMODE  0x0A
#define lcd_RDMADCTL  0x0B
#define lcd_RDPIXFMT  0x0C
#define lcd_RDIMGFMT  0x0D
#define lcd_RDSELFDIAG  0x0F

#define lcd_INVOFF  0x20
#define lcd_INVON   0x21
#define lcd_GAMMASET 0x26
#define lcd_DISPOFF 0x28
#define lcd_DISPON  0x29

#define lcd_CASET   0x2A
#define lcd_PASET   0x2B
#define lcd_RAMWR   0x2C
#define lcd_RAMRD   0x2E

#define lcd_PTLAR   0x30
#define lcd_MADCTL  0x36
#define lcd_PIXFMT  0x3A

#define lcd_FRMCTR1 0xB1
#define lcd_FRMCTR2 0xB2
#define lcd_FRMCTR3 0xB3
#define lcd_INVCTR  0xB4
#define lcd_DFUNCTR 0xB6

#define lcd_PWCTR1  0xC0
#define lcd_PWCTR2  0xC1
#define lcd_PWCTR3  0xC2
#define lcd_PWCTR4  0xC3
#define lcd_PWCTR5  0xC4
#define lcd_VMCTR1  0xC5
#define lcd_VMCTR2  0xC7

#define lcd_RDID1   0xDA
#define lcd_RDID2   0xDB
#define lcd_RDID3   0xDC
#define lcd_RDID4   0xDD

#define lcd_GMCTRP1 0xE0
#define lcd_GMCTRN1 0xE1
/*
#define lcd_PWCTR6  0xFC

*/

// Color definitions
#define BLACK       0x0000      /*   0,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define DARKGREY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define GREEN       0x07E0      /*   0, 255,   0 */
#define CYAN        0x07FF      /*   0, 255, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define PINK        0xF81F

#define LCD_CS_Pin          CS_Pin
#define LCD_CS_GPIO_Port    CS_GPIO_Port

void soft_spi_transmit(uint8_t *data, int len);
void lcd_reset(void);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t dat);
void lcd_data16(uint16_t color);
void lcd_colorRGB(uint8_t r, uint8_t g, uint8_t b);
uint16_t lcd_color565(uint8_t r, uint8_t g, uint8_t b);
uint16_t lcd_color_RGB_to_BGR(uint16_t color);
void lcd_setframe(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void drawPixel(int16_t x, int16_t y, uint16_t color);
void lcd_fillframe(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void lcd_fill(uint16_t color);
void lcd_fillframeRGB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b);
void lcd_fillRGB(uint8_t r, uint8_t g, uint8_t b);
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void lcd_begin(void/*SPI_HandleTypeDef *hSpi*/);
void lcd_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void lcd_drawDotLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t space, uint16_t color);
void lcd_invertDisplay(bool i);
void lcd_setTextSize(uint8_t s);
void lcd_setTextColor(uint16_t c);
void lcd_setTextBgColor(uint16_t c);
void lcd_setTextWrap(bool w);
void lcd_setCursor(int16_t x, int16_t y);
uint16_t lcd_getCursorX(void);
uint16_t lcd_getCursorY(void);
void lcd_writeString(char *s);
void lcd_write(uint8_t c);
void lcd_cp437(bool x);
uint16_t kedeilcd_w(void);
uint16_t kedeilcd_h(void);
void lcd_setrotation(uint8_t m);
uint16_t lcd_getRotation(void);

#endif
