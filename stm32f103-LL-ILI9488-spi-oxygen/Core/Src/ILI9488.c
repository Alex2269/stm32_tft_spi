#include "spi_tft.h"
#include "ILI9488.h"
#include "glcdfont.h"
#include "ftoa_impl.h"
#include "main.h"
#include "math.h"
#include "stm32f1xx_ll_gpio.h"

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

#ifndef abs
  #define abs(a) ((a) < 0 ? -(a) : (a))
#endif

#ifndef _swap_int16_t
  #define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

static uint16_t _width = ILI9488_TFTWIDTH;
static uint32_t _height = ILI9488_TFTHEIGHT;
static uint16_t _rotation = 0;
static uint16_t _cursor_x = 0;
static uint16_t _cursor_y = 0;
static uint16_t _textsize = 1;
static uint16_t _textColor = 0xFFFF;
static uint16_t _textBgColor = 0xFFFF;
static bool _wrap = true;
static bool _cp437 = false;

static void writeCommand(uint8_t c);
static void writeData(uint8_t c);
static void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void drawPixel(int16_t x, int16_t y, uint16_t color);
static void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
static void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
static void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

/* private (static) functions */
void lcd_reset(void) 
{
  ResetPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
  LL_mDelay(20);
  SetPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
  LL_mDelay(20);
}

void writeCommand(uint8_t c) 
{
  ResetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
  ResetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  spiWrite(c);
  SetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
}

void writeData(uint8_t c) 
{
  ResetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
  SetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  spiWrite(c);
  SetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
}

void writeData16(uint16_t c) 
{
  ResetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
  SetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  spiWrite16(c);
  SetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
}

static inline void writeColor16(uint16_t color)
{
  writeData((color & 0xF800) >> 8); // red
  writeData((color & 0x07E0) >> 3); // green
  writeData((color & 0x001F) << 3); // blue
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  writeCommand(0x2a);
  writeData16(x0);
  writeData16(x1);
  writeCommand(0x2b);
  writeData16(y0);
  writeData16(y1);
  writeCommand(0x2c);
}

void drawPixel(int16_t x, int16_t y, uint16_t color) 
{
  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
  setAddrWindow(x,y,x+1,y+1);
  // writeColor16(color);
  writeColor16(color & 0b1000010000010000); // One bit per color, upper speed for spi
}

void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) 
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

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
    if (cornername & 0x1) 
    {
      ILI9488_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      ILI9488_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) 
    {
      ILI9488_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      ILI9488_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) 
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

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
    if (cornername & 0x4) 
    {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) 
    {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) 
    {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) 
    {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) 
{
  if((x >= _width) || // Clip right
  (y >= _height) || // Clip bottom
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
        if(size == 1) drawPixel(x+i, y+j, color);
        else ILI9488_fillRect(x+(i*size), y+(j*size), size, size, color);
      }
      else if(bg != color) 
      {
        if(size == 1) drawPixel(x+i, y+j, bg);
        else ILI9488_fillRect(x+i*size, y+j*size, size, size, bg);
      }
    }
  }
}

/* end private (static) functions */
/* public functions */
void ILI9488_begin(void) 
{
  LL_mDelay(20);
  writeCommand(0x01);// Software Reset
  LL_mDelay(10);

  writeCommand(0xE0);
  writeData(0x00);
  writeData(0x03);
  writeData(0x09);
  writeData(0x08);
  writeData(0x16);
  writeData(0x0A);
  writeData(0x3F);
  writeData(0x78);
  writeData(0x4C);
  writeData(0x09);
  writeData(0x0A);
  writeData(0x08);
  writeData(0x16);
  writeData(0x1A);
  writeData(0x0F);

  writeCommand(0XE1);
  writeData(0x00);
  writeData(0x16);
  writeData(0x19);
  writeData(0x03);
  writeData(0x0F);
  writeData(0x05);
  writeData(0x32);
  writeData(0x45);
  writeData(0x46);
  writeData(0x04);
  writeData(0x0E);
  writeData(0x0D);
  writeData(0x35);
  writeData(0x37);
  writeData(0x0F);

  writeCommand(0XC0); // Power Control 1
  writeData(0x17); // Vreg1out
  writeData(0x15); // Verg2out

  writeCommand(0xC1); // Power Control 2
  writeData(0x41); // VGH,VGL

  writeCommand(0xC5); // Power Control 3
  writeData(0x00);
  writeData(0x12); // Vcom
  writeData(0x80);

  writeCommand(0x36); // Memory Access
  writeData(0x48);
  writeCommand(0x3A); // Interface Pixel Format
  writeData(0x66);   // 18 bit
  writeCommand(0XB0); // Interface Mode Control
  writeData(0x80);   // SDO NOT USE
  writeCommand(0xB1); // Frame rate
  writeData(0xA0); // 60Hz
  writeCommand(0xB4); // Display Inversion Control
  writeData(0x02); // 2-dot
  writeCommand(0XB6); // Display Function Control RGB/MCU Interface Control
  writeData(0x02); // MCU
  writeData(0x02); // Source,Gate scan dieection
  writeCommand(0XE9); // Set Image Functio
  writeData(0x00); // Disable 24 bit data
  writeCommand(0xF7); // Adjust Control
  writeData(0xA9);
  writeData(0x51);
  writeData(0x2C);
  writeData(0x82); // D7 stream, loose
  writeCommand(0x11); // Exit Sleep
  LL_mDelay(120);
  writeCommand(0x29); // Display on
}

