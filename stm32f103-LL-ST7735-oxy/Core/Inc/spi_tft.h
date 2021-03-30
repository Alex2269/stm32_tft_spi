#ifndef _soft_spi_tft_h
#define _soft_spi_tft_h

#include "main.h"
#include "stm32f1xx_ll_gpio.h"

void SetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);
void ResetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);

void spiWrite(uint8_t data);
void spiWrite16(uint16_t data);

#endif // _soft_spi_tft_h

