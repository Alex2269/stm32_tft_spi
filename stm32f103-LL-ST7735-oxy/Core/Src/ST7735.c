#include "spi_tft.h"

#include "ST7735.h"
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

static uint16_t _width = ST7735_TFTWIDTH;
static uint32_t _height = ST7735_TFTHEIGHT;
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
void lcd_reset(void) {
  ResetPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
  
  LL_mDelay(20);
  SetPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
  LL_mDelay(20);
}

void writeCommand(uint8_t c) {
  ResetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  spiWrite(c);
}

void writeData(uint8_t c) {
  SetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  spiWrite(c);
}

void writeData16(uint16_t c) {
  SetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  spiWrite16(c);
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  writeCommand(ST7735_CASET); // Column addr set
  writeData16(x0);            // XSTART
  writeData16(x1);            // XEND

  writeCommand(ST7735_PASET); // Row addr set
  writeData16(y0);            // YSTART
  writeData16(y1);            // YEND

  writeCommand(ST7735_RAMWR); // write to RAM
}

void drawPixel(int16_t x, int16_t y, uint16_t color) {
  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

  setAddrWindow(x,y,x+1,y+1);
  // writeData16(color);
  writeData16(color & 0b1000010000010000); // One bit per color, upper speed for spi
}

void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      ST7735_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      ST7735_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      ST7735_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      ST7735_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if((x >= _width)            || // Clip right
     (y >= _height)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

  for(int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if(i < 5) line = pgm_read_byte(font+(c*5)+i);
    else      line = 0x0;
    for(int8_t j=0; j<8; j++, line >>= 1) {
      if(line & 0x1) {
        if(size == 1) drawPixel(x+i, y+j, color);
        else          ST7735_fillRect(x+(i*size), y+(j*size), size, size, color);
      } else if(bg != color) {
        if(size == 1) drawPixel(x+i, y+j, bg);
        else          ST7735_fillRect(x+i*size, y+j*size, size, size, bg);
      }
    }
  }
}

/* end private (static) functions */

/* public functions */

void ST7735_begin(void) {
  
  LL_mDelay(1);
  writeCommand(0x11); // Sleep out & booster on
  LL_mDelay(1); // Datasheet says what display wakes about 120ms (may be much faster actually)
  writeCommand(0xb1); // In normal mode (full colors):
  spiWrite(0x05); // RTNA set 1-line period: RTNA2, RTNA0
  spiWrite(0x3c); // Front porch: FPA5,FPA4,FPA3,FPA2
  spiWrite(0x3c); // Back porch: BPA5,BPA4,BPA3,BPA2
  writeCommand(0xb2); // In idle mode (8-colors):
  spiWrite(0x05); // RTNB set 1-line period: RTNAB, RTNB0
  spiWrite(0x3c); // Front porch: FPB5,FPB4,FPB3,FPB2
  spiWrite(0x3c); // Back porch: BPB5,BPB4,BPB3,BPB2
  writeCommand(0xb3); // In partial mode + full colors:

  SetPin(LCD_DC_GPIO_Port, LCD_DC_Pin); // data-command

  spiWrite(0x05); // RTNC set 1-line period: RTNC2, RTNC0
  spiWrite(0x3c); // Front porch: FPC5,FPC4,FPC3,FPC2
  spiWrite(0x3c); // Back porch: BPC5,BPC4,BPC3,BPC2
  spiWrite(0x05); // RTND set 1-line period: RTND2, RTND0
  spiWrite(0x3c); // Front porch: FPD5,FPD4,FPD3,FPD2
  spiWrite(0x3c); // Back porch: BPD5,BPD4,BPD3,BPD2
  writeCommand(0xB4); // Display dot inversion control:
  writeData(0x03); // NLB,NLC
  writeCommand(0x3a); // Interface pixel format
  // writeData(0x03); // 12-bit/pixel RGB 4-4-4 (4k colors)
  writeData(0x05); // 16-bit/pixel RGB 5-6-5 (65k colors)
  // writeData(0x06); // 18-bit/pixel RGB 6-6-6 (256k colors)
  // writeCommand(0x36); // Memory data access control:
  // MY MX MV ML RGB MH - -
  // writeData(0x00); // Normal: Top to Bottom; Left to Right; RGB
  // writeData(0x80); // Y-Mirror: Bottom to top; Left to Right; RGB
  // writeData(0x40); // X-Mirror: Top to Bottom; Right to Left; RGB
  // writeData(0xc0); // X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB
  // writeData(0x20); // X-Y Exchange: X and Y changed positions
  // writeData(0xA0); // X-Y Exchange,Y-Mirror
  // writeData(0x60); // X-Y Exchange,X-Mirror
  // writeData(0xE0); // X-Y Exchange,X-Mirror,Y-Mirror
  writeCommand(0x20); // Display inversion off
  // writeCommand(0x21); // Display inversion on
  writeCommand(0x13); // Partial mode off
  writeCommand(0x26); // Gamma curve set:
  writeData(0x01); // Gamma curve 1 (G2.2) or (G1.0)
  // writeData(0x02); // Gamma curve 2 (G1.8) or (G2.5)
  // writeData(0x04); // Gamma curve 3 (G2.5) or (G2.2)
  // writeData(0x08); // Gamma curve 4 (G1.0) or (G1.8)
  writeCommand(0x29); // Display on
}

