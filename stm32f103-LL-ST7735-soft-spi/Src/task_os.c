#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "ST7735.h"
#include "adc_cfg.h"
#include "task_os.h"

uint16_t adc_val_0, adc_val_1, adc_val_2, adc_val_3, adc_val_4, adc_val_5,adc_val_6;

void fill_buff(uint16_t i);
void write_graph(uint16_t i);

void DefaultTask(void const * argument)
{
  /* BEGIN StartDefaultTask */
  osDelay(2);
  for(float i = 2; i<100 ; i++)
  {
    LL_GPIO_SetOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
    osDelay(100-i);
    LL_GPIO_ResetOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
    osDelay(i);
  }
  for(float i = 100; i>=2 ; i--)
  {
    LL_GPIO_SetOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
    osDelay(100-i);
    LL_GPIO_ResetOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
    osDelay(i);
  }
  /* END StartDefaultTask */
}

void Task02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_14);
  osDelay(25);
  LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_14);
  osDelay(5);
  /* USER CODE END StartTask02 */
}

void Task03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  read_adc(ADC1, 0); // heed for rank count
  adc_val_1 = read_adc(ADC1, 1);
  adc_val_2 = read_adc(ADC1, 2);
  adc_val_3 = read_adc(ADC1, 3);
  adc_val_4 = read_adc(ADC1, 4);
  adc_val_5 = read_adc(ADC1, 5);
  adc_val_6 = read_adc(ADC1, 6);
  /* USER CODE END StartTask03 */
}

void Task04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */

  /* USER CODE END StartTask04 */
}
