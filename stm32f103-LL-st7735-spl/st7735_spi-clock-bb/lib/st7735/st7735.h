#ifndef __ST7735_H__
#define __ST7735_H__

typedef enum {
	scr_normal = 0,
	scr_CW     = 1,
	scr_CCW    = 2,
	scr_180    = 3
} ScrOrientation_TypeDef;

extern uint16_t scr_width;
extern uint16_t scr_height;

void *mmcpy(void *dest, const void *src, uint8_t n);
uint16_t RGB565(uint8_t R,uint8_t G,uint8_t B);
void ST7735_Init(void);
void ST7735_AddrSet(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ST7735_Orientation(ScrOrientation_TypeDef orientation);
void ST7735_Clear(uint16_t color);
void ST7735_Pixel(uint16_t X, uint16_t Y, uint16_t color);
void ST7735_HLine(uint16_t X1, uint16_t X2, uint16_t Y, uint16_t color);
void ST7735_VLine(uint16_t X, uint16_t Y1, uint16_t Y2, uint16_t color);
void ST7735_Line(int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint16_t color);
void ST7735_Rect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color);
void ST7735_FillRect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t color);
void ST7735_PutChar5x7(uint16_t X, uint16_t Y, uint8_t chr, uint16_t color);
void ST7735_PutStr5x7(uint8_t X, uint8_t Y, char *str, uint16_t color);
void ST7735_PutChar_asc2_1608(uint16_t X, uint16_t Y, uint8_t chr, uint16_t color);
void ST7735_PutStr_asc2_1608(uint8_t X, uint8_t Y, char *str, uint16_t color);
void ST7735_PutChar_SmallFont(uint16_t X, uint16_t Y, uint8_t chr, uint16_t color);
void ST7735_PutStr_SmallFont(uint8_t X, uint8_t Y, char *str, uint16_t color);

#endif // __ST7735_H__
