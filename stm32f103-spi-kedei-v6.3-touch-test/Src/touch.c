#include "touch.h"

#define tc_min_x 245
#define tc_max_x 3920
#define tc_center_x (tc_max_x>>1)+(tc_min_x>>1)

#define tc_min_y 245
#define tc_max_y 3920
#define tc_center_y (tc_max_y>>1)+(tc_min_y>>1)

extern uint16_t ofset_ray; // vertical ofset ray
extern uint16_t accuracy; // speed - accuracy ray
TS_Point p;

extern uint16_t lcd_height;
extern uint16_t lcd_width;
extern float real_result[];
extern bool trigger_synhro;

void trigger_synhro_on(void)
{
  if(p.z<100)return;
  if(p.x>2048-128 && p.x<2048+128 && p.y>1950-128 && p.y<1950+128 ) // center point
  {
    lcd_fillframe((lcd_width>>1)-4, (lcd_height>>1)-4, 8,8, RED); // center-center touch point
    lcd_fillframe((lcd_width>>1)-4, (lcd_height)-12, 8,8, BLUE); // center-down touch point
    trigger_synhro=1;
  }
}

/*
*/
void frequency_measure(void)
{
  if(p.x>tc_center_x-tc_min_x && p.x<tc_center_x+tc_min_x && p.y>tc_max_y-tc_min_y && p.y<tc_max_y) accuracy+=96; // draw rect left center
  if(p.x>tc_center_x-tc_min_x && p.x<tc_center_x+tc_min_x && p.y>tc_min_y && p.y<tc_min_y*2) accuracy-=96; // draw rect right center
  if(accuracy>1024) accuracy=4; // adc counter in cycle
  if(accuracy<4) accuracy=1024; // adc counter in cycle
  //draw_ms(time_freeze); // время за которое заполнился буфер
  //draw_freq(1000/time_freeze); // частота заполнения заполнения буфера
}

void trigger_synhro_off(void)
{
  if(p.z<100)return;
  if(p.x>380-128 && p.x<380+128 && p.y>1950-128 && p.y<1950+128 ) // center point
  {
    lcd_fillframe((lcd_width>>1)-4, (lcd_height>>1)-4, 8,8, GREEN); // center-center touch point
    lcd_fillframe((lcd_width>>1)-4, (lcd_height)-12, 8,8, GREEN); // center-down touch point
    trigger_synhro=0;
  }
}

/*
* test touch panel
*/
void test_touch(void)
{
  get_point(&p); // get coordinates x y z from xpt2046
  if(p.z<100)return; // redraw only if pressed in touch panel
  draw_touch(p.x,p.y,p.z); // for text data
  lcd_fillframe(8, 6, 8,8, GREEN); // draw rect for touch point
  lcd_fillframe(8, (lcd_height>>1)-4, 8,8, GREEN); // draw rect for touch point
  lcd_fillframe(8, (lcd_height)-12, 8,8, GREEN); // draw rect for touch point
  lcd_fillframe(lcd_width-16, 6, 8,8, GREEN); // draw rect for touch point
  lcd_fillframe(lcd_width-16, (lcd_height>>1)-4, 8,8, GREEN); // draw rect for touch point
  lcd_fillframe(lcd_width-16, (lcd_height)-12, 8,8, GREEN); // draw rect for touch point
  if(p.x>3580 && p.x<3780 && p.y>3530 && p.y<3730 )lcd_fillframe(8, 6, 8,8, YELLOW); // draw rect left up
  if(p.x>tc_center_x-256 && p.x<tc_center_x+256 && p.y>tc_max_y-(tc_min_y>>2) && p.y<tc_max_y )lcd_fillframe(8, (lcd_height>>1)-4, 8,8, YELLOW); // draw rect left center
  if(p.x>270 && p.x<470 && p.y>3530 && p.y<3730 )lcd_fillframe(8, (lcd_height)-12, 8,8, YELLOW); // draw rect left down
  if(p.x>3580 && p.x<3780 && p.y>170 && p.y<370 )lcd_fillframe(lcd_width-16, 6, 8,8, YELLOW); // draw rect right up
  if(p.x>tc_center_x-256 && p.x<tc_center_x+256 && p.y>tc_min_y && p.y<(tc_min_y>>2))lcd_fillframe(lcd_width-16, (lcd_height>>1)-4, 8,8, YELLOW); // draw rect right center
  if(p.x>270 && p.x<470 && p.y>170 && p.y<370 )lcd_fillframe(lcd_width-16, (lcd_height)-12, 8,8, YELLOW); // draw rect right down
}

