#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <math.h>
#include <gpio.h>

#include "soft_pcf8591.h"
#include "soft_i2c.h"

#define PCF8591_ADDR (0x48)
#define PCF8591_IN0 0x00
#define PCF8591_IN1 0x01
#define PCF8591_IN2 0x02
#define PCF8591_IN3 0x03
#define PCF8591_INALL 0x44 // all channel
#define DAC 0x40

float pcf8591_value_in[4];

void pcf8591_usleep(uint32_t cycle_count)
{
  uint32_t count=0;
  for(count=0;count<cycle_count;count++)
  {
    __asm volatile ("nop");
  }
}

pcf8591_t *pcf8591_create(GPIO_TypeDef* GPIOx, int scl, int sda, int addr)
{
  i2c_t i2c = i2c_init(GPIOx, scl, sda);

  if ( !_pcf8591_check(i2c, addr) ) return NULL;

  pcf8591_t *pcf8591 = (pcf8591_t*) malloc(sizeof(pcf8591_t));

  pcf8591->_addr = addr;
  pcf8591->_i2c.scl = i2c.scl;
  pcf8591->_i2c.sda = i2c.sda;
  pcf8591->_i2c.GPIOx = i2c.GPIOx;
  pcf8591->err = 0;

  pcf8591_init(pcf8591);
  return pcf8591;
}

/* check if INA219 driver is ready */
int _pcf8591_check(i2c_t i2c, int addr)
{
  i2c_start(i2c);
  int r = i2c_send_byte(i2c, addr << 1 | I2C_WRITE);
  if (r != I2C_ACK) return 0;
  i2c_stop(i2c);

  return 1;
}

void pcf8591_init(pcf8591_t *pcf8591)
{
  ;
}

void pcf8591_destroy(pcf8591_t *pcf8591)
{
  free(pcf8591);
}

void pcf8591_start(pcf8591_t *pcf8591)
{
  i2c_start(pcf8591->_i2c);
}

void pcf8591_stop(pcf8591_t *pcf8591)
{
  i2c_stop(pcf8591->_i2c);
}

void pcf8591_send_data(pcf8591_t *pcf8591, int data)
{
  i2c_send_byte(pcf8591->_i2c, data);
}

uint8_t pcf8591_get_data(pcf8591_t *pcf8591)
{
  return i2c_read_byte(pcf8591->_i2c);
}

void pcf8591_send_bit(pcf8591_t *pcf8591, int bit)
{
  i2c_send_bit(pcf8591->_i2c, bit);
}

int pcf8591_read_bit(pcf8591_t *pcf8591)
{
  return i2c_read_bit(pcf8591->_i2c);
}

void pcf8591_beginTransmission(pcf8591_t *pcf8591)
{
  pcf8591_start(pcf8591); // Wire.begin(); // begin I2C
  pcf8591_send_data(pcf8591, pcf8591->_addr<<1|I2C_WRITE); // launch PCF8591
}

void pcf8591_requestFrom(pcf8591_t *pcf8591)
{
  pcf8591_start(pcf8591); // Wire.begin(); // begin I2C
  pcf8591_send_data(pcf8591, pcf8591->_addr<<1|I2C_READ); // launch PCF8591
}

void pcf8591_dac_set(pcf8591_t *pcf8591, uint8_t val)
{
  pcf8591_beginTransmission(pcf8591); // launch PCF8591
  pcf8591_send_data(pcf8591, PCF8591_INALL); // PCF8591_IN0 control byte
  pcf8591_send_data(pcf8591, val); // значение, которое будет выдано на AOUT
  pcf8591_stop(pcf8591); // закончили передачу
}

void pcf8591_adc_get(pcf8591_t *pcf8591, uint8_t channel)
{
  if(channel>PCF8591_IN3) return;
  pcf8591_beginTransmission(pcf8591); // launch PCF8591
  pcf8591_send_data(pcf8591, PCF8591_INALL); // PCF8591_IN0 control byte
  pcf8591_stop(pcf8591); // закончили передачу

  pcf8591_requestFrom(pcf8591); // launch to read PCF8591
  pcf8591_value_in[channel] = pcf8591_get_data(pcf8591);
  pcf8591_stop(pcf8591); // закончили передачу
}
