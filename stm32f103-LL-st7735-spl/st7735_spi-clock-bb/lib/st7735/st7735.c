#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include "delay.h"
#include "port_io.h"
#include "st7735.h"
#include "font5x7.h"
#include "font7x11.h"
#include "tft_font.h"
#include "DefaultFonts.h"

uint16_t scr_width;
uint16_t scr_height;

void *
mmcpy(void *dest, const void *src, uint8_t n)
{
  char* dst8 = (char*)dest;
  char* src8 = (char*)src;
  while (n--)
  {
    *dst8++ = *src8++;
  }
  return dest;
}

void ST7735_cmd(uint8_t cmd)
{
  GPIOB_LO = 1<<DC; // data-command
  soft_spi_write(cmd);
  GPIOB_HI = 1<<DC; // data-command
}

void ST7735_data(uint8_t data)
{
  GPIOB_HI = 1<<DC; // data-command
  soft_spi_write(data);
  GPIOB_LO = 1<<DC; // data-command
}

uint16_t RGB565(uint8_t R,uint8_t G,uint8_t B)
{
  return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
}

void ST7735_Init(void)
{
  PINS_ini();

  // Reset display
  GPIOB_HI = 1<<RST;
  Delay_ms(1);
  GPIOB_LO = 1<<RST;
  Delay_ms(1);
  GPIOB_HI = 1<<RST;

  Delay_ms(1);
  ST7735_cmd(0x11); // Sleep out & booster on
  Delay_ms(1); // Datasheet says what display wakes about 120ms (may be much faster actually)
  ST7735_cmd(0xb1); // In normal mode (full colors):
  soft_spi_write(0x05); // RTNA set 1-line period: RTNA2, RTNA0
  soft_spi_write(0x3c); // Front porch: FPA5,FPA4,FPA3,FPA2
  soft_spi_write(0x3c); // Back porch: BPA5,BPA4,BPA3,BPA2
  ST7735_cmd(0xb2); // In idle mode (8-colors):
  soft_spi_write(0x05); // RTNB set 1-line period: RTNAB, RTNB0
  soft_spi_write(0x3c); // Front porch: FPB5,FPB4,FPB3,FPB2
  soft_spi_write(0x3c); // Back porch: BPB5,BPB4,BPB3,BPB2
  ST7735_cmd(0xb3); // In partial mode + full colors:
  GPIOB_HI = 1<<DC; // data-command
  soft_spi_write(0x05); // RTNC set 1-line period: RTNC2, RTNC0
  soft_spi_write(0x3c); // Front porch: FPC5,FPC4,FPC3,FPC2
  soft_spi_write(0x3c); // Back porch: BPC5,BPC4,BPC3,BPC2
  soft_spi_write(0x05); // RTND set 1-line period: RTND2, RTND0
  soft_spi_write(0x3c); // Front porch: FPD5,FPD4,FPD3,FPD2
  soft_spi_write(0x3c); // Back porch: BPD5,BPD4,BPD3,BPD2
  ST7735_cmd(0xB4); // Display dot inversion control:
  ST7735_data(0x03); // NLB,NLC
  ST7735_cmd(0x3a); // Interface pixel format
  // ST7735_data(0x03); // 12-bit/pixel RGB 4-4-4 (4k colors)
  ST7735_data(0x05); // 16-bit/pixel RGB 5-6-5 (65k colors)
  // ST7735_data(0x06); // 18-bit/pixel RGB 6-6-6 (256k colors)
  // ST7735_cmd(0x36); // Memory data access control:
  // MY MX MV ML RGB MH - -
  // ST7735_data(0x00); // Normal: Top to Bottom; Left to Right; RGB
  // ST7735_data(0x80); // Y-Mirror: Bottom to top; Left to Right; RGB
  // ST7735_data(0x40); // X-Mirror: Top to Bottom; Right to Left; RGB
  // ST7735_data(0xc0); // X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB
  // ST7735_data(0x20); // X-Y Exchange: X and Y changed positions
  // ST7735_data(0xA0); // X-Y Exchange,Y-Mirror
  // ST7735_data(0x60); // X-Y Exchange,X-Mirror
  // ST7735_data(0xE0); // X-Y Exchange,X-Mirror,Y-Mirror
  ST7735_cmd(0x20); // Display inversion off
  // ST7735_cmd(0x21); // Display inversion on
  ST7735_cmd(0x13); // Partial mode off
  ST7735_cmd(0x26); // Gamma curve set:
  ST7735_data(0x01); // Gamma curve 1 (G2.2) or (G1.0)
  // ST7735_data(0x02); // Gamma curve 2 (G1.8) or (G2.5)
  // ST7735_data(0x04); // Gamma curve 3 (G2.5) or (G2.2)
  // ST7735_data(0x08); // Gamma curve 4 (G1.0) or (G1.8)
  ST7735_cmd(0x29); // Display on
  ST7735_Orientation(scr_normal);
}

