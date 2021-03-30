#include "draw.h"

#define width 480
#define height 320

extern uint16_t ofset_ray; // vertical ofset ray
extern uint16_t accuracy; // speed - accuracy ray
TS_Point p;

uint32_t pin_status_led = 0; // internal test generation
volatile uint32_t ms_counter=0; // incrementation in interrupts
volatile uint32_t time_freeze=0; // time freeze
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
uint16_t min_ofset=800;
//uint16_t coord_x[width] = {0};
uint16_t buff_ray[width] = {0};
bool trigger_synhro = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3)
{
  pin_status_led++;
  if(pin_status_led>100000)
  {
    GPIOC->ODR ^= GPIO_ODR_ODR13;
    pin_status_led = 0;
  }
}

void trigger_synhro_reset(void)
{
  if(pin_status_led>90000)
  {
    pin_status_led=0;
    lcd_fillframe((lcd_width>>1)-4, (lcd_height>>1)-4, 8,8, GREEN); // center-center touch point
    lcd_fillframe((lcd_width>>1)-4, (lcd_height)-12, 8,8, GREEN); // center-down touch point
    trigger_synhro=0;
  }
}

/*
* main while
* постоянно вызывается из main
*/
void oscil_run(void)
{
  get_point(&p); // get coordinates x y z from xpt2046

  if(p.z>100)frequency_measure();
  trigger_synhro_on();
  trigger_synhro_off();
  trigger_amplitude();

  rotate_buff(5); // scrolling buffer
  for(uint16_t i = 0; i < lcd_width; i++) // measurement
  {
    fill_buff(i);
    if(i<16) ms_counter=0; // сбрасываем счечик времени
    if(i==(lcd_width-20))time_freeze = ms_counter;
  }
  for(uint16_t i = 16; i < lcd_width-20; i++) // display redraw
  {
    draw_graph(i);
  }
  if(p.z>100)draw_ms(time_freeze); // время за которое заполнился буфер
  if(p.z>100)draw_freq(1000/time_freeze); // частота заполнения заполнения буфера

  draw_dot_grid(); // upgrade grid
  count_draw++;
  if(count_draw>15)
  {
    count_draw=0;
    draw_pressure(i); // upgrade label & voltage measurement
    draw_max(i);
  }
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
  buff_ray[i] = lcd_height+(-ofset_ray)-adcResult/adc_counter/20; // буфер для рисования луча
  real_result[i] = adcResult/adc_counter*0.82; // 3300/4096=0.80566 буфер для сохранения напряжения
  // if(real_result[i]<500) real_result[i] = 0; // отношение напряжения к давлению, со смещением 500
  // if(real_result[i]>500) real_result[i] -= 500; // отнимаем 500 едениц от показаний
  // real_result[i] *= .4; // значение в барах
  adcResult = 0; // Обнуляем значение
  adc_counter = 0; // Обнуляем счетчик измерений
  if(real_result[i]>max_result) max_result = real_result[i]; // save max value
}

/*
* сдвиг луча по coord_x
* задается параметром r
*/
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
* init
* begin draw grid
* и string parameters(write_text)
*/
void draw_statusbar(void)
{
  lcd_fill(BLACK);
  lcd_fillframe(2,2,lcd_width,16,DARKGREY); // draw statusbar
  lcd_fillframe(2,lcd_height-16,lcd_width,16,DARKGREY); // draw statusbar

  lcd_fillframe(8, 6, 8,8, GREEN); // left-up touch point
  lcd_fillframe(8, (lcd_height>>1)-4, 8,8, GREEN); // left-center touch point
  lcd_fillframe(8, (lcd_height)-12, 8,8, GREEN); // left-down touch point

  lcd_fillframe((lcd_width>>1)-4, 6, 8,8, GREEN); // center-up touch point
  lcd_fillframe((lcd_width>>1)-4, (lcd_height>>1)-4, 8,8, GREEN); // center-center touch point
  lcd_fillframe((lcd_width>>1)-4, (lcd_height)-12, 8,8, GREEN); // center-down touch point

  lcd_fillframe(lcd_width-16, 6, 8,8, GREEN); // right-up touch point
  lcd_fillframe(lcd_width-16, (lcd_height>>1)-4, 8,8, GREEN); // right-center touch point
  lcd_fillframe(lcd_width-16, (lcd_height)-12, 8,8, GREEN); // right-down touch point
}

/*
* draw string parameters
*/
void draw_pressure(uint16_t i) // for text data
{
  lcd_setTextBgColor(BLACK);
  lcd_setCursor(10, 2);
  lcd_setTextSize(2);
  lcd_setTextColor(CYAN);
  char lb_real[12] = " bar ";
  lcd_writeString(lb_real);

  lcd_setTextSize(2);
  lcd_setTextColor(YELLOW);
  char real_mv[12];
  utoa((int)real_result[i], real_mv, 10);
  for(uint8_t i=0;i<4;i++)
  {
    if(real_mv[i]<1)lcd_write('0');
  }
  lcd_writeString(real_mv);
}

void draw_max(uint16_t i) // for text data
{
  lcd_setTextBgColor(BLACK);
  lcd_setCursor(160, 2);
  lcd_setTextSize(2);
  lcd_setTextColor(CYAN);
  char lb_max[12] = " Max ";
  lcd_writeString(lb_max);

  lcd_setTextSize(2);
  lcd_setTextColor(YELLOW);
  char max_mv[12];
  utoa((int)max_result, max_mv, 10);
  for(uint8_t i=0;i<4;i++)
  {
    if(max_mv[i]<1)lcd_write('0');
  }
  lcd_writeString(max_mv);
}

