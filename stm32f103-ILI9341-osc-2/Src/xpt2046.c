#include "xpt2046.h"

#define CMD_X 0xD1 // inquiry of coordinate X
#define CMD_Y 0x91 // inquiry of coordinate Y
#define CMD_Z 0xB1 // inquiry Z information on pressing

bitband_t touch_cs;

void pin_pen_on(void)
{
  //pin_mode(T_IRQ_GPIO_Port, T_IRQ_Pin, GPIO_MODE_INPUT);
}

void get_point(TS_Point *p)
{
  uint8_t counter=0;
  uint32_t sum_x = 0;
  uint32_t sum_y = 0;
  uint32_t sum_z = 0;

  soft_spi_2_begin();
  *(touch_cs)= 0;

  for(counter=0;counter<16;counter++)
  {
    soft_spi_2_write16(CMD_X);
    sum_x += soft_spi_2_transfer16(0) >> 3;

    soft_spi_2_write16(CMD_Y);
    sum_y += soft_spi_2_transfer16(0) >> 3;

    soft_spi_2_write16(CMD_Z);
    sum_z += soft_spi_2_transfer16(0) >> 3;
  }

  p->x=sum_x/counter;
  p->y=sum_y/counter;
  p->z=sum_z/counter;

  sum_x = 0;
  sum_y = 0;
  sum_z = 0;

  *(touch_cs)= 1;
  soft_spi_2_end();
}
