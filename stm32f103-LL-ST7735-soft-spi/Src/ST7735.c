#include "ST7735.h"
#include "main.h"
#include "soft_spi.h"
#include "stm32f1xx_ll_gpio.h"

#include "font24.c"
#include "font20.c"
#include "font16.c"
#include "font12.c"
#include "font8.c"

uint16_t ST7735_WIDTH;
uint16_t ST7735_HEIGHT;

typedef struct
{
  uint16_t TextColor;
  uint16_t BackColor;
  sFONT *pFont;
} LCD_DrawPropTypeDef;

LCD_DrawPropTypeDef lcdprop;

void ST7735_SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  uint16_t data[3];
  writeCommand(0x2A);  // CASET
  data[0] = x0;
  data[1] = x1;
  data[2] = '\0'; // NULL-terminated
  write_data_array16(data); // X START-END

  writeCommand(0x2B);  // RASET
  data[0] = y0;
  data[1] = y1;
  data[2] = '\0'; // NULL-terminated
  write_data_array16(data);
  writeCommand(0x2C);  // RAMWR
}

void ST7735_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
   // rudimentary clipping (drawChar w/big text requires this)
  if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
  if((x + w - 1) >= ST7735_WIDTH)  w = ST7735_WIDTH  - x;
  if((y + h - 1) >= ST7735_HEIGHT) h = ST7735_HEIGHT - y;

  ST7735_SetAddrWindow(x, y, x+w-1, y+h-1);

  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      writeData16(color);
    }
  }
}

void ST7735_FillScreen(uint16_t color)
{
  ST7735_FillRect(0, 0, ST7735_WIDTH-1, ST7735_HEIGHT-1, color);
}

void ST7735_DrawPixel(int x, int y, uint16_t color)
{
  if((x<0)||(y<0)||(x>=ST7735_WIDTH)||(y>=ST7735_HEIGHT)) return;
  ST7735_SetAddrWindow(x,y,x,y);
  writeCommand(0x2C);
  writeData16(color);
}

void ST7735_DrawLine(uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int steep = abs(y2-y1)>abs(x2-x1);
  if(steep)
  {
    swap(x1,y1);
    swap(x2,y2);
  }
  if(x1>x2)
  {
    swap(x1,x2);
    swap(y1,y2);
  }
  int dx,dy;
  dx=x2-x1;
  dy=abs(y2-y1);
  int err=dx/2;
  int ystep;
  if(y1<y2) ystep=1;
  else ystep=-1;
  for(;x1<=x2;x1++)
  {
    if(steep) ST7735_DrawPixel(y1,x1,color);
    else ST7735_DrawPixel(x1,y1,color);
    err-=dy;
    if(err<0)
    {
      y1 += ystep;
      err=dx;
    }
  }
}

void ST7735_DrawRect(uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  ST7735_DrawLine(color,x1,y1,x2,y1);
  ST7735_DrawLine(color,x2,y1,x2,y2);
  ST7735_DrawLine(color,x1,y1,x1,y2);
  ST7735_DrawLine(color,x1,y2,x2,y2);
}

void ST7735_DrawCircle(uint16_t x0, uint16_t y0, int r, uint16_t color)
{
  int f = 1-r;
  int ddF_x=1;
  int ddF_y=-2*r;
  int x = 0;
  int y = r;
  ST7735_DrawPixel(x0,y0+r,color);
  ST7735_DrawPixel(x0,y0-r,color);
  ST7735_DrawPixel(x0+r,y0,color);
  ST7735_DrawPixel(x0-r,y0,color);
  while (x<y)
  {
    if (f>=0)
    {
      y--;
      ddF_y+=2;
      f+=ddF_y;
    }
    x++;
    ddF_x+=2;
    f+=ddF_x;
    ST7735_DrawPixel(x0+x,y0+y,color);
    ST7735_DrawPixel(x0-x,y0+y,color);
    ST7735_DrawPixel(x0+x,y0-y,color);
    ST7735_DrawPixel(x0-x,y0-y,color);
    ST7735_DrawPixel(x0+y,y0+x,color);
    ST7735_DrawPixel(x0-y,y0+x,color);
    ST7735_DrawPixel(x0+y,y0-x,color);
    ST7735_DrawPixel(x0-y,y0-x,color);
  }
}

