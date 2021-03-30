#include "bit_band.h"
#include "config_pin.h"
#include "kedei.h"
#include "glcdfont.h"
#include "stm32f1xx_hal.h"

#define touchscreen 1
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

#ifndef abs
 #define abs(a) ((a) < 0 ? -(a) : (a))
#endif

#ifndef _swap_int16_t
 #define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#define delayms HAL_Delay

bitband_t lcd_dc;// = BITBAND_PERIPH(&GPIOB->ODR, 7);
//bitband_t lcd_rst;// = BITBAND_PERIPH(&GPIOB->ODR, 8);
bitband_t lcd_cs;// = BITBAND_PERIPH(&GPIOB->ODR, 9);
bitband_t tch_cs;// = BITBAND_PERIPH(&GPIOB->ODR, 10);

// static SPI_HandleTypeDef *_hSpi;
static uint16_t lcd_w = lcd_TFTWIDTH;
static uint32_t lcd_h = lcd_TFTHEIGHT;
static uint16_t _rotation = 0;
static uint16_t _cursor_x = 0;
static uint16_t _cursor_y = 0;
static uint16_t _textsize = 1;
static uint16_t _textColor = 0xFFFF;
static uint16_t _textBgColor = 0xFFFF;
static bool _wrap = true;
static bool _cp437 = false;

/* private (static) functions */
void soft_spi_transmit(uint8_t *data, int len)
{
  *(lcd_cs)= 1;
  for(uint8_t i = 0; i<len; i++)
  {
    soft_spi_wr(data[i]);
    // soft_spi_write(data[i]);
  }
  *(lcd_cs)= 0;

  *(tch_cs)= 1;
  *(tch_cs)= 0;
}

void lcd_reset(void)
{
  uint8_t buff[3];
  buff[0] = 0x00;
  buff[1] = 0x01;
  buff[2] = 0x00;
  soft_spi_transmit(&buff[0], sizeof(buff));
  delayms(50);

  buff[0] = 0x00;
  buff[1] = 0x00;
  buff[2] = 0x00;
  soft_spi_transmit(&buff[0], sizeof(buff));
  delayms(100);

  buff[0] = 0x00;
  buff[1] = 0x01;
  buff[2] = 0x00;
  soft_spi_transmit(&buff[0], sizeof(buff));
  delayms(50);
}

void lcd_cmd(uint8_t cmd)
{
  uint8_t buff[3];
  buff[0] = 0x11;
  buff[1] = 0x00;
  buff[2] = cmd;
  soft_spi_transmit(&buff[0], sizeof(buff));
}

void lcd_data(uint8_t dat)
{
  uint8_t buff[3];
  buff[0] = 0x15;
  buff[1] = 0x00;
  buff[2] = dat;
  soft_spi_transmit(&buff[0], sizeof(buff));
}

void lcd_data16(uint16_t color)
{
  uint8_t buff[3];
  buff[0] = 0x15;
  buff[1] = lcd_color_RGB_to_BGR(color)>>8;
  buff[2] = lcd_color_RGB_to_BGR(color)&0xFF;
  soft_spi_transmit(&buff[0], sizeof(buff));
}

void lcd_colorRGB(uint8_t r, uint8_t g, uint8_t b)
{
  uint16_t color =  ((r<<8) & 0xF800) | ((g<<3) & 0x07E0) | ((b>>3) & 0x001F);
  lcd_data16(color);
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t lcd_color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r>>3) & 0x001F) | ((g<<3) & 0x07E0) | ((b<<8) & 0xF800);
}

uint16_t lcd_color_RGB_to_BGR(uint16_t color)
{
  uint8_t r, g, b;
  r = color>>8;
  g = color>>3;
  b = color<<3;
  return ((r>>3) & 0x001F) | ((g<<3) & 0x07E0) | ((b<<8) & 0xF800);
}

void lcd_setframe(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  lcd_cmd(0x2A);
  lcd_data(x>>8); lcd_data(x&0xFF); lcd_data(((w+x)-1)>>8); lcd_data(((w+x)-1)&0xFF);
  lcd_cmd(0x2B);
  lcd_data(y>>8); lcd_data(y&0xFF); lcd_data(((h+y)-1)>>8); lcd_data(((h+y)-1)&0xFF);
  lcd_cmd(0x2C);
}

void drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if((x < 0) ||(x >= lcd_w) || (y < 0) || (y >= lcd_h)) return;
  lcd_setframe(x,y,x+1,y+1);
  lcd_data16(color);
}

void lcd_fillframe(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  int span=h*w;
  int q;
  lcd_setframe(x,y,w,h);
  for(q=0;q<span;q++)
  {
    lcd_data16(color);
  }
}

void lcd_fill(uint16_t color)
{
  lcd_fillframe(0, 0, lcd_w, lcd_h, color);
}

void lcd_fillframeRGB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b)
{
  int span=h*w;
  lcd_setframe(x,y,w,h);
  int q;
  for(q=0;q<span;q++)
  {
    lcd_colorRGB(r, g, b);
  }
}