void ST7735_Orientation(ScrOrientation_TypeDef orientation)
{
  ST7735_cmd(0x36); // Memory data access control:
  switch(orientation)
  {
    case scr_CW:
    scr_width = scr_h;
    scr_height = scr_w;
    ST7735_data(0xA0); // X-Y Exchange,Y-Mirror
    break;
    case scr_CCW:
    scr_width = scr_h;
    scr_height = scr_w;
    ST7735_data(0x60); // X-Y Exchange,X-Mirror
    break;
    case scr_180:
    scr_width = scr_w;
    scr_height = scr_h;
    ST7735_data(0xc0); // X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB
    break;
    default:
    scr_width = scr_w;
    scr_height = scr_h;
    ST7735_data(0x00); // Normal: Top to Bottom; Left to Right; RGB
    break;
  }
}

void ST7735_AddrSet(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  ST7735_cmd(0x2a); // Column address set
  soft_spi_write((uint8_t)x1 >> 8);
  soft_spi_write((uint8_t)x1);
  soft_spi_write((uint8_t)x2 >> 8);
  soft_spi_write((uint8_t)x2);
  ST7735_cmd(0x2b); // Row address set
  soft_spi_write((uint8_t)y1 >> 8);
  soft_spi_write((uint8_t)y1);
  soft_spi_write((uint8_t)y2 >> 8);
  soft_spi_write((uint8_t)y2);
  ST7735_cmd(0x2c); // Memory write
}

void ST7735_Clear(uint16_t color)
{
  uint16_t i;
  ST7735_AddrSet(0,0,scr_width - 1,scr_height - 1);
  for (i = 0; i < scr_width * scr_height; i++)
  {
    soft_spi_write(color >> 8);
    soft_spi_write((uint8_t)color);
  }
}

void ST7735_Pixel(uint16_t X, uint16_t Y, uint16_t color)
{
  ST7735_AddrSet(X,Y,X,Y);
  soft_spi_write(color >> 8);
  soft_spi_write((uint8_t)color);
}

void ST7735_HLine(uint16_t X1, uint16_t X2, uint16_t Y, uint16_t color)
{
  uint16_t i;
  ST7735_AddrSet(X1,Y,X2,Y);
  for (i = 0; i <= (X2 - X1); i++)
  {
    soft_spi_write(color >> 8);
    soft_spi_write((uint8_t)color);
  }
}

void ST7735_VLine(uint16_t X, uint16_t Y1, uint16_t Y2, uint16_t color)
{
  uint16_t i;
  ST7735_AddrSet(X,Y1,X,Y2);
  for (i = 0; i <= (Y2 - Y1); i++)
  {
    soft_spi_write(color >> 8);
    soft_spi_write((uint8_t)color);
  }
}

void ST7735_Line(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t color)
{
  int16_t dX = X2-X1;
  int16_t dY = Y2-Y1;
  int16_t dXsym = (dX > 0) ? 1 : -1;
  int16_t dYsym = (dY > 0) ? 1 : -1;
  if (dX == 0)
  {
    if (Y2>Y1) ST7735_VLine(X1,Y1,Y2,color); else ST7735_VLine(X1,Y2,Y1,color);
    return;
  }
  if (dY == 0)
  {
    if (X2>X1) ST7735_HLine(X1,X2,Y1,color); else ST7735_HLine(X2,X1,Y1,color);
    return;
  }
  dX *= dXsym;
  dY *= dYsym;
  int16_t dX2 = dX << 1;
  int16_t dY2 = dY << 1;
  int16_t di;
  if (dX >= dY)
  {
    di = dY2 - dX;
    while (X1 != X2)
    {
      ST7735_Pixel(X1,Y1,color);
      X1 += dXsym;
      if (di < 0)
      {
        di += dY2;
      }
      else
      {
        di += dY2 - dX2;
        Y1 += dYsym;
      }
    }
  }
  else
  {
    di = dX2 - dY;
    while (Y1 != Y2)
    {
      ST7735_Pixel(X1,Y1,color);
      Y1 += dYsym;
      if (di < 0)
      {
        di += dX2;
      }
      else
      {
        di += dX2 - dY2;
        X1 += dXsym;
      }
    }
  }
  ST7735_Pixel(X1,Y1,color);
}