/*
* синхронизация по уровню:
* захват уровня
*/
float level = 3300/2; // порог в миливольтах
void trigger_amplitude(void)
{
  if(!trigger_synhro)return;
  float a1, a2;
  if(p.x>3580 && p.x<3780 && p.y>170 && p.y<370 )level+=64; // draw rect right up
  if(level>3000)level=256; // если проскочили максимальный уровень, тогда начинаем с минимального
  if(p.x>270 && p.x<470 && p.y>170 && p.y<370 )level-=64; // draw rect right down
  if(level<256)level=3000; // если проскочили минимальный уровень, тогда начинаем с максимального
  fill_buff(0); a1 = real_result[0];
  fill_buff(0); a2 = real_result[0];
  while (!((a1 < level) && (a2 >= level))) // т.е. было ниже порога, а стало больше или равно - был переход через порог
  {
    a1 = a2;
    fill_buff(0); a2 = real_result[0];
  }
  draw_trigger(level);
}

/*
 *
 */
void draw_trigger(uint32_t trg)
{
  get_point(&p); // get coordinates x y z from xpt2046
  if(p.z<100)return; // redraw only if pressed in touch panel
  lcd_setCursor((lcd_width-70), lcd_height-16);
  lcd_setTextSize(1);
  lcd_setTextColor(CYAN);
  char lbl_trg[12] = " trg ";
  lcd_writeString(lbl_trg);

  lcd_setTextSize(1);
  lcd_setTextColor(YELLOW);
  char trigger[12];
  utoa((int)(trg), trigger, 10);
  for(uint8_t i=0;i<4;i++)
  {
    if(trigger[i]<1)lcd_write('0');
  }
  lcd_writeString(trigger);

  lcd_fillframe(4, (lcd_height>>1)-4, 8,8, BLUE); //draw horizontal central line
  lcd_fillframe(lcd_width-16, (lcd_height>>1)-4, 8,8, BLUE); //draw horizontal central line
}

/*
* draw touch panel
*/
void draw_touch(uint16_t coord_X, uint16_t coord_Y, uint16_t coord_Z) // for text data
{
  lcd_setTextBgColor(BLACK);
  lcd_setCursor((lcd_width>>2), (lcd_height>>1)-28);
  lcd_setTextSize(3);
  lcd_setTextColor(CYAN);
  char lb_cord_x[12] = "CordX";
  lcd_writeString(lb_cord_x);

  lcd_setTextSize(3);
  lcd_setTextColor(YELLOW);
  char real_x[12];
  utoa((int)coord_X, real_x, 10);
  for(uint8_t i=0;i<4;i++)
  {
    if(real_x[i]<1)lcd_write('0');
  }
  lcd_writeString(real_x);

  lcd_setCursor((lcd_width>>2), (lcd_height>>1)+8);
  lcd_setTextSize(3);
  lcd_setTextColor(CYAN);
  char lb_cord_y[12] = "CordY";
  lcd_writeString(lb_cord_y);

  lcd_setTextSize(3);
  lcd_setTextColor(YELLOW);
  char real_y[12];
  utoa((int)coord_Y, real_y, 10);
  for(uint8_t i=0;i<4;i++)
  {
    if(real_y[i]<1)lcd_write('0');
  }
  lcd_writeString(real_y);

  lcd_setCursor((lcd_width>>2), (lcd_height>>1)+42);
  lcd_setTextSize(3);
  lcd_setTextColor(CYAN);
  char lb_cord_z[12] = "CordZ";
  lcd_writeString(lb_cord_z);

  lcd_setTextSize(3);
  lcd_setTextColor(YELLOW);
  char real_z[12];
  utoa((int)coord_Z, real_z, 10);
  for(uint8_t i=0;i<4;i++)
  {
    if(real_z[i]<1)lcd_write('0');
  }
  lcd_writeString(real_z);
}