void lcd_fillRGB(uint8_t r, uint8_t g, uint8_t b)
{
  lcd_fillframeRGB(0, 0, lcd_w, lcd_h, r, g, b);
}

void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
  if((x >= lcd_w) || // Clip right
  (y >= lcd_h) || // Clip bottom
  ((x + 6 * size - 1) < 0) || // Clip left
  ((y + 8 * size - 1) < 0)) // Clip top
  return;
  if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior
  for(int8_t i=0; i<6; i++ )
  {
    uint8_t line;
    if(i < 5) line = pgm_read_byte(font+(c*5)+i);
    else line = 0x0;
    for(int8_t j=0; j<8; j++, line >>= 1)
    {
      if(line & 0x1)
      {
        lcd_fillframe(x+(i*size), y+(j*size), size, size, color);
      }
      else if(bg != color)
      {
        lcd_fillframe(x+i*size, y+j*size, size, size, bg);
      }
    }
  }
}

/* end private (static) functions */
/* public functions */
void lcd_begin(void/*SPI_HandleTypeDef *hSpi*/)
{
  //lcd_reset();
  lcd_cmd(0x01); // software reset
  delayms(50);

  lcd_cmd(0x11); // sleep out
  delayms(150);

  lcd_cmd(0x3A); lcd_data(0x55); // color rgb 565
  lcd_cmd(0xB0); lcd_data(0x00);
  lcd_cmd(0xD0); lcd_data(0x07); lcd_data(0x07); lcd_data(0x1D); lcd_data(0x03);
  lcd_cmd(0x21); // INVON 0x21
  lcd_setrotation(3);
  lcd_cmd(0x29); // Display ON
}

void lcd_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  drawPixel(x0 , y0+r, color);
  drawPixel(x0 , y0-r, color);
  drawPixel(x0+r, y0 , color);
  drawPixel(x0-r, y0 , color);
  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void lcd_drawDotLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t space, uint16_t color)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }
  if (x0 > x1)
  {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }
  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);
  int16_t err = dx / 2;
  int16_t ystep = 0;
  if (y0 < y1)
  {
    ystep = space;
  }
  else
  {
    ystep -= space;
  }
  for (; x0<=x1; x0+=space)
  {
    if (steep)
    {
      drawPixel(y0, x0, color);
    }
    else
    {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void lcd_invertDisplay(bool i)
{
  lcd_cmd(i ? lcd_INVON : lcd_INVOFF);
}

void lcd_setTextSize(uint8_t s)
{
  _textsize = (s > 0) ? s : 1;
}

void lcd_setTextColor(uint16_t c)
{
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  _textColor = c;
}

void lcd_setTextBgColor(uint16_t c)
{
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  _textBgColor = c;
}

void lcd_setTextWrap(bool w)
{
  _wrap = w;
}

void lcd_setCursor(int16_t x, int16_t y)
{
  _cursor_x = x;
  _cursor_y = y;
}

uint16_t lcd_getCursorX(void)
{
  return _cursor_x;
}

uint16_t lcd_getCursorY(void)
{
  return _cursor_y;
}

void lcd_writeString(char *s)
{
  while (*(s)) lcd_write(*s++);
}

void lcd_write(uint8_t c)
{
  if(c == '\n')
  {
    _cursor_y += _textsize*8;
    _cursor_x = 0;
  }
  else if(c == '\r')
  {
    // skip em
  }
  else
  {
    if(_wrap && ((_cursor_x + _textsize * 6) >= lcd_w))
    {
      // Heading off edge?
      _cursor_x = 0; // Reset x to zero
      _cursor_y += _textsize * 8; // Advance y one line
    }
    drawChar(_cursor_x, _cursor_y, c, _textColor, _textBgColor, _textsize);
    _cursor_x += _textsize * 6;
  }
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it. But a TON of code has been written
// with the erroneous character indices. By default, the library uses the
// original 'wrong' behavior and old sketches will still work. Pass 'true'
// to this function to use correct CP437 character values in your code.
void lcd_cp437(bool x)
{
  _cp437 = x;
}

uint16_t kedeilcd_w(void)
{
  return lcd_w;
}

uint16_t kedeilcd_h(void)
{
  return lcd_h;
}

#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH 0x04

void lcd_setrotation(uint8_t m)
{
  lcd_cmd(lcd_MADCTL);
  if(0==m)
  {
    lcd_data(MADCTL_MX | MADCTL_BGR);
    lcd_w = lcd_TFTWIDTH;
    lcd_h = lcd_TFTHEIGHT;
  }
  if(1==m)
  {
    lcd_data(MADCTL_MV | MADCTL_BGR);
    lcd_w = lcd_TFTHEIGHT;
    lcd_h = lcd_TFTWIDTH;
  }
  if(2==m)
  {
    lcd_data(MADCTL_MY | MADCTL_BGR);
    lcd_w = lcd_TFTWIDTH;
    lcd_h = lcd_TFTHEIGHT;
  }
  if(3==m)
  {
    lcd_data(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
    lcd_w = lcd_TFTHEIGHT;
    lcd_h = lcd_TFTWIDTH;
  }
}

uint16_t lcd_getRotation(void)
{
  return _rotation;
}

