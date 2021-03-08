#ifndef _SOFT_INA_H
#define _SOFT_INA_H

#include <stdint.h>
#include "stdbool.h"
#include <inttypes.h>
// #include "stm32f1xx_hal.h"
#include "soft_i2c.h"

#define INA219_ADDRESS (0x40)
#define INA219_READ (0x01)
#define INA219_REG_CONFIG (0x00)
#define INA219_CONFIG_RESET (0x8000)
#define INA219_CONFIG_BVOLTAGERANGE_MASK (0x2000)

enum {
  INA219_CONFIG_BVOLTAGERANGE_16V = (0x0000),
  INA219_CONFIG_BVOLTAGERANGE_32V = (0x2000),
};

#define INA219_CONFIG_GAIN_MASK (0x1800)

enum {
  INA219_CONFIG_GAIN_1_40MV = (0x0000),
  INA219_CONFIG_GAIN_2_80MV = (0x0800),
  INA219_CONFIG_GAIN_4_160MV = (0x1000),
  INA219_CONFIG_GAIN_8_320MV = (0x1800),
};

#define INA219_CONFIG_BADCRES_MASK (0x0780)

enum {
  INA219_CONFIG_BADCRES_9BIT = (0x0000),
  INA219_CONFIG_BADCRES_10BIT = (0x0080),
  INA219_CONFIG_BADCRES_11BIT = (0x0100),
  INA219_CONFIG_BADCRES_12BIT = (0x0180),
};

#define INA219_CONFIG_SADCRES_MASK (0x0078)

enum {
  INA219_CONFIG_SADCRES_9BIT_1S_84US = (0x0000),
  INA219_CONFIG_SADCRES_10BIT_1S_148US = (0x0008),
  INA219_CONFIG_SADCRES_11BIT_1S_276US = (0x0010),
  INA219_CONFIG_SADCRES_12BIT_1S_532US = (0x0018),
  INA219_CONFIG_SADCRES_12BIT_2S_1060US = (0x0048),
  INA219_CONFIG_SADCRES_12BIT_4S_2130US = (0x0050),
  INA219_CONFIG_SADCRES_12BIT_8S_4260US = (0x0058),
  INA219_CONFIG_SADCRES_12BIT_16S_8510US = (0x0060),
  INA219_CONFIG_SADCRES_12BIT_32S_17MS = (0x0068),
  INA219_CONFIG_SADCRES_12BIT_64S_34MS = (0x0070),
  INA219_CONFIG_SADCRES_12BIT_128S_69MS = (0x0078),
};

#define INA219_CONFIG_MODE_MASK (0x0007)

enum {
  INA219_CONFIG_MODE_POWERDOWN,
  INA219_CONFIG_MODE_SVOLT_TRIGGERED,
  INA219_CONFIG_MODE_BVOLT_TRIGGERED,
  INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED,
  INA219_CONFIG_MODE_ADCOFF,
  INA219_CONFIG_MODE_SVOLT_CONTINUOUS,
  INA219_CONFIG_MODE_BVOLT_CONTINUOUS,
  INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS
};

#define INA219_REG_SHUNTVOLTAGE (0x01)
#define INA219_REG_BUSVOLTAGE (0x02)
#define INA219_REG_POWER (0x03)
#define INA219_REG_CURRENT (0x04)
#define INA219_REG_CALIBRATION (0x05)

typedef struct {
	/* ADC configuration */

	/* User options */
	int err;

	/* Internals */
	int _addr;
	i2c_t _i2c;
} ina_t;

void ina219_usleep(uint32_t cycle_count);
ina_t *ina219_create(GPIO_TypeDef* GPIOx, int scl, int sda, int addr);
int _ina219_check(i2c_t i2c, int addr);
void ina219_init(ina_t *ina);
void ina219_destroy(ina_t *ina);
void ina219_start(ina_t *ina);
void ina219_stop(ina_t *ina);
void ina219_send_data(ina_t *ina, int data);
uint8_t ina219_get_data(ina_t *ina);
void ina219_send_bit(ina_t *ina, int bit);
void ina219_WriteRegister(ina_t *ina, uint8_t reg, uint16_t value);
void ina219_ReadRegister(ina_t *ina, uint8_t reg, uint16_t *value);
void ina219_setCalibration_32V_2A(ina_t *ina);
void ina219_setCalibration_32V_1A(ina_t *ina);
void ina219_setCalibration_16V_400mA(ina_t *ina);
int16_t ina219_getBusVoltage_raw(ina_t *ina);
int16_t ina219_getShuntVoltage_raw(ina_t *ina);
int16_t ina219_getCurrent_raw(ina_t *ina);
int16_t ina219_getPower_raw(ina_t *ina);
float ina219_getShuntVoltage_mV(ina_t *ina);
float ina219_getBusVoltage_V(ina_t *ina);
float ina219_getCurrent_mA(ina_t *ina);
float ina219_getPower_mW(ina_t *ina);

#endif // _SOFT_INA_H



