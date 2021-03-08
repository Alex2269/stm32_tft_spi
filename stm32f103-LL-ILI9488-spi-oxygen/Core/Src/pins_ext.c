#include "pins_ext.h"

void tic_delay(uint32_t cycle_count)
{
  uint32_t count=0;
  for(count=0;count<cycle_count;count++)
  {
    __asm volatile ("nop");
  }
}

void SetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->BSRR, (PinMask >> GPIO_PIN_MASK_POS) & 0x0000FFFFU); 
}

void ResetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->BRR, (PinMask >> GPIO_PIN_MASK_POS) & 0x0000FFFFU);
}

void TogglePin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->ODR, READ_REG(GPIOx->ODR) ^ ((PinMask >> GPIO_PIN_MASK_POS) & 0x0000FFFFU));
}

uint32_t IsInputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return (READ_BIT(GPIOx->IDR, (PinMask >> GPIO_PIN_MASK_POS) & 0x0000FFFFU) == ((PinMask >> GPIO_PIN_MASK_POS) & 0x0000FFFFU));
}

uint32_t pin_read(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
  pin_in(GPIOx, GPIO_Pin);
  if(1  == (IsInputPinSet(GPIOx, GPIO_Pin)))
  {
    return 1UL;
  }
  else
  {
    return 0UL;
  }
  // HAL_GPIO_DeInit(GPIOx, GPIO_Pin);
  // LL_GPIO_DeInit(GPIOx);
}

void pin_write(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, int state)
{
  pin_out(GPIOx, GPIO_Pin);

  if(state)
  {
    // GPIOx->ODR |= GPIO_Pin;
    SetOutputPin(GPIOx, GPIO_Pin);
  }
  else
  {
    // GPIOx->ODR &= ~GPIO_Pin;
    ResetOutputPin(GPIOx, GPIO_Pin);
  }
}

void pin_toggle(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
  pin_out(GPIOx, GPIO_Pin);

  // GPIOx->ODR ^= GPIO_Pin;
  TogglePin(GPIOx, GPIO_Pin);
}

void pin_pulse_hi_low(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, uint32_t delay)
{
  pin_out(GPIOx, GPIO_Pin);

  // GPIOx->ODR |= GPIO_Pin;
  SetOutputPin(GPIOx, GPIO_Pin);
  tic_delay(delay);
  // GPIOx->ODR &= ~GPIO_Pin;
  ResetOutputPin(GPIOx, GPIO_Pin);
  tic_delay(delay);
}

void pin_pulse_low_hi(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, uint32_t delay)
{
  pin_out(GPIOx, GPIO_Pin);

  // GPIOx->ODR &= ~GPIO_Pin;
  ResetOutputPin(GPIOx, GPIO_Pin);
  tic_delay(delay);
  // GPIOx->ODR |= GPIO_Pin;
  SetOutputPin(GPIOx, GPIO_Pin);
  tic_delay(delay);
}

void pin_in(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
  /* GPIO Ports Clock Enable */
  if(GPIOx==GPIOA) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  if(GPIOx==GPIOB) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  if(GPIOx==GPIOC) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
  if(GPIOx==GPIOD) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /**/
  GPIO_InitStruct.Pin = GPIO_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void pin_out(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
  /* GPIO Ports Clock Enable */
  if(GPIOx==GPIOA) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  if(GPIOx==GPIOB) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  if(GPIOx==GPIOC) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
  if(GPIOx==GPIOD) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  // LL_GPIO_ResetOutputPin(GPIOx, GPIO_Pin);

  /**/
  GPIO_InitStruct.Pin = GPIO_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOx, &GPIO_InitStruct);

  LL_GPIO_SetPinMode(GPIOx, GPIO_Pin, LL_GPIO_MODE_OUTPUT);
}

