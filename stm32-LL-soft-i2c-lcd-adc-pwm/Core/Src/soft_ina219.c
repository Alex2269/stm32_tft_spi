#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <math.h>
#include <gpio.h>

#include "soft_ina219.h"
#include "soft_i2c.h"

uint8_t ina219_i2caddr;
uint32_t ina219_calValue;
uint32_t ina219_currentDivider_mA;
float ina219_powerMultiplier_mW;


void ina219_usleep(uint32_t cycle_count)
{
  uint32_t count=0;
  for(count=0;count<cycle_count;count++)
  {
    __asm volatile ("nop");
  }
}

ina_t *ina219_create(GPIO_TypeDef* GPIOx, int scl, int sda, int addr)
{
  i2c_t i2c = i2c_init(GPIOx, scl, sda);

  if ( !_ina219_check(i2c, addr) ) return NULL;

  ina_t *ina = (ina_t*) malloc(sizeof(ina_t));

  ina->_addr = addr;
  ina->_i2c.scl = i2c.scl;
  ina->_i2c.sda = i2c.sda;
  ina->_i2c.GPIOx = i2c.GPIOx;
  ina->err = 0;

  ina219_init(ina);
  return ina;
}

/* check if INA219 driver is ready */
int _ina219_check(i2c_t i2c, int addr)
{
  i2c_start(i2c);
  int r = i2c_send_byte(i2c, addr << 1 | I2C_WRITE);
  if (r != I2C_ACK) return 0;
  i2c_stop(i2c);

  return 1;
}

void ina219_init(ina_t *ina)
{
  ina219_setCalibration_32V_2A(ina);
}

void ina219_destroy(ina_t *ina)
{
  free(ina);
}

void ina219_start(ina_t *ina)
{
  i2c_start(ina->_i2c);
}

void ina219_stop(ina_t *ina)
{
  i2c_stop(ina->_i2c);
}

void ina219_send_data(ina_t *ina, int data)
{
  i2c_send_byte(ina->_i2c, data);
}

uint8_t ina219_get_data(ina_t *ina)
{
  return i2c_read_byte(ina->_i2c);
}

void ina219_send_bit(ina_t *ina, int bit)
{
  i2c_send_bit(ina->_i2c, bit);
}

void ina219_WriteRegister(ina_t *ina, uint8_t reg, uint16_t value)
{
  ina219_start(ina);
  ina219_send_data(ina, ina->_addr<<1|I2C_WRITE);
  ina219_send_data(ina, reg);
  ina219_send_data(ina, (value >> 8) & 0xFF); // Upper 8-bits
  ina219_send_data(ina, value & 0xFF); // Lower 8-bits
  ina219_stop(ina);
}

void ina219_ReadRegister(ina_t *ina, uint8_t reg, uint16_t *value)
{
  ina219_start(ina);
  ina219_send_data(ina, ina->_addr << 1 | I2C_WRITE);
  ina219_send_data(ina, reg);
  ina219_stop(ina);

  ina219_start(ina);
  ina219_send_data(ina, ina->_addr << 1 | I2C_READ);

  uint8_t buffer[2];
  buffer[0] = ina219_get_data(ina); ina219_send_bit(ina, I2C_ACK);
  buffer[1] = ina219_get_data(ina); ina219_send_bit(ina, I2C_ACK);
  ina219_stop(ina);

  int16_t val;
  val = (buffer[0] << 8) | (buffer[1] & 0xff);

  *value = val;
}

/*!
**  @brief  Configures to INA219 to be able to measure up to 32V and 2A
**          of current.  Each unit of current corresponds to 100uA, and
**          each unit of power corresponds to 2mW. Counter overflow
**          occurs at 3.2A.
**  @note   These calculations assume a 0.1 ohm resistor is present
*/
void ina219_setCalibration_32V_2A(ina_t *ina)
{
  ina219_calValue = 4096;
  ina219_currentDivider_mA = 10;
  ina219_powerMultiplier_mW = 2;

  ina219_WriteRegister(ina, INA219_REG_CALIBRATION, ina219_calValue);

  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

  ina219_WriteRegister(ina, INA219_REG_CONFIG, config);
}

