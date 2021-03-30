/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __pins_ext_H
#define __pins_ext_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "main.h"

void tic_delay(uint32_t cycle_count);
void SetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);
void ResetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);
void TogglePin(GPIO_TypeDef *GPIOx, uint32_t PinMask);
uint32_t IsInputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask);
uint32_t pin_read(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
void pin_write(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, int state);
void pin_toggle(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
void pin_pulse_hi_low(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, uint32_t delay);
void pin_pulse_low_hi(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, uint32_t delay);
void pin_in(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
void pin_out(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);

#ifdef __cplusplus
}
#endif
#endif /* __pins_ext_H */

