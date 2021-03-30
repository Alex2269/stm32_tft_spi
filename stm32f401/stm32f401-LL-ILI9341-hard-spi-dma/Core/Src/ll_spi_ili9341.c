#include <stdlib.h>
#include "ll_ili9341_init.h"
#include "ll_spi_ili9341.h"
#include "ll_spi_dma_io.h"
#include "spi_tft.h"
#include "font24.h"
#include "font20.h"
#include "font16.h"
#include "font12.h"
#include "font8.h"

uint16_t dsp_width;
uint16_t dsp_height;

extern uint32_t dma_spi_part;

uint8_t frame_buffer[4096] = {0};

typedef struct
{
  uint16_t TextColor;
  uint16_t BackColor;
  sFONT *pFont;
}

DSP_DrawPropTypeDef;
DSP_DrawPropTypeDef lcdprop;

static inline
void push_color(uint16_t color)
{
  static uint8_t p_color[2];
  p_color[0] = (color>>8);
  p_color[1] = (color & 0xFF);
  dsp_write_data(p_color, 2);
}

void dsp_set_addr_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  // column address set
  static uint8_t addr_x[4];
  static uint8_t addr_y[4];

  addr_x[0] = x1>>8; addr_x[1] = x1;
  addr_x[2] = x2>>8; addr_x[3] = x2;
  addr_y[0] = y1>>8; addr_y[1] = y1;
  addr_y[2] = y2>>8; addr_y[3] = y2;

  dsp_send_command(0x2A); // column address set
  dsp_write_data(addr_x, 4);
  dsp_send_command(0x2B); // row address set
  dsp_write_data(addr_y, 4);
  dsp_send_command(0x2C); // write to RAM
}

void dsp_draw_pixel(int x, int y, uint16_t color)
{
  if((x<0)||(y<0)||(x>=dsp_width)||(y>=dsp_height)) return;
  dsp_set_addr_window(x,y,x,y);
  push_color(color);
}

void dsp_draw_fast_v_line(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  // Rudimentary clipping
  if((x >= dsp_width) || (y >= dsp_height || h < 1)) return;
  if((y + h - 1) >= dsp_height) 
  {
    h = dsp_height - y; 
  }
  if(h < 2 ) 
  {
    dsp_draw_pixel(x, y, color); return; 
  }
  dsp_set_addr_window(x, y, x, y + h - 1);
  // --
  for(uint16_t i = 0;i<h*2;i+=2)
  {
   frame_buffer[i] = color>>8;
   frame_buffer[i+1] = color>>8;
  }
  dsp_write_data(frame_buffer, h*2);
}

void dsp_draw_fast_h_line(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  // Rudimentary clipping
  if((x >= dsp_width) || (y >= dsp_height || w < 1)) return;
  if((x + w - 1) >= dsp_width) 
  {
    w = dsp_width - x; 
  }
  if(w < 2 ) 
  {
    dsp_draw_pixel(x, y, color); return; 
  }
  dsp_set_addr_window(x, y, x + w - 1, y);
  // --
  for(uint16_t i = 0;i<w*2;i+=2)
  {
   frame_buffer[i] = color>>8;
   frame_buffer[i+1] = color>>8;
  }
  dsp_write_data(frame_buffer, w*2);
}