void ILI9488_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) 
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

void ILI9488_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) 
{
  ILI9488_drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void ILI9488_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) 
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
  int16_t ystep;

  if (y0 < y1) 
  {
    ystep = 1;
  }
  else 
  {
    ystep = -1;
  }

  for (; x0<=x1; x0++) 
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

void ILI9488_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
{
  ILI9488_drawFastHLine(x, y, w, color);
  ILI9488_drawFastHLine(x, y+h-1, w, color);
  ILI9488_drawFastVLine(x, y, h, color);
  ILI9488_drawFastVLine(x+w-1, y, h, color);
}

// fill a rectangle
void ILI9488_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
{
  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;

  if((x + w - 1) >= _width) w = _width - x;
  if((y + h - 1) >= _height) h = _height - y;

  setAddrWindow(x, y, x+w-1, y+h-1);

  for(y=h; y>0; y--) 
  {
    for(x=w; x>0; x--) 
    {
      writeColor16(color);
    }
  }
}

void ILI9488_fillScreen(uint16_t color) 
{
  ILI9488_fillRect(0, 0, _width, _height, color);
}

void ILI9488_invertDisplay(bool i) 
{
  writeCommand(i ? ILI9488_INVON : ILI9488_INVOFF);
}

void ILI9488_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) 
{
  // smarter version
  ILI9488_drawFastHLine(x+r , y , w-2*r, color); // Top
  ILI9488_drawFastHLine(x+r , y+h-1, w-2*r, color); // Bottom
  ILI9488_drawFastVLine(x , y+r , h-2*r, color); // Left
  ILI9488_drawFastVLine(x+w-1, y+r , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r , y+r , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r , y+h-r-1, r, 8, color);
}

void ILI9488_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) 
{
  // smarter version
  ILI9488_fillRect(x+r, y, w-2*r, h, color);
  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r , y+r, r, 2, h-2*r-1, color);
}

void ILI9488_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) 
{
  ILI9488_drawLine(x0, y0, x1, y1, color);
  ILI9488_drawLine(x1, y1, x2, y2, color);
  ILI9488_drawLine(x2, y2, x0, y0, color);
}

void ILI9488_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) 
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) 
  {
    _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
  }
  if (y1 > y2) 
  {
    _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
  }
  if (y0 > y1) 
  {
    _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
  }
  if(y0 == y2) 
  {
    // Handle awkward all-on-same-linecase as its own thing
    a = b = x0;
    if(x1 < a) a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a) a = x2;
    else if(x2 > b) b = x2;
    ILI9488_drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
  dx01 = x1 - x0,
  dy01 = y1 - y0,
  dx02 = x2 - x0,
  dy02 = y2 - y0,
  dx12 = x2 - x1,
  dy12 = y2 - y1;

  int32_t
  sa = 0,
  sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2. If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).

  if(y1 == y2) last = y1; // Include y1 scanline
  else last = y1-1; // Skip it

  for(y=y0; y<=last; y++) 
  {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */

    if(a > b) _swap_int16_t(a,b);
    ILI9488_drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2. This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);

  for(; y<=y2; y++) 
  {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */

    if(a > b) _swap_int16_t(a,b);
    ILI9488_drawFastHLine(a, y, b-a+1, color);
  }
}

void ILI9488_setTextSize(uint8_t s) 
{
  _textsize = (s > 0) ? s : 1;
}

uint16_t ILI9488_getTextColor(void) 
{
  return _textColor;
}

void ILI9488_setTextColor(uint16_t c) 
{
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  _textColor = c;
}

uint16_t ILI9488_getTextBgColor(void) 
{
  return _textBgColor;
}

void ILI9488_setTextBgColor(uint16_t c) 
{
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  _textBgColor = c;
}

void ILI9488_setTextWrap(bool w) 
{
  _wrap = w;
}

