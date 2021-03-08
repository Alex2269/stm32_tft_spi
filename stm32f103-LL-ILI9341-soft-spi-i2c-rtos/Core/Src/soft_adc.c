/*
 * soft_adc, software i2c for stm32
 *      Library to use ADC1115 based ADC via software I2C bus.
 * 
 * see original code:
 *      https://github.com/electronicayciencia/wPi_soft_adc
 *      https://electronicayciencia.blogspot.com/
 *      Reinoso G.
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <math.h>
#include <gpio.h>

#include "soft_adc.h"
#include "soft_i2c.h"

void ads1115_usleep(uint32_t cycle_count) {
  uint32_t count=0;
  for(count=0;count<cycle_count;count++)
  {
    __asm volatile ("nop");
  }
}

adc_t *ads1115_create(GPIO_TypeDef* GPIOx, int scl, int sda, int addr) {

	i2c_t i2c = i2c_init(GPIOx, scl, sda);
	if ( !_ads1115_check(i2c, addr) ) return NULL;

	adc_t *adc = (adc_t*) malloc(sizeof(adc_t));
	adc->_addr         = addr;
    adc->_i2c.scl      = i2c.scl;
    adc->_i2c.sda      = i2c.sda;
    adc->_i2c.GPIOx    = i2c.GPIOx;

	adc->err           = 0;

	return adc;
}

void ads1115_destroy(adc_t *adc) {
	free(adc);
}

void ads1115_start(adc_t *adc)
{
  i2c_start(adc->_i2c);
}

void ads1115_stop(adc_t *adc)
{
  i2c_stop(adc->_i2c);
}

void ads1115_send_data(adc_t *adc, int data)
{
  i2c_send_byte(adc->_i2c, data);
}

void ads1115_set_single_channal(adc_t *adc, uint8_t channal)
{
  if(channal>3) channal=0; // must be 0,1,2,3 the number input
  channal = channal*0x10+0xC1;
  
  ads1115_start(adc);
  ads1115_send_data(adc, adc->_addr<<1|I2C_WRITE);
  ads1115_send_data(adc, 0x01);
  ads1115_send_data(adc, channal);
  ads1115_send_data(adc, 0x83);
  ads1115_stop(adc);
  
  ads1115_start(adc);
  ads1115_send_data(adc, adc->_addr<<1|I2C_WRITE);
  ads1115_send_data(adc, 0x00);
  ads1115_stop(adc);
}

void ads1115_set_diff_channal(adc_t *adc, uint8_t _input_select, uint8_t _gain)
{
  uint8_t set_gain = 0;

  if(_gain==0)set_gain = 0x00; // +/-6.144V range = Gain 2/3
  if(_gain==1)set_gain = 0x02; // +/-4.096V range = Gain 1
  if(_gain==2)set_gain = 0x04; // +/-2.048V range = Gain 2 (default)
  if(_gain==4)set_gain = 0x06; // +/-1.024V range = Gain 4
  if(_gain==8)set_gain = 0x08; // +/-0.512V range = Gain 8
  if(_gain==16)set_gain = 0x0A; // +/-0.256V range = Gain 16

  uint8_t in_sel = 0x00;
  if(_input_select<=1)in_sel = 0x00; // diff inputs A0-A1
  if(_input_select>1) in_sel = 0x30; // diff inputs A2-A3

  ads1115_start(adc); // Wire.begin(); // begin I2C
  ads1115_send_data(adc, adc->_addr<<1|I2C_WRITE);

  ads1115_send_data(adc, 0x01);
  ads1115_send_data(adc, 0x81|in_sel|set_gain);
  ads1115_send_data(adc, 0x83);
  ads1115_stop(adc);

  ads1115_start(adc);
  ads1115_send_data(adc, adc->_addr<<1|I2C_WRITE);
  ads1115_send_data(adc, 0x00);
  ads1115_stop(adc);
}

float get_diff_measure(adc_t *adc, uint8_t _gain)
{
  float result = 0;
  if(_gain==0)result = ads1115_get(adc)*(6.144/32768.0); // +/-6.144V range = Gain 2/3
  if(_gain==1)result = ads1115_get(adc)*(4.096/32768.0); // +/-4.096V range = Gain 1
  if(_gain==2)result = ads1115_get(adc)*(2.048/32768.0); // +/-2.048V range = Gain 2 (default)
  if(_gain==4)result = ads1115_get(adc)*(1.024/32768.0); // +/-1.024V range = Gain 4
  if(_gain==8)result = ads1115_get(adc)*(0.512/32768.0); // +/-0.512V range = Gain 8
  if(_gain==16)result = ads1115_get(adc)*(0.256/32768.0); // +/-0.256V range = Gain 16
  return result;
}

int16_t ads1115_get(adc_t *adc) 
{
  uint8_t buffer[2];

  ads1115_start(adc);

  i2c_send_byte(adc->_i2c, adc->_addr << 1 | I2C_READ);
  buffer[0] = i2c_read_byte(adc->_i2c);
  i2c_send_bit(adc->_i2c, I2C_ACK);
  buffer[1] = i2c_read_byte(adc->_i2c);
  i2c_send_bit(adc->_i2c, I2C_ACK);

  ads1115_stop(adc);

  int16_t val;
  val = buffer[0] << 8 | buffer[1];
  if (val > 32768) val = 0;

  return val;
}

/* check if ADC1115 driver is ready */
int _ads1115_check(i2c_t i2c, int addr) {
	i2c_start(i2c);

	int r = i2c_send_byte(
		i2c,
		addr << 1 | I2C_WRITE);
	
	if (r != I2C_ACK) return 0;
	
	i2c_stop(i2c);
	return 1;
}