void ST7735_SetTextColor(uint16_t color)
{
  lcdprop.TextColor=color;
}

void ST7735_SetBackColor(uint16_t color)
{
  lcdprop.BackColor=color;
}

void ST7735_SetFont(sFONT *pFonts)
{
  lcdprop.pFont=pFonts;
}

void ST7735_DrawChar(uint16_t x, uint16_t y, uint8_t c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t offset;
  uint8_t *c_t;
  uint8_t *pchar;
  uint32_t line=0;
  height = lcdprop.pFont->Height;
  width  = lcdprop.pFont->Width;
  offset = 8 *((width + 7)/8) -  width ;
  c_t = (uint8_t*) &(lcdprop.pFont->table[(c-' ') * lcdprop.pFont->Height * ((lcdprop.pFont->Width + 7) / 8)]);
  for(i = 0; i < height; i++)
  {
    pchar = ((uint8_t *)c_t + (width + 7)/8 * i);
    switch(((width + 7)/8))
    {
      case 1:
          line =  pchar[0];      
          break;
      case 2:
          line =  (pchar[0]<< 8) | pchar[1];
          break;
      case 3:
      default:
        line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];      
        break;
    }
    for (j = 0; j < width; j++)
    {
      if(line & (1 << (width- j + offset- 1))) 
      {
        ST7735_DrawPixel((x + j), y, lcdprop.TextColor);
      }
      else
      {
        ST7735_DrawPixel((x + j), y, lcdprop.BackColor);
      } 
    }
    y++;      
  }
}

void ST7735_String(uint16_t x,uint16_t y, char *str)
{
  while(*str)
  {
    ST7735_DrawChar(x,y,str[0]);
    x+=lcdprop.pFont->Width;
    (void)*str++;
  }
}

void ST7735_SetRotation(uint8_t  orientation)
{
  writeCommand(ST7735_MADCTL);
  switch(orientation)
  {
    case 0:
     ST7735_WIDTH  = 128;
     ST7735_HEIGHT = 160;
     writeData(ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_RGB);
    break;
    case 1:
     ST7735_WIDTH  = 128;
     ST7735_HEIGHT = 160;
     writeData(ST7735_MADCTL_RGB);
    break;
    case 2:
     ST7735_WIDTH  = 160;
     ST7735_HEIGHT = 128;
    writeData(ST7735_MADCTL_SWAP_XY_Mirror_Y | ST7735_MADCTL_RGB);
    break;
    case 3:
     ST7735_WIDTH  = 160;
     ST7735_HEIGHT = 128;
    writeData(ST7735_MADCTL_SWAP_XY_Mirror_X | ST7735_MADCTL_RGB);
    break;
  }
}

void ST7735_FontsIni(void)
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
  lcdprop.BackColor=ST7735_BLACK;
  lcdprop.TextColor=ST7735_GREEN;
  lcdprop.pFont=&Font16;
}

