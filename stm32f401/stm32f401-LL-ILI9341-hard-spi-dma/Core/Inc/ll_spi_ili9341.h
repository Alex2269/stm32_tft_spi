#ifndef __LL_SPI_ILI9341_H
#define __LL_SPI_ILI9341_H
//-------------------------------------------------------------------
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_dma.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "ll_ili9341_init.h"
#include "fonts.h"
//-------------------------------------------------------------------

#include "main.h"

//-------------------------------------------------------------------
#define swap(a,b) {int16_t t=a;a=b;b=t;}
#define _swap_int16_t(a,b) {int16_t t=a;a=b;b=t;}
//-------------------------------------------------------------------

// static inline
// void push_color(uint16_t color);
void dsp_set_addr_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void dsp_draw_pixel(int x, int y, uint16_t color);
void dsp_draw_fast_v_line(int16_t x, int16_t y, int16_t h, uint16_t color);
void dsp_draw_fast_h_line(int16_t x, int16_t y, int16_t w, uint16_t color);
void dsp_draw_line(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color);
// void dsp_fill_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
void dsp_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void dsp_fill_screen(uint16_t color);
// void dsp_draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void dsp_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
//void dsp_draw_circle(uint16_t x0, uint16_t y0, int r, uint16_t color);
void dsp_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void DrawEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color);
void FillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color);
void dsp_set_text_color(uint16_t color);
void dsp_set_back_color(uint16_t color);
void dsp_set_font(sFONT *pFonts);
void dsp_draw_char(uint16_t x, uint16_t y, uint8_t c);
void dsp_string(uint16_t x,uint16_t y, char *str);
void dsp_set_rotation(uint8_t r);
void dsp_fonts_ini(void);

//-------------------------------------------------------------------
#endif /* __LL_SPI_ILI9341_H */