void ILI9488_setCursor(int16_t x, int16_t y) 
{
  _cursor_x = x;
  _cursor_y = y;
}

uint16_t ILI9488_getCursorX(void) 
{
  return _cursor_x;
}

uint16_t ILI9488_getCursorY(void) 
{
  return _cursor_y;
}

void ILI9488_draw_value(int64_t value)
{
  uint8_t max_mv[16];
  utoa_local((int64_t)value, max_mv, 10);
  // if(value<10000000) ILI9488_write('0');
  // if(value<1000000) ILI9488_write('0');
  // if(value<100000) ILI9488_write('0');
  // if(value<10000) ILI9488_write('0');
  if(value<1000) ILI9488_write('0');
  if(value<100) ILI9488_write('0');
  if(value<10) ILI9488_write('0');
  ILI9488_writeString(max_mv);
}

void ILI9488_draw_fvalue(float flt, uint8_t afterpoint)
{
  uint8_t res[24];
  if(flt>=0) ILI9488_write('+');
  if(flt<0)
  {
    ILI9488_write('-');
    flt = fabsf(flt);
  }
  ftoa_local(flt, res, afterpoint);
  ILI9488_writeString(res);
}

void ILI9488_writeString(uint8_t *s)
{
  while (*(s)) ILI9488_write(*s++);
}

void ILI9488_write(uint8_t c) 
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
    if(_wrap && ((_cursor_x + _textsize * 6) >= _width)) 
    {
      // Heading off edge?
      _cursor_x = 0; // Reset x to zero
      _cursor_y += _textsize * 8; // Advance y one line
    }
    drawChar(_cursor_x, _cursor_y, c, _textColor, _textBgColor, _textsize);
    _cursor_x += _textsize * 6;
  }
}

// Pass string and a cursor position, returns UL corner and W,H.
void ILI9488_getTextBounds(char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) 
{
  uint8_t c; // Current character
  *x1 = x;
  *y1 = y;
  *w = *h = 0;
  uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

  while((c = *str++)) 
  {
    if(c != '\n') 
    {
      // Not a newline
      if(c != '\r') 
      {
        // Not a carriage return, is normal char
        if(_wrap && ((x + _textsize * 6) >= _width)) 
        {
          x = 0; // Reset x to 0
          y += _textsize * 8; // Advance y by 1 line
          if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
          lineWidth = _textsize * 6; // First char on new line
        }
        else 
        {
          // No line _wrap, just keep incrementing X
          lineWidth += _textsize * 6; // Includes interchar x gap
        }
      }
      // Carriage return = do nothing
    }
    else 
    {
      // Newline
      x = 0; // Reset x to 0
      y += _textsize * 8; // Advance y by 1 line
      if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
      lineWidth = 0; // Reset lineWidth for new line
    }
  }
  // End of string
  if(lineWidth) y += _textsize * 8; // Add height of last (or only) line
  *w = maxWidth - 1; // Don't include last interchar x gap
  *h = y - *y1;
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it. But a TON of code has been written
// with the erroneous character indices. By default, the library uses the
// original 'wrong' behavior and old sketches will still work. Pass 'true'
// to this function to use correct CP437 character values in your code.
void ILI9488_cp437(bool x) 
{
  _cp437 = x;
}

uint16_t ILI9488_width(void) 
{
  return _width;
}

uint16_t ILI9488_height(void) 
{
  return _height;
}

void ILI9488_pushColor(uint16_t color) 
{
  writeColor16(color);
}

void ILI9488_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) 
{
  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((y+h-1) >= _height) h = _height-y;
  setAddrWindow(x, y, x, y+h-1);

  while (h--) 
  {
    writeColor16(color);
  }
}

void ILI9488_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) 
{
  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width) w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  while (w--) 
  {
    writeColor16(color);
  }
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t ILI9488_color565(uint8_t r, uint8_t g, uint8_t b) 
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void ILI9488_setRotation(uint8_t r)
{
  writeCommand(0x36);
  switch(r)
  {
   case 0:
    writeData(0x40|0x08);
    _width = ILI9488_TFTWIDTH;
    _height = ILI9488_TFTHEIGHT;
    break;
   case 1:
    writeData(0x20|0x08);
    _width = ILI9488_TFTHEIGHT;
    _height = ILI9488_TFTWIDTH;
    break;
   case 2:
    writeData(0x80|0x08);
    _width = ILI9488_TFTWIDTH;
    _height = ILI9488_TFTHEIGHT;
    break;
   case 3:
    writeData(0x40|0x80|0x20|0x08);
    _width = ILI9488_TFTHEIGHT;
    _height = ILI9488_TFTWIDTH;
    break;
  }
}

uint16_t ILI9488_getRotation(void) 
{
  return _rotation;
}

