#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "ILI9341.h"
#include "adc_cfg.h"
#include "task_os.h"

#include "soft_lcd.h"
#include "soft_adc.h"
#include "soft_ina219.h"
#include "pins_ext.h"

extern lcd_t *lcd; // pointer to i2c object, for lcd module
// extern adc_t *adc; // pointer to i2c object, for ads1115
// extern ina_t *ina; // pointer to i2c object, for ina219

uint16_t adc_val_0, adc_val_1, adc_val_2, adc_val_3, adc_val_4, adc_val_5,adc_val_6;

void fill_buff(uint16_t i);
void write_graph(uint16_t i);

/**
 * it is clocked from system quanta of rtos
 * width base quants_base for example is 100 quanta
 */
void soft_pwm(GPIO_TypeDef *GPIOx, uint32_t Pin, uint16_t duty_cycle, uint16_t quants_base)
{
  {
    osDelay(duty_cycle);
    LL_GPIO_SetOutputPin(GPIOx, Pin);
    osDelay(quants_base-duty_cycle);
    LL_GPIO_ResetOutputPin(GPIOx, Pin);
  }
}

void DefaultTask(void const * argument)
{
  /* BEGIN StartDefaultTask */
  osDelay(1);
 
  for(int16_t duty_cycle = 0; duty_cycle<=100 ; duty_cycle++)
  {
    soft_pwm(STATUS_LED_GPIO_Port, STATUS_LED_Pin, duty_cycle, 100);
  }
  osDelay(1);
  for(int16_t duty_cycle = 100; duty_cycle>0 ; duty_cycle--)
  {
    soft_pwm(STATUS_LED_GPIO_Port, STATUS_LED_Pin, duty_cycle, 100);
  }
  LL_GPIO_SetOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
  /* END StartDefaultTask */
}

void Task02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  for(float i = 0; i<100; i+=0.1)
  {
    lcd_pos(lcd, 0,12);
    lcd_printdigit(lcd, i);
    osDelay(5000);
  }
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
  ILI9341_setTextBgColor(ILI9341_BLACK);

  ILI9341_setTextSize(3);
  ILI9341_setTextColor(ILI9341_YELLOW);

  ILI9341_setCursor(10,  10); ILI9341_draw_value(adc_val_1);
  ILI9341_setCursor(10,  45); ILI9341_draw_value(adc_val_2);
  ILI9341_setCursor(10,  80); ILI9341_draw_value(adc_val_3);
  ILI9341_setCursor(10, 115); ILI9341_draw_value(adc_val_4);
  ILI9341_setCursor(10, 150); ILI9341_draw_value(adc_val_5);
  ILI9341_setCursor(10, 185); ILI9341_draw_value(adc_val_6);
  /* USER CODE END StartTask04 */
}
