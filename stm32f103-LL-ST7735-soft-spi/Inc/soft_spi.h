#ifndef __soft_spi_H
#define __soft_spi_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "stm32f1xx_ll_gpio.h"

void SetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);
void ResetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);
void lcd_reset(void);
void spiWrite(uint8_t data);
void spiWrite16(uint16_t data);
void writeCommand(uint8_t c);
void writeData(uint8_t c);
void writeData16(uint16_t c);
void write_data_array(uint8_t* buff);
void write_data_array16(uint16_t* buff);


#endif // __soft_spi_H