void ST7735_Rect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color)
{
  ST7735_HLine(X1,X2,Y1,color);
  ST7735_HLine(X1,X2,Y2,color);
  ST7735_VLine(X1,Y1,Y2,color);
  ST7735_VLine(X2,Y1,Y2,color);
}

void ST7735_FillRect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color)
{
  uint16_t i;
  uint16_t FS = (X2 - X1 + 1) * (Y2 - Y1 + 1);
  ST7735_AddrSet(X1,Y1,X2,Y2);
  for (i = 0; i < FS; i++)
  {
    soft_spi_write(color >> 8);
    soft_spi_write((uint8_t)color);
  }
}

void ST7735_PutChar5x7(uint16_t X, uint16_t Y, uint8_t chr, uint16_t color)
{
  uint16_t i,j;
  uint8_t buffer[5];
  mmcpy(buffer,&Font5x7[(chr - 32) * 5],5);
  ST7735_AddrSet(X,Y,X + 4,Y + 6);

  for (j = 0; j < 7; j++)
  {
    for (i = 0; i < 5; i++)
    {
      if ((buffer[i] >> j) & 0x01)
      {
        soft_spi_write(color >> 8);
        soft_spi_write((uint8_t)color);
      }
      else
      {
        soft_spi_write(0x00);
        soft_spi_write(0x00);
      }
    }
  }
}

void ST7735_PutStr5x7(uint8_t X, uint8_t Y, char *str, uint16_t color)
{
  while (*str)
  {
    ST7735_PutChar5x7(X,Y,*str++,color);
    if (X < scr_width - 6)
    {
      X += 6;
    }
    else if (Y < scr_height - 8)
    {
      X = 0; Y += 8;
    }
    else
    {
      X = 0; Y = 0;
    }
  };
}

void ST7735_PutChar_asc2_1608(uint16_t X, uint16_t Y, uint8_t chr, uint16_t color)
{
  uint16_t i,j;
  uint8_t buffer[16];
  mmcpy(buffer,&asc2_1608[(chr - 32) * 16],16);
  ST7735_AddrSet(X,Y,X + 16-1,Y + 8+2);

  //for (i = 0; i < 8; i++)
  for (i = 8; i > 0; i--)
  {
    for (j = 0; j < 16; j++)
    {
      //if ((buffer[j] >> i) & 0x01)
      if ((buffer[j] >> (i % 8)) & 0x01)
      {
       soft_spi_write(color >> 8);
       soft_spi_write((uint8_t)color);
      }
      else
      {
        soft_spi_write(0x00);
        soft_spi_write(0x00);
      }
    }
  }
}

void ST7735_PutStr_asc2_1608(uint8_t X, uint8_t Y, char *str, uint16_t color)
{
  while (*str)
  {
    ST7735_PutChar_asc2_1608(X,Y,*str++,color);
    if (X < scr_width - 16)
    {
      X += 16;
    }
    else if (Y < scr_height - 16)
    {
      X = 0; Y += 16;
    }
    else
    {
      X = 0; Y = 0;
    }
  };
}

void ST7735_PutChar_SmallFont(uint16_t X, uint16_t Y, uint8_t chr, uint16_t color)
{
  uint16_t i,j;
  uint8_t buffer[12];
  mmcpy(buffer,&SmallFont[(chr - 32) * 12+6],12);
  ST7735_AddrSet(X,Y,X + 12-1,Y + 8);

  for (i = 0; i < 8; i++)
  {
    for (j = 12; j > 0; j--)
    {
      if ((buffer[i] >> j) & 0x01)
      {
        soft_spi_write(color >> 8);
        soft_spi_write((uint8_t)color);
      }
      else
      {
        soft_spi_write(0x00);
        soft_spi_write(0x00);
      }
    }
  }
}

void ST7735_PutStr_SmallFont(uint8_t X, uint8_t Y, char *str, uint16_t color)
{
  while (*str)
  {
    ST7735_PutChar_SmallFont(X,Y,*str++,color);
    if (X < scr_width - 8+2)
    {
      X += 8+2;
    }
    else if (Y < scr_height - 12)
    {
      X = 0; Y += 12;
    }
    else
    {
      X = 0; Y = 0;
    }
  };
}
