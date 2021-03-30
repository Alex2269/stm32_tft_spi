#ifndef _soft_spi_tft_h
#define _soft_spi_tft_h

#include "stdbool.h"
#include "main.h"
#include "spi_tft.h"


// for cortex m3
// static inline
/* void SetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode)
{
  register uint32_t *pReg = (uint32_t *)((uint32_t)((uint32_t)(&GPIOx->CRL) + (Pin >> 24)));
  MODIFY_REG(*pReg, ((GPIO_CRL_CNF0 | GPIO_CRL_MODE0) << (POSITION_VAL(Pin) * 4U)), (Mode << (POSITION_VAL(Pin) * 4U)));
} */

// for cortex m4
// static inline
void SetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode)
{
  MODIFY_REG(GPIOx->MODER, (3U << (POSITION_VAL(Pin) * 2U)), (Mode << (POSITION_VAL(Pin) * 2U)));
}

// for cortex m7
// static inline
/* void SetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode)
{
  MODIFY_REG(GPIOx->MODER, ((Pin * Pin) * 3U), ((Pin * Pin) * Mode));
} */

// static inline
void SetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->BSRR, PinMask);
}

// static inline
void ResetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->BSRR, (PinMask << 16U));
}

#pragma GCC push_options
#pragma GCC optimize("O3")
void spiWrite(uint8_t data)
{
  uint16_t size = sizeof(data)<<3;
  bool flagSet;
  uint16_t mask = 1;

  for(uint16_t i = 1; i < size; i++, mask <<= 1);

  for(uint16_t i = 0; i < size; i++)
  {
    (mask & data) ? (flagSet = 1 ) : (flagSet = 0);
    data = data << 1;

    if(flagSet)  SetPin(MOSI_GPIO_Port, MOSI_Pin);
    else ResetPin(MOSI_GPIO_Port, MOSI_Pin);

    ResetPin(SCK_GPIO_Port, SCK_Pin);
    SetPin(SCK_GPIO_Port, SCK_Pin);
  }
}

void spiWrite16(uint16_t data)
{
  uint16_t size = sizeof(data)<<3;
  bool flagSet;
  uint16_t mask = 1;

  for(uint16_t i = 1; i < size; i++, mask <<= 1);

  for(uint16_t i = 0; i < size; i++)
  {
    (mask & data) ? (flagSet = 1 ) : (flagSet = 0);
    data = data << 1;

    if(flagSet)  SetPin(MOSI_GPIO_Port, MOSI_Pin);
    else ResetPin(MOSI_GPIO_Port, MOSI_Pin);

    ResetPin(SCK_GPIO_Port, SCK_Pin);
    SetPin(SCK_GPIO_Port, SCK_Pin);
  }
}

void spiWrite32(uint32_t data)
{
  uint16_t size = sizeof(data)<<3;
  bool flagSet;
  uint16_t mask = 1;

  for(uint16_t i = 1; i < size; i++, mask <<= 1);

  for(uint16_t i = 0; i < size; i++)
  {
    (mask & data) ? (flagSet = 1 ) : (flagSet = 0);
    data = data << 1;

    if(flagSet)  SetPin(MOSI_GPIO_Port, MOSI_Pin);
    else ResetPin(MOSI_GPIO_Port, MOSI_Pin);

    ResetPin(SCK_GPIO_Port, SCK_Pin);
    SetPin(SCK_GPIO_Port, SCK_Pin);
  }
}

void spiWriteFrame8(uint8_t * pData, uint32_t Size)
{
  while(Size--)
  {
    spiWrite(*pData);
    pData++;
  }
}

#pragma GCC pop_options

#endif // _soft_spi_tft_h