/*
* Draw lines faster by calculating straight sections and drawing them with fastVline and fastHline.
*/
void dsp_draw_line(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color)
{
  if((y0 < 0 && y1 <0) || (y0 > dsp_height && y1 > dsp_height)) return;
  if((x0 < 0 && x1 <0) || (x0 > dsp_width && x1 > dsp_width)) return;
  if(x0 < 0) x0 = 0;
  if(x1 < 0) x1 = 0;
  if(y0 < 0) y0 = 0;
  if(y1 < 0) y1 = 0;
  if(y0 == y1) 
  {
    if(x1 > x0) 
    {
      dsp_draw_fast_h_line(x0, y0, x1 - x0 + 1, color);
    }
    else if(x1 < x0) 
    {
      dsp_draw_fast_h_line(x1, y0, x0 - x1 + 1, color);
    }
    else 
    {
      dsp_draw_pixel(x0, y0, color);
    }
    return;
  }
  else if(x0 == x1) 
  {
    if(y1 > y0) 
    {
      dsp_draw_fast_v_line(x0, y0, y1 - y0 + 1, color);
    }
    else 
    {
      dsp_draw_fast_v_line(x0, y1, y0 - y1 + 1, color);
    }
    return;
  }
  bool steep = abs(y1 - y0) > abs(x1 - x0);
  if(steep) 
  {
    swap(x0, y0);
    swap(x1, y1);
  }
  if(x0 > x1) 
  {
    swap(x0, x1);
    swap(y0, y1);
  }
  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);
  int16_t err = dx / 2;
  int16_t ystep;
  if(y0 < y1) 
  {
    ystep = 1;
  }
  else 
  {
    ystep = -1;
  }
  int16_t xbegin = x0;
  if(steep) 
  {
    for(; x0 <= x1; x0++) 
    {
      err -= dy;
      if(err < 0) 
      {
        int16_t len = x0 - xbegin;
        if(len) 
        {
          dsp_draw_fast_v_line (y0, xbegin, len + 1, color);
          // writeVLine_cont_noCS_noFill(y0, xbegin, len + 1);
        }
        else 
        {
          dsp_draw_pixel(y0, x0, color);
          // dsp_draw_pixel_cont_noCS(y0, x0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if(x0 > xbegin + 1) 
    {
      // writeVLine_cont_noCS_noFill(y0, xbegin, x0 - xbegin);
      dsp_draw_fast_v_line(y0, xbegin, x0 - xbegin, color);
    }
  }
  else 
  {
    for(; x0 <= x1; x0++) 
    {
      err -= dy;
      if(err < 0) 
      {
        int16_t len = x0 - xbegin;
        if(len) 
        {
          dsp_draw_fast_h_line(xbegin, y0, len + 1, color);
          // writeHLine_cont_noCS_noFill(xbegin, y0, len + 1);
        }
        else 
        {
          dsp_draw_pixel(x0, y0, color);
          // dsp_draw_pixel_cont_noCS(x0, y0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if(x0 > xbegin + 1) 
    {
      // writeHLine_cont_noCS_noFill(xbegin, y0, x0 - xbegin);
      dsp_draw_fast_h_line(xbegin, y0, x0 - xbegin, color);
    }
  }
}

void dsp_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  for (uint16_t i = x; i < x + w; i++) {
    dsp_draw_fast_v_line(i, y, h, color);
  }
}

void dsp_fill_screen(uint16_t color)
{
  dsp_fill_rect(0, 0, dsp_width, dsp_height, color);
}

void dsp_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  dsp_draw_fast_h_line(x, y, w, color);
  dsp_draw_fast_h_line(x, y + h - 1, w, color);
  dsp_draw_fast_v_line(x, y, h, color);
  dsp_draw_fast_v_line(x + w - 1, y, h, color);
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      dsp_draw_pixel(x0 + x, y0 + y, color);
      dsp_draw_pixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      dsp_draw_pixel(x0 + x, y0 - y, color);
      dsp_draw_pixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      dsp_draw_pixel(x0 - y, y0 + x, color);
      dsp_draw_pixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      dsp_draw_pixel(x0 - y, y0 - x, color);
      dsp_draw_pixel(x0 - x, y0 - y, color);
    }
  }
}

void dsp_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  dsp_draw_pixel(x0, y0 + r, color);
  dsp_draw_pixel(x0, y0 - r, color);
  dsp_draw_pixel(x0 + r, y0, color);
  dsp_draw_pixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    dsp_draw_pixel(x0 + x, y0 + y, color);
    dsp_draw_pixel(x0 - x, y0 + y, color);
    dsp_draw_pixel(x0 + x, y0 - y, color);
    dsp_draw_pixel(x0 - x, y0 - y, color);
    dsp_draw_pixel(x0 + y, y0 + x, color);
    dsp_draw_pixel(x0 - y, y0 + x, color);
    dsp_draw_pixel(x0 + y, y0 - x, color);
    dsp_draw_pixel(x0 - y, y0 - x, color);
  }
}