/*
*
* full time fill buffer adc
* and frequency for full screen
*
*/
void draw_ms(uint32_t time_freeze)
{
  lcd_setCursor((lcd_width>>4), lcd_height-16);
  lcd_setTextSize(1);
  lcd_setTextColor(CYAN);
  char lbl_time_ms[12] = " ms ";
  lcd_writeString(lbl_time_ms);

  lcd_setTextSize(1);
  lcd_setTextColor(YELLOW);
  char time_ms[12];
  utoa((int)time_freeze, time_ms, 10);
  for(uint8_t i=0;i<3;i++)
  {
    if(time_ms[i]<1)lcd_write('0');
  }
  lcd_writeString(time_ms);

  lcd_fillframe(4, (lcd_height>>1)-4, 8,8, BLUE); //draw horizontal central line
  lcd_fillframe(lcd_width-16, (lcd_height>>1)-4, 8,8, BLUE); //draw horizontal central line
}

void draw_freq(uint32_t time_freeze)
{
  lcd_setCursor((lcd_width-140), lcd_height-16);
  lcd_setTextSize(1);
  lcd_setTextColor(CYAN);
  char lbl_freq[12] = " freq ";
  lcd_writeString(lbl_freq);

  lcd_setTextSize(1);
  lcd_setTextColor(YELLOW);
  char frequency[12];
  utoa((int)(time_freeze), frequency, 10);
  for(uint8_t i=0;i<3;i++)
  {
    if(frequency[i]<1)lcd_write('0');
  }
  lcd_writeString(frequency);

  lcd_fillframe(4, (lcd_height>>1)-4, 8,8, BLUE); //draw horizontal central line
  lcd_fillframe(lcd_width-16, (lcd_height>>1)-4, 8,8, BLUE); //draw horizontal central line
}

/*
* draw old ray & draw new ray
* рисуем когда i больше минимального,
* тоесть пропускаем первые координаты
*/
void draw_graph(uint16_t i)
{
  if(i<min_ofset)min_ofset = i;
  lcd_drawDotLine(i+1, buff_clean[i+1], i, buff_clean[i], 4, BLACK); //draw old ray;
  if(i!=min_ofset)lcd_drawDotLine(i, buff_ray[i], i-1, buff_ray[i-1], 4, WHITE); //draw new ray;
  buff_clean[i]=buff_ray[i];
}

/*
* draw coordinate grid
*/
void draw_dot_grid(void)
{
  uint16_t ofset_x = lcd_width>>4;
  uint16_t ofset_y = lcd_height>>4;
  for(uint16_t y = ofset_y; y<=lcd_height-ofset_y; y+=ofset_y)for(uint16_t x = ofset_x; x<=lcd_width-ofset_x; x+=ofset_x)lcd_drawCircle(x, y, 0, GREEN); //draw horizontal dot line
  lcd_drawDotLine(0, ofset_y, 0, lcd_height-ofset_y, ofset_y>>2, GREEN); // vertical-begin-dot-line
  lcd_drawDotLine(lcd_width>>1, ofset_y, lcd_width>>1, lcd_height-ofset_y, ofset_y>>2, GREEN); // vertical-center-dot-line
  lcd_drawDotLine(lcd_width-1, ofset_y, lcd_width-1, lcd_height-ofset_y, ofset_y>>2, GREEN); // vertical-end-dot-line
  lcd_drawDotLine(ofset_y>>2, ofset_y, lcd_width-(ofset_y>>2), ofset_y, ofset_y>>2, GREEN); // horizontal-begin-dot-line
  lcd_drawDotLine(ofset_y>>2, lcd_height>>1, lcd_width-(ofset_y>>2), lcd_height>>1, ofset_y>>2, GREEN); // horizontal-center-dot-line
  lcd_drawDotLine(ofset_y>>2, height-ofset_y, lcd_width-(ofset_y>>2), height-ofset_y, ofset_y>>2, GREEN); // horizontal-end-dot-line
}

/*
* draw coordinate grid
*/
void draw_grid(void)
{
  uint16_t space = 18;
  for(uint16_t y = 12; y<lcd_height-8; y+=36)for(uint16_t x = 12; x<lcd_width-12; x+=36)lcd_drawCircle(x, y, 0, YELLOW); //draw vertical dot line
  for(uint16_t y = 12; y<lcd_height-8; y+=36)for(uint16_t x = 12; x<lcd_width-12; x+=36)lcd_drawCircle(x, y, 0, YELLOW); //draw horizontal dot line
  for(uint16_t y = 12; y<lcd_height-8; y+=space)lcd_drawCircle(12, y, 0, YELLOW); //draw vertical up line
  for(uint16_t y = 12; y<lcd_height-8; y+=space)lcd_drawCircle(lcd_width-19, y, 0, YELLOW); //draw vertical down line
  for(uint16_t y = 12; y<lcd_height-8; y+=space)lcd_drawCircle(lcd_width/2-4, y, 0, YELLOW); //draw vertical central line
  for(uint16_t x = 12; x<lcd_width-12; x+=space)lcd_drawCircle(x, 12, 0, YELLOW); //draw horizontal up line
  for(uint16_t x = 12; x<lcd_width-12; x+=space)lcd_drawCircle(x, lcd_height/2, 0, YELLOW); //draw horizontal central line
  for(uint16_t x = 12; x<lcd_width-12; x+=space)lcd_drawCircle(x, lcd_height-12, 0, YELLOW); //draw horizontal down line
}
