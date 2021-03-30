#ifndef _SOFT_ADC_H
#define _SOFT_ADC_H

#include <stdint.h>
#include <inttypes.h>
// #include "stm32f1xx_hal.h"
#include "soft_i2c.h"

#define ADC_ENABLED 4
#define ADC_ERR_I2C 1

typedef struct {
	/* ADC configuration */

	/* User options */
	int err;

	/* Internals */
	int _addr;
	i2c_t _i2c;
} adc_t;

void ads1115_usleep(uint32_t cycle_count);
adc_t *ads1115_create(GPIO_TypeDef* GPIOx, int scl, int sda, int addr);
void ads1115_destroy(adc_t *adc);
void ads1115_start(adc_t *adc);
void ads1115_stop(adc_t *adc);
void ads1115_send_data(adc_t *adc, int data);
void ads1115_set_single_channal(adc_t *adc, uint8_t channal);
void ads1115_set_diff_channal(adc_t *adc, uint8_t _input_select, uint8_t _gain);
float get_diff_measure(adc_t *adc, uint8_t _gain);
int16_t ads1115_get(adc_t *adc);
int _ads1115_check(i2c_t i2c, int addr);

#endif

