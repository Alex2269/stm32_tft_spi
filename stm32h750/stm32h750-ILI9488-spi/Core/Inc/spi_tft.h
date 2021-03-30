#ifndef _soft_spi_tft_h
#define _soft_spi_tft_h

#include "main.h"
#include "stm32h7xx_ll_gpio.h"

void SetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode);
void SetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);
void ResetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);

void spiWrite(uint8_t data);
void spiWrite16(uint16_t data);
void spiWrite32(uint32_t data);
void spiWriteFrame8(uint8_t * pData, uint32_t Size);

#endif // _soft_spi_tft_h