void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
        dsp_draw_fast_v_line(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        dsp_draw_fast_v_line(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        dsp_draw_fast_v_line(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        dsp_draw_fast_v_line(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  dsp_draw_fast_v_line(x0, y0 - r, 2 * r + 1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  dsp_draw_fast_h_line(x + r, y, w - 2 * r, color);         // Top
  dsp_draw_fast_h_line(x + r, y + h - 1, w - 2 * r, color); // Bottom
  dsp_draw_fast_v_line(x, y + r, h - 2 * r, color);         // Left
  dsp_draw_fast_v_line(x + w - 1, y + r, h - 2 * r, color); // Right
  // draw four corners
  drawCircleHelper(x + r, y + r, r, 1, color);
  drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  dsp_fill_rect(x + r, y, w - 2 * r, h, color);
  // draw four corners
  fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

void DrawEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
{
    if (rx < 2)
        return;
    if (ry < 2)
        return;
    int16_t x, y;
    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t fx2 = 4 * rx2;
    int32_t fy2 = 4 * ry2;
    int32_t s;

    for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
        dsp_draw_pixel(x0 + x, y0 + y, color);
        dsp_draw_pixel(x0 - x, y0 + y, color);
        dsp_draw_pixel(x0 - x, y0 - y, color);
        dsp_draw_pixel(x0 + x, y0 - y, color);
        if (s >= 0) {
            s += fx2 * (1 - y);
            y--;
        }
        s += ry2 * ((4 * x) + 6);
    }

    for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
        dsp_draw_pixel(x0 + x, y0 + y, color);
        dsp_draw_pixel(x0 - x, y0 + y, color);
        dsp_draw_pixel(x0 - x, y0 - y, color);
        dsp_draw_pixel(x0 + x, y0 - y, color);
        if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
        }
        s += rx2 * ((4 * y) + 6);
    }
}

void FillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
{
    if (rx < 2)
        return;
    if (ry < 2)
        return;
    int16_t x, y;
    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t fx2 = 4 * rx2;
    int32_t fy2 = 4 * ry2;
    int32_t s;

    for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
        dsp_draw_fast_h_line(x0 - x, y0 - y, x + x + 1, color);
        dsp_draw_fast_h_line(x0 - x, y0 + y, x + x + 1, color);

        if (s >= 0) {
            s += fx2 * (1 - y);
            y--;
        }
        s += ry2 * ((4 * x) + 6);
    }

    for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
        dsp_draw_fast_h_line(x0 - x, y0 - y, x + x + 1, color);
        dsp_draw_fast_h_line(x0 - x, y0 + y, x + x + 1, color);

        if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
        }
        s += rx2 * ((4 * y) + 6);
    }
}

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1);
    swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1);
    swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1);
    swap(x0, x1);
  }

  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    dsp_draw_fast_h_line(a, y0, b - a + 1, color);
    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      swap(a, b);
    dsp_draw_fast_h_line(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      swap(a, b);
    dsp_draw_fast_h_line(a, y, b - a + 1, color);
  }
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  dsp_draw_line(x0, y0, x1, y1, color);
  dsp_draw_line(x1, y1, x2, y2, color);
  dsp_draw_line(x2, y2, x0, y0, color);
}

void dsp_set_text_color(uint16_t color)
{
  lcdprop.TextColor=color;
}

void dsp_set_back_color(uint16_t color)
{
  lcdprop.BackColor=color;
}

void dsp_set_font(sFONT *pFonts)
{
  lcdprop.pFont=pFonts;
}

void dsp_draw_char(uint16_t x, uint16_t y, uint8_t c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint16_t y_cur = y;
  uint8_t offset;
  uint8_t *c_t;
  uint8_t *pchar;
  uint32_t line=0;
  height = lcdprop.pFont->Height;
  width = lcdprop.pFont->Width;
  offset = 8 *((width + 7)/8) - width;
  c_t = (uint8_t*) &(lcdprop.pFont->table[(c-' ') * lcdprop.pFont->Height * ((lcdprop.pFont->Width + 7) / 8)]);
  for(i = 0; i < height; i++)
  {
    pchar = ((uint8_t*)c_t + (width + 7)/8 * i);
    switch(((width + 7)/8))
    {
     case 1:
      line = pchar[0];
      break;
     case 2:
      line = (pchar[0]<< 8) | pchar[1];
      break;
     case 3:
      default:
      line = (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];
      break;
    }
    for(j = 0; j < width; j++)
    {
      if(line & (1 << (width- j + offset- 1)))
      {
        frame_buffer[(i*width + j) * 2] = lcdprop.TextColor >> 8;
        frame_buffer[(i*width + j)*2+1] = lcdprop.TextColor & 0xFF;
      }
      else
      {
        frame_buffer[(i*width + j)*2] = lcdprop.BackColor >> 8;
        frame_buffer[(i*width + j)*2+1] = lcdprop.BackColor & 0xFF;
      }
    }
    y_cur++;
  }
  dsp_set_addr_window(x, y, x+width-1, y+height-1);
  // --
  dsp_write_data(frame_buffer,(uint32_t) width * height * 2);
}

void dsp_string(uint16_t x,uint16_t y, char *str)
{
  while(*str)
  {
    dsp_draw_char(x,y,str[0]);
    x+=lcdprop.pFont->Width;
    (void)*str++;
  }
}

void dsp_set_rotation(uint8_t r)
{
  dsp_send_command(0x36);
  switch(r)
  {
   case 0:
    dsp_send_data(0x48);
    dsp_width = 240;
    dsp_height = 320;
    break;
   case 1:
    dsp_send_data(0x28);
    dsp_width = 320;
    dsp_height = 240;
    break;
   case 2:
    dsp_send_data(0x88);
    dsp_width = 240;
    dsp_height = 320;
    break;
   case 3:
    dsp_send_data(0xE8);
    dsp_width = 320;
    dsp_height = 240;
    break;
  }
}

void dsp_fonts_ini(void)
{
  Font8.Height = 8;
  Font8.Width = 5;
  Font12.Height = 12;
  Font12.Width = 7;
  Font16.Height = 16;
  Font16.Width = 11;
  Font20.Height = 20;
  Font20.Width = 14;
  Font24.Height = 24;
  Font24.Width = 17;
  lcdprop.BackColor=DSP_BLACK;
  lcdprop.TextColor=DSP_GREEN;
  lcdprop.pFont=&Font16;
}