void ST7735_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0  , y0+r, color);
  drawPixel(x0  , y0-r, color);
  drawPixel(x0+r, y0  , color);
  drawPixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
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

void ST7735_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  ST7735_drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void ST7735_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void ST7735_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  ST7735_drawFastHLine(x, y, w, color);
  ST7735_drawFastHLine(x, y+h-1, w, color);
  ST7735_drawFastVLine(x, y, h, color);
  ST7735_drawFastVLine(x+w-1, y, h, color);
}

// fill a rectangle
void ST7735_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  setAddrWindow(x, y, x+w-1, y+h-1);

  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      writeData16(color);
    }
  }
  
}

void ST7735_fillScreen(uint16_t color) {
  ST7735_fillRect(0, 0,  _width, _height, color);
}

void ST7735_invertDisplay(bool i) {
  writeCommand(i ? ST7735_INVON : ST7735_INVOFF);
}

void ST7735_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  ST7735_drawFastHLine(x+r  , y    , w-2*r, color); // Top
  ST7735_drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  ST7735_drawFastVLine(x    , y+r  , h-2*r, color); // Left
  ST7735_drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void ST7735_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  ST7735_fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

void ST7735_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  ST7735_drawLine(x0, y0, x1, y1, color);
  ST7735_drawLine(x1, y1, x2, y2, color);
  ST7735_drawLine(x2, y2, x0, y0, color);
}

void ST7735_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
  }
  if (y1 > y2) {
    _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
  }
  if (y0 > y1) {
    _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    ST7735_drawFastHLine(a, y0, b-a+1, color);
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
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) _swap_int16_t(a,b);
    ST7735_drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) _swap_int16_t(a,b);
    ST7735_drawFastHLine(a, y, b-a+1, color);
  }
}

void ST7735_setTextSize(uint8_t s) {
  _textsize = (s > 0) ? s : 1;
}

uint16_t ST7735_getTextColor(void) {
  return _textColor;
}

void ST7735_setTextColor(uint16_t c) {
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  _textColor = c;
}

uint16_t ST7735_getTextBgColor(void) {
  return _textBgColor;
}

void ST7735_setTextBgColor(uint16_t c) {
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  _textBgColor = c;
}

void ST7735_setTextWrap(bool w) {
  _wrap = w;
}

void ST7735_setCursor(int16_t x, int16_t y) {
  _cursor_x = x;
  _cursor_y = y;
}

uint16_t ST7735_getCursorX(void) {
  return _cursor_x;
}

uint16_t ST7735_getCursorY(void) {
  return _cursor_y;
}

void ST7735_draw_value(int64_t value)
{
  uint8_t max_mv[16];
  utoa_local((int64_t)value, max_mv, 10);
  // if(value<10000000) ST7735_write('0');
  // if(value<1000000) ST7735_write('0');
  // if(value<100000) ST7735_write('0');
  // if(value<10000) ST7735_write('0');
  if(value<1000) ST7735_write('0');
  if(value<100) ST7735_write('0');
  if(value<10) ST7735_write('0');
  ST7735_writeString(max_mv);
}

void ST7735_draw_fvalue(float flt, uint8_t afterpoint)
{
  uint8_t res[24];

  if(flt>=0) ST7735_write('+');
  if(flt<0)
  {
    ST7735_write('-');
    flt = fabsf(flt);
  }

  ftoa_local(flt, res, afterpoint);
  ST7735_writeString(res); 
}

