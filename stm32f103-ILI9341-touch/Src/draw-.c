#include "draw.h"
#include "ILI9341.h"
#include "xpt2046.h"
#include "adc.h"

#define width 320
#define height 240

extern uint16_t ofset_ray; // vertical ofset ray
extern uint16_t accuracy; // speed - accuracy ray

uint16_t lcd_height = height;
uint16_t lcd_width = width;
uint16_t BACK_COLOR,POINT_COLOR;
uint16_t adc_counter = 0;
uint16_t i = 0;
uint16_t count_draw = 0;
uint32_t adcResult = 0;
uint32_t max_result = 0;
float real_result[width] = {0};
uint16_t buff_clean[width] = {0};
//uint16_t coord_x[width] = {0};
uint16_t buff_ray[width] = {0};

/*
 * test touch panel
 */
void draw_touch(uint16_t coord_X, uint16_t coord_Y, uint16_t coord_Z) // for text data
{

  ILI9341_setTextBgColor(ILI9341_BLACK);

  ILI9341_setCursor((lcd_width>>2), (lcd_height>>1)-28);
  ILI9341_setTextSize(2);
  ILI9341_setTextColor(ILI9341_CYAN);
  char lb_cord_x[16] = "CordX";
  ILI9341_writeString(lb_cord_x);

  ILI9341_setTextSize(3);
  ILI9341_setTextColor(ILI9341_YELLOW);

  char real_x[16];
  utoa((int)coord_X, real_x, 10);
  if(coord_X<1000)
      ILI9341_write('0');
  if(coord_X<100)
      ILI9341_write('0');
  ILI9341_writeString(real_x);

  ILI9341_setCursor((lcd_width>>2), (lcd_height>>1)+8);
  ILI9341_setTextSize(2);
  ILI9341_setTextColor(ILI9341_CYAN);
  char lb_cord_y[16] = "CordY";
  ILI9341_writeString(lb_cord_y);

  ILI9341_setTextSize(3);
  ILI9341_setTextColor(ILI9341_YELLOW);

  char real_y[16];
  utoa((int)coord_Y, real_y, 10);
  if(coord_Y<1000)
      ILI9341_write('0');
  if(coord_Y<100)
      ILI9341_write('0');
  ILI9341_writeString(real_y);

  ILI9341_setCursor((lcd_width>>2), (lcd_height>>1)+42);
  ILI9341_setTextSize(2);
  ILI9341_setTextColor(ILI9341_CYAN);
  char lb_cord_z[16] = "CordZ";
  ILI9341_writeString(lb_cord_z);

  ILI9341_setTextSize(3);
  ILI9341_setTextColor(ILI9341_YELLOW);

  char real_z[16];
  utoa((int)coord_Z, real_z, 10);
  if(coord_Z<1000)
      ILI9341_write('0');
  if(coord_Z<100)
      ILI9341_write('0');
  ILI9341_writeString(real_z);
}

/*
 * init
 * begin draw grid
 * и string parameters(write_text)
 */
void oscil_init(void)
{
  ILI9341_setRotation(1);
  ILI9341_fillScreen(ILI9341_BLACK);

  ILI9341_fillRect(31,0,0,lcd_width,ILI9341_RED); // draw contour statusbar
  ILI9341_fillRect(1,1,30,lcd_width-1,ILI9341_YELLOW); // draw statusbar

  ILI9341_fillRect(lcd_height-31,0,lcd_height,lcd_width,ILI9341_RED); // draw contour statusbar
  ILI9341_fillRect(lcd_height-30,1,lcd_height-1,lcd_width-1,ILI9341_YELLOW); // draw statusbar

  write_grid(); // draw grid
  write_text(i); // draw label & voltage measurement

  BACK_COLOR=ILI9341_RED;
  POINT_COLOR=ILI9341_BLUE;
  //drawChar(lcd_width/2,20,'A',TFT_STRING_MODE_NO_BACKGROUND, 2);
}

/*
 * main loop
 * постоянно вызывается из main
 */
void oscil_run(void)
{
    float t, a1, a2;

    t = 3300/2; // порог в миливольтах
    fill_buff(0); a1 = real_result[0];
    fill_buff(0); a2 = real_result[0];

    while (!((a1 < t) && (a2 >= t))) // т.е. было ниже порога, а стало больше или равно - был переход через порог
    {
      a1 = a2;
      fill_buff(0); a2 = real_result[0];
    }
    
    rotate_buff(5); // scrolling buffer

    for(uint16_t i = 0; i < lcd_width; i++) // measurement
    {
      fill_buff(i);
    }

    for(uint16_t i = 0; i < lcd_width; i++) // display redraw
    {
      write_graph(i);
    }

    write_grid(); // upgrade grid
    write_text(i); // upgrade label & voltage measurement
}

/*
 * save measure adc and fill buffers buff_ray[i] & real_result[i]
 * buff_ray[] use for draw ray with ofset vertical
 * real_result[] save measure from adc with coefitient for move volts to bars
 */