void ST7735_ini(uint16_t w_size, uint16_t h_size)
{
  uint8_t data[16];
  lcd_reset();
  // Software Reset
  writeCommand(0x01);
  LL_mDelay(100);
  // Power Control A
  writeCommand(0xCB);
  data[0] = 0x39;
  data[1] = 0x2C;
  data[2] = 0x00;
  data[3] = 0x34;
  data[4] = 0x02;
  data[5] = '\0'; // NULL-terminated
  write_data_array(data);
  // Power Control B
  writeCommand(0xCF);
  data[0] = 0x00;
  data[1] = 0xC1;
  data[2] = 0x30;
  data[3] = '\0'; // NULL-terminated
  write_data_array(data);
  // Driver timing control A
  writeCommand(0xE8);
  data[0] = 0x85;
  data[1] = 0x00;
  data[2] = 0x78;
  data[3] = '\0'; // NULL-terminated
  write_data_array(data);
  // Driver timing control B
  writeCommand(0xEA);
  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = '\0'; // NULL-terminated
  write_data_array(data);
  // Power on Sequence control
  writeCommand(0xED);
  data[0] = 0x64;
  data[1] = 0x03;
  data[2] = 0x12;
  data[3] = 0x81;
  data[4] = '\0'; // NULL-terminated
  write_data_array(data);
  // Pump ratio control
  writeCommand(0xF7); 
  data[0] = 0x20;
  data[1] = '\0'; // NULL-terminated
  write_data_array(data);
  // Power Control,VRH[5:0]
  writeCommand(0xC0);
  data[0] = 0x10;
  data[1] = '\0'; // NULL-terminated
  write_data_array(data);
  // Power Control,SAP[2:0];BT[3:0]
  writeCommand(0xC1);
  data[0] = 0x10;
  data[1] = '\0'; // NULL-terminated
  write_data_array(data);
  // VCOM Control 1
  writeCommand(0xC5);
  data[0] = 0x3E;
  data[1] = 0x28;
  data[2] = '\0'; // NULL-terminated
  write_data_array(data);
  // VCOM Control 2
  writeCommand(0xC7);
  data[0] = 0x86;
  data[1] = '\0'; // NULL-terminated
  write_data_array(data);
  // Memory Acsess Control
  writeCommand(0x36);
  data[0] = 0x48;
  data[1] = '\0'; // NULL-terminated
  write_data_array(data);
  // Pixel Format Set
  writeCommand(0x3A); 
  data[0] = 0x55; // 16bit
  data[1] = '\0'; // NULL-terminated
  write_data_array(data);
  // Frame Rratio Control, Standard RGB Color
  writeCommand(0xB1);
  data[0] = 0x00;
  data[1] = 0x18;
  data[2] = '\0'; // NULL-terminated
  write_data_array(data);
  // Display Function Control
  writeCommand(0xB6);
  data[0] = 0x08;
  data[1] = 0x82;
  data[2] = 0x27; // 320 line size tft
  data[3] = '\0'; // NULL-terminated
  write_data_array(data);
  // Enable 3G
  writeCommand(0xF2);
  data[0] = 0x00;
  data[1] = '\0'; // NULL-terminated
  write_data_array(data);
  // Gamma set
  writeCommand(0x26); 
  data[0] = 0x01; // Gamma Curve (G2.2)
  data[1] = '\0'; // NULL-terminated
  write_data_array(data);
  // Positive Gamma  Correction
  writeCommand(0xE0); 
  data[0] = 0x0F;
  data[1] = 0x31;
  data[2] = 0x2B;
  data[3] = 0x0C;
  data[4] = 0x0E;
  data[5] = 0x08;
  data[6] = 0x4E;
  data[7] = 0xF1;
  data[8] = 0x37;
  data[9] = 0x07;
  data[10] = 0x10;
  data[11] = 0x03;
  data[12] = 0x0E;
  data[13] = 0x09;
  data[14] = 0x00;
  data[15] = '\0'; // NULL-terminated
  write_data_array(data);
  // Negative Gamma  Correction
  writeCommand(0xE1);
  data[0] = 0x00;
  data[1] = 0x0E;
  data[2] = 0x14;
  data[3] = 0x03;
  data[4] = 0x11;
  data[5] = 0x07;
  data[6] = 0x31;
  data[7] = 0xC1;
  data[8] = 0x48;
  data[9] = 0x08;
  data[10] = 0x0F;
  data[11] = 0x0C;
  data[12] = 0x31;
  data[13] = 0x36;
  data[14] = 0x0F;
  data[15] = '\0'; // NULL-terminated
  write_data_array(data);
  writeCommand(0x11); // sleep out
  LL_mDelay(120);
  // Display ON
  writeCommand(0x29); 
  data[0] = ST7735_ROTATION;
  data[1] = '\0'; // NULL-terminated
  write_data_array(data);
  ST7735_WIDTH = w_size;
  ST7735_HEIGHT = h_size;
  ST7735_FontsIni();
}