void ST7735_writeString(uint8_t *s)
{
    while (*(s)) ST7735_write(*s++);
}

void ST7735_write(uint8_t c) {
  if(c == '\n') {
    _cursor_y += _textsize*8;
    _cursor_x  = 0;
  } else if(c == '\r') {
    // skip em
  } else {
    if(_wrap && ((_cursor_x + _textsize * 6) >= _width)) { // Heading off edge?
      _cursor_x  = 0;            // Reset x to zero
      _cursor_y += _textsize * 8; // Advance y one line
    }
    drawChar(_cursor_x, _cursor_y, c, _textColor, _textBgColor, _textsize);
    _cursor_x += _textsize * 6;
  }
}

// Pass string and a cursor position, returns UL corner and W,H.
void ST7735_getTextBounds(char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
  uint8_t c; // Current character

  *x1 = x;
  *y1 = y;
  *w  = *h = 0;

  uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

  while((c = *str++)) {
    if(c != '\n') { // Not a newline
      if(c != '\r') { // Not a carriage return, is normal char
        if(_wrap && ((x + _textsize * 6) >= _width)) {
          x  = 0;            // Reset x to 0
          y += _textsize * 8; // Advance y by 1 line
          if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
          lineWidth  = _textsize * 6; // First char on new line
        } else { // No line _wrap, just keep incrementing X
          lineWidth += _textsize * 6; // Includes interchar x gap
        }
      } // Carriage return = do nothing
    } else { // Newline
      x  = 0;            // Reset x to 0
      y += _textsize * 8; // Advance y by 1 line
      if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
      lineWidth = 0;     // Reset lineWidth for new line
    }
  }
  // End of string
  if(lineWidth) y += _textsize * 8; // Add height of last (or only) line
  *w = maxWidth - 1;               // Don't include last interchar x gap
  *h = y - *y1;
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void ST7735_cp437(bool x) {
  _cp437 = x;
}

uint16_t ST7735_width(void) {
  return _width;
}

uint16_t ST7735_height(void) {
  return _height;
}

void ST7735_pushColor(uint16_t color) {
  writeData16(color);
}

void ST7735_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;

  if((y+h-1) >= _height)
    h = _height-y;

  setAddrWindow(x, y, x, y+h-1);

  while (h--) {
    writeData16(color);
  }
}

void ST7735_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  while (w--) {
    writeData16(color);
  }
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t ST7735_color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Memory Access Control
#define MADCTL_MY  0x80  // < Bottom to top
#define MADCTL_MX  0x40  // < Right to left
#define MADCTL_MV  0x20  // < Reverse Mode
#define MADCTL_ML  0x10  // < LCD refresh Bottom to top
#define MADCTL_RGB 0x00  // < Red-Green-Blue pixel order
#define MADCTL_BGR 0x08  // < Blue-Green-Red pixel order
#define MADCTL_MH  0x04  // < LCD refresh right to left
#define MADCTL_SWAP_XY_Mirror_Y  0xa0  // < X-Y Exchange,Y-Mirror
#define MADCTL_SWAP_XY_Mirror_X  0x60  // < X-Y Exchange,X-Mirror
#define MADCTL_Mirror_XY  0xc0         // < X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB

void ST7735_setRotation(uint8_t  orientation)
{
  writeCommand(ST7735_MADCTL);
  switch(orientation)
  {
    case 0:
     _width  = ST7735_TFTWIDTH;
     _height = ST7735_TFTHEIGHT;
     writeData( MADCTL_MX |MADCTL_MY | MADCTL_RGB);
    break;
    case 1:
     _width  = ST7735_TFTWIDTH;
     _height = ST7735_TFTHEIGHT;
     writeData(MADCTL_RGB);
    break;
    case 2:
     _width  = ST7735_TFTHEIGHT;
     _height = ST7735_TFTWIDTH;
    writeData(  MADCTL_SWAP_XY_Mirror_Y | MADCTL_RGB);
    break;
    case 3:
     _width = ST7735_TFTHEIGHT;
     _height  = ST7735_TFTWIDTH;
    writeData( MADCTL_SWAP_XY_Mirror_X | MADCTL_RGB);
    break;
  }
}

uint16_t ST7735_getRotation(void) {
  return _rotation;
}