void fill_buff(uint16_t i)
{
  while(adc_counter < accuracy) // while counter measure less N
  {
    adcResult += HAL_ADC_GetValue(&hadc1); // Сумируем полученные значения ацп
    adc_counter++; // Увеличиваем счетчик измерений
  }

  buff_ray[i] = lcd_height+(-ofset_ray)-adcResult/adc_counter/18; // Вычисляем среднее значение
  real_result[i] = adcResult/adc_counter*0.82; // 3300/4096=0.80566 Вычисляем среднее значение

  // if(real_result[i]<500) real_result[i] = 0;
  // if(real_result[i]>500) real_result[i] -= 500;
  // real_result[i] *= .4; // значение в барах

  adcResult = 0; // Обнуляем значение
  adc_counter = 0; // Обнуляем счетчик измерений
  if(real_result[i]>max_result) max_result = real_result[i]; // save max value
}

void rotate_buff(uint16_t r)
{
    uint16_t tmp;
    for(i = 0; i < r; i++) // scroll buff_ray
    {
      tmp = buff_ray[lcd_width-1];
      for(uint16_t i = lcd_width-1; i; i--)
      {
        buff_ray[i] = buff_ray[i-1];
      }
      buff_ray[0] = tmp;
    }
}

/*
 * draw string parameters
 */
void write_text(uint16_t i) // for text data
{
  count_draw++;
  if(count_draw<20)
    return;
  count_draw=0;

  ILI9341_setTextBgColor(ILI9341_BLACK);

  ILI9341_setCursor(10, 15);
  ILI9341_setTextSize(2);
  ILI9341_setTextColor(ILI9341_CYAN);
  char lb_real[16] = " bar ";
  ILI9341_writeString(lb_real);

  ILI9341_setTextSize(2);
  ILI9341_setTextColor(ILI9341_YELLOW);

  char real_mv[16];
  utoa((int)real_result[i], real_mv, 10);
  if(real_result[i]<1000)
      ILI9341_write('0');
  if(real_result[i]<100)
      ILI9341_write('0');
  ILI9341_writeString(real_mv);

  ILI9341_setCursor(160, 15);
  ILI9341_setTextSize(2);
  ILI9341_setTextColor(ILI9341_CYAN);
  char lb_max[16] = " Max ";
  ILI9341_writeString(lb_max);

  ILI9341_setTextSize(2);
  ILI9341_setTextColor(ILI9341_YELLOW);

  char max_mv[16];
  utoa((int)max_result, max_mv, 10);
  if(max_result<1000)
      ILI9341_write('0');
  if(max_result<100)
      ILI9341_write('0');
  ILI9341_writeString(max_mv);
}

/*
 * draw old ray & draw new ray
 */
void write_graph(uint16_t i)
{
  //write_text(i);
  if(i>0 && i < lcd_width-0){ // clean old ray
    //tft_draw_point_big(i, buff_clean[i],ILI9341_BLACK); //draw old ray;
    //tft_draw_line_slim(i+1, buff_clean[i+1], i, buff_clean[i], ILI9341_BLACK); //draw old ray;
    ILI9341_drawDotLine(i+1, buff_clean[i+1], i, buff_clean[i], 8, ILI9341_BLACK); //draw old ray;
  }
  if(i>2 && i < lcd_width-2){ // draw new ray
    //tft_draw_point_big(i, buff_ray[i], ILI9341_WHITE); //draw new ray;
    //tft_draw_line_slim(i, buff_ray[i], i-1, buff_ray[i-1], ILI9341_WHITE); //draw new ray;
    ILI9341_drawDotLine(i, buff_ray[i], i-1, buff_ray[i-1], 8, ILI9341_WHITE); //draw new ray;
  }
  buff_clean[i]=buff_ray[i];
}

/*
 * draw coordinate grid
 */
void write_grid(void)
{
  uint16_t space = 18;
  for(uint16_t y = 12; y<lcd_height-8; y+=36)for(uint16_t x = 12; x<lcd_width-12; x+=36)ILI9341_drawCircle(x, y, 0, ILI9341_YELLOW); //draw vertical dot line
  for(uint16_t y = 12; y<lcd_height-8; y+=36)for(uint16_t x = 12; x<lcd_width-12; x+=36)ILI9341_drawCircle(x, y, 0, ILI9341_YELLOW); //draw horizontal dot line

  for(uint16_t y = 12; y<lcd_height-8; y+=space)ILI9341_drawCircle(12, y, 0, ILI9341_YELLOW); //draw vertical up line
  for(uint16_t y = 12; y<lcd_height-8; y+=space)ILI9341_drawCircle(lcd_width-19, y, 0, ILI9341_YELLOW); //draw vertical down line
  for(uint16_t y = 12; y<lcd_height-8; y+=space)ILI9341_drawCircle(lcd_width/2-4, y, 0, ILI9341_YELLOW); //draw vertical central line

  for(uint16_t x = 12; x<lcd_width-12; x+=space)ILI9341_drawCircle(x, 12, 0, ILI9341_YELLOW); //draw horizontal up line
  for(uint16_t x = 12; x<lcd_width-12; x+=space)ILI9341_drawCircle(x, lcd_height/2, 0, ILI9341_YELLOW); //draw horizontal central line
  for(uint16_t x = 12; x<lcd_width-12; x+=space)ILI9341_drawCircle(x, lcd_height-12, 0, ILI9341_YELLOW); //draw horizontal down line
}