/*!
**  @brief  Configures to INA219 to be able to measure up to 32V and 1A
**          of current.  Each unit of current corresponds to 40uA, and each
**          unit of power corresponds to 800�W. Counter overflow occurs at
**          1.3A.
**  @note   These calculations assume a 0.1 ohm resistor is present
*/
void ina219_setCalibration_32V_1A(ina_t *ina)
{
  ina219_calValue = 10240;
  ina219_currentDivider_mA = 25;
  ina219_powerMultiplier_mW = 0.8f;

  ina219_WriteRegister(ina, INA219_REG_CALIBRATION, ina219_calValue);

  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

  ina219_WriteRegister(ina, INA219_REG_CONFIG, config);
}

/*!
**  @brief set device to alibration which uses the highest precision for
**     current measurement (0.1mA), at the expense of
**     only supporting 16V at 400mA max.
*/
void ina219_setCalibration_16V_400mA(ina_t *ina)
{
  ina219_calValue = 8192;
  ina219_currentDivider_mA = 20;
  ina219_powerMultiplier_mW = 1.0f;

  ina219_WriteRegister(ina, INA219_REG_CALIBRATION, ina219_calValue);

  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
                    INA219_CONFIG_GAIN_1_40MV |
                    INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

  ina219_WriteRegister(ina, INA219_REG_CONFIG, config);
}

/*!
** @brief Gets the raw bus voltage (16-bit signed integer, so +-32767)
** @return the raw bus voltage reading
*/
int16_t ina219_getBusVoltage_raw(ina_t *ina)
{
  uint16_t value;
  ina219_ReadRegister(ina, INA219_REG_BUSVOLTAGE, &value);

  // Shift to the right 3 to drop CNVR and OVF and multiply by LSB
  return (int16_t)((value >> 3) * 4);
}

/*!
** @brief Gets the raw shunt voltage (16-bit signed integer, so +-32767)
** @return the raw shunt voltage reading
*/
int16_t ina219_getShuntVoltage_raw(ina_t *ina)
{
  uint16_t value;
  ina219_ReadRegister(ina, INA219_REG_SHUNTVOLTAGE, &value);
  return (int16_t)value;
}

/*!
** @brief Gets the raw current value (16-bit signed integer, so +-32767)
** @return the raw current reading
*/
int16_t ina219_getCurrent_raw(ina_t *ina)
{
  uint16_t value;

  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  ina219_WriteRegister(ina, INA219_REG_CALIBRATION, ina219_calValue);

  // Now we can safely read the CURRENT register!
  ina219_ReadRegister(ina, INA219_REG_CURRENT, &value);

  return (int16_t)value;
}

/*!
** @brief Gets the raw power value (16-bit signed integer, so +-32767)
** @return raw power reading
*/
int16_t ina219_getPower_raw(ina_t *ina)
{
  uint16_t value;

  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  ina219_WriteRegister(ina, INA219_REG_CALIBRATION, ina219_calValue);

  // Now we can safely read the POWER register!
  ina219_ReadRegister(ina, INA219_REG_POWER, &value);

  return (int16_t)value;
}

/*!
** @brief Gets the shunt voltage in mV (so +-327mV)
** @return the shunt voltage converted to millivolts
*/
float ina219_getShuntVoltage_mV(ina_t *ina)
{
  int16_t value;
  value = ina219_getShuntVoltage_raw(ina);
  return value * 0.01;
}

/*!
** @brief Gets the shunt voltage in volts
** @return the bus voltage converted to volts
*/
float ina219_getBusVoltage_V(ina_t *ina)
{
  int16_t value = ina219_getBusVoltage_raw(ina);
  return value * 0.001;
}

/*!
** @brief Gets the current value in mA, taking into account the
** config settings and current LSB
** @return the current reading convereted to milliamps
*/
float ina219_getCurrent_mA(ina_t *ina)
{
  float valueDec = ina219_getCurrent_raw(ina);
  valueDec /= ina219_currentDivider_mA;
  return valueDec;
}

/*!
** @brief Gets the power value in mW, taking into account the
** config settings and current LSB
** @return power reading converted to milliwatts
**/
float ina219_getPower_mW(ina_t *ina)
{
  float valueDec = ina219_getPower_raw(ina);
  valueDec *= ina219_powerMultiplier_mW;
  return valueDec;
}

