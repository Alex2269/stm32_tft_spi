#ifndef _pcf8591_H
#define _pcf8591_H

#include <stdint.h>
#include <inttypes.h>
// #include "stm32f1xx_hal.h"
#include "soft_i2c.h"


typedef struct {
	/* ADC configuration */

	/* User options */
	int err;

	/* Internals */
	int _addr;
	i2c_t _i2c;
} pcf8591_t;

void pcf8591_usleep(uint32_t cycle_count);
pcf8591_t *pcf8591_create(GPIO_TypeDef* GPIOx, int scl, int sda, int addr);
int _pcf8591_check(i2c_t i2c, int addr);
void pcf8591_init(pcf8591_t *pcf8591);
void pcf8591_destroy(pcf8591_t *pcf8591);
void pcf8591_start(pcf8591_t *pcf8591);
void pcf8591_stop(pcf8591_t *pcf8591);
void pcf8591_beginTransmission(pcf8591_t *pcf8591);
void pcf8591_requestFrom(pcf8591_t *pcf8591);
void pcf8591_send_data(pcf8591_t *pcf8591, int data);
uint8_t pcf8591_get_data(pcf8591_t *pcf8591);
void pcf8591_send_bit(pcf8591_t *pcf8591, int bit);
int pcf8591_read_bit(pcf8591_t *pcf8591);
void pcf8591_dac_set(pcf8591_t *pcf8591, uint8_t val);
void pcf8591_adc_get(pcf8591_t *pcf8591, uint8_t channel);

#endif // _pcf8591_H
