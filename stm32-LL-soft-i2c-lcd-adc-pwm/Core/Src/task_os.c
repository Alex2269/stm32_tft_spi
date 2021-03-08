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
#include "soft_pcf8591.h"

#include "pins_ext.h"


// --
float U_power = 12.7; // power voltage
float resistance;
// --
float current_shunt_1;
float shunt_1 = 0.10; // resistance shunt_1
float correction_1 = +0.0;
#define R1 1000.0 // resistor feedback to ground
#define R2 22000.0 // resistor feedback
#define DELTA_GAIN_1 100.0 // calibration of an error
float gain_amplifier_1 = R2 / R1 * DELTA_GAIN_1;
// --
float current_shunt_2;
float shunt_2 = 63.0;
float correction_2 = +0.0;
#define R3 23000.0 // resistor feedback to ground
#define R4 47000.0 // resistor feedback
#define DELTA_GAIN_2 1.0 // calibration of an error
float gain_amplifier_2 = R4 / R3 * DELTA_GAIN_2;

int16_t nernst;
uint16_t virtuall_gnd;

extern lcd_t *lcd; // pointer to i2c object, for lcd module
extern adc_t *adc; // pointer to i2c object, for ads1115
extern ina_t *ina; // pointer to i2c object, for ina219
// extern pcf8591_t *pcf8591; // pointer to i2c object, for pcf8591

void show_ina219(lcd_t *lcd, ina_t *ina);

float adc_val_0, adc_val_1, adc_val_2, adc_val_3, adc_val_4, adc_val_5, adc_val_6, adc_temp, adc_vref;

float h_cycle=0;

void fill_buff(uint16_t i);
void write_graph(uint16_t i);

/**
 * it is clocked from system quanta of rtos
 * width base quants_base for example is 100 quanta
 */
void soft_pwm(GPIO_TypeDef *GPIOx, uint32_t Pin, uint16_t duty_cycle, uint16_t quants_base)
{
  osDelay(duty_cycle);
  LL_GPIO_SetOutputPin(GPIOx, Pin);

  osDelay(quants_base-duty_cycle);
  LL_GPIO_ResetOutputPin(GPIOx, Pin);
}

/**
 * it is clocked from system quanta of rtos
 * width base quants_base for example is 100 quanta
 */
void soft_pwm_heating(GPIO_TypeDef *GPIOx, uint32_t Pin, uint16_t duty_cycle, uint16_t quants_base)
{
  LL_GPIO_SetOutputPin(GPIOx, Pin);

  adc_val_0 = read_adc(ADC1, 0);
  adc_val_1 = read_adc(ADC1, 1);
  adc_val_2 = read_adc(ADC1, 2);
  adc_val_3 = read_adc(ADC1, 3);
  adc_val_4 = read_adc(ADC1, 4);
  adc_val_5 = read_adc(ADC1, 5);
  adc_val_6 = read_adc(ADC1, 6);

  /*
   * if exit from range
   * protection heating
   * setting the low level on gate
   */
  if( (resistance < 1.30) || (resistance > 40.5 ))
  {
    LL_GPIO_ResetOutputPin(GPIOx, Pin);
    h_cycle = 0;
  }
  osDelay(duty_cycle);

  LL_GPIO_ResetOutputPin(GPIOx, Pin);
  osDelay(quants_base-duty_cycle);
}

void DefaultTask(void const * argument)
{
  /* BEGIN StartDefaultTask */

  volatile uint16_t pumping=128;
  LL_TIM_OC_SetCompareCH4(TIM3, 128); // pin PB1 virtual ground 2.5 v
  /* Infinite loop */
  for(;;)
  {

    if(h_cycle > 170) h_cycle = 170;
    if(pumping < 30) pumping = 30;
    if(pumping > 220) pumping = 220;

    if(nernst>450) pumping += 2;
    if(nernst<450) pumping -= 2;

    // soft_pwm(STATUS_LED_GPIO_Port, STATUS_LED_Pin, h_cycle, 8);
    // soft_pwm(STATUS_LED_GPIO_Port, STATUS_LED_Pin, pumping, 255);
    soft_pwm_heating(STATUS_LED_GPIO_Port, STATUS_LED_Pin, h_cycle, 200);
    h_cycle += 0.1; // heating cycle duration

    // pcf8591_dac_set(pcf8591, pumping);
    LL_TIM_OC_SetCompareCH3(TIM3, pumping); // pin PB0 pump current

    // LL_GPIO_SetOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);

    osDelay(2);
  }
  /* END StartDefaultTask */
}

void Task02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  //for(float i = 0; i<100; i+=0.1)
  //{
    //soft_pwm(GPIOA, LL_GPIO_PIN_9, i, 255);
    // lcd_pos(lcd, 0,12);
    // lcd_printdigit(lcd, i);
    //osDelay(500);
  //}

//   for(uint16_t i = 0; i<511; i++)
//   {
//     LL_TIM_OC_SetCompareCH3(TIM3,i); // TIM3->CCR3=i;
//     LL_TIM_OC_SetCompareCH4(TIM3,511-i); // TIM3->CCR4=511-i;
//     for(uint16_t i = 0; i < 1000; i++){__asm__ volatile("nop\n\t"::);}
//   }
  osDelay(1);

  /* USER CODE END StartTask02 */
}

void Task03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  // read_adc(ADC1, 0); // heed for rank count
  // adc_val_1 = read_adc(ADC1, 1);
  // adc_val_2 = read_adc(ADC1, 2);
  // adc_val_3 = read_adc(ADC1, 3);
  // adc_val_4 = read_adc(ADC1, 4);
  // adc_val_5 = read_adc(ADC1, 5);
  // adc_val_6 = read_adc(ADC1, 6);
  /* USER CODE END StartTask03 */
}

// extern float pcf8591_value_in[4];

void Task04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
  // for(uint8_t i=0;i<4;i++) pcf8591_adc_get(pcf8591, i); // read all channel
  // lcd_pos(lcd, 0,  2); lcd_printdigit(lcd, pcf8591_value_in[0]);
  // lcd_pos(lcd, 1,  2); lcd_printdigit(lcd, pcf8591_value_in[1]);
  // lcd_pos(lcd, 2,  2); lcd_printdigit(lcd, pcf8591_value_in[2]);
  // lcd_pos(lcd, 0, 11); lcd_printdigit(lcd, pcf8591_value_in[3]);

  // lcd_pos(lcd, 0,  0); lcd_print(lcd, " "); lcd_printdigit(lcd, adc_val_1);
  // lcd_pos(lcd, 0, 10); lcd_print(lcd, " "); lcd_printdigit(lcd, adc_val_2);
  // lcd_pos(lcd, 1,  0); lcd_print(lcd, " "); lcd_printdigit(lcd, adc_val_3);
  // lcd_pos(lcd, 1, 10); lcd_print(lcd, " "); lcd_printdigit(lcd, adc_val_4);
  // lcd_pos(lcd, 2,  0); lcd_print(lcd, " "); lcd_printdigit(lcd, adc_val_5);
  // lcd_pos(lcd, 2, 10); lcd_print(lcd, " "); lcd_printdigit(lcd, adc_val_6);

  lcd_clear(lcd);
  osDelay(100);
  // lcd_pos(lcd, 0,  0); lcd_printf(lcd, "%d", (uint16_t)adc_val_1);
  // lcd_pos(lcd, 0, 10); lcd_printf(lcd, "%d", (uint16_t)adc_val_2);
  lcd_pos(lcd, 1,  0); lcd_printf(lcd, "%d", (uint16_t)adc_val_3);
  lcd_pos(lcd, 1, 10); lcd_printf(lcd, "%d", (uint16_t)adc_val_4);
  lcd_pos(lcd, 2,  0); lcd_printf(lcd, "%d", (uint16_t)adc_val_5);
  // lcd_pos(lcd, 2, 10); lcd_printf(lcd, "%d", (uint16_t)adc_val_6);

  // lcd_pos(lcd, 3,  0); lcd_printf(lcd, "vr %d", (uint16_t)adc_vref);
  // lcd_pos(lcd, 3,  8); lcd_printf(lcd, "te %d", (uint32_t)adc_temp);

  current_shunt_1 = ((float)adc_val_6 / gain_amplifier_1) / shunt_1 ; // проходящий ток через шунт1
  resistance = U_power / current_shunt_1 + correction_1; // сопротивление нагревателя
  // lcd_pos(lcd, 3,  8); lcd_printf(lcd, "te %d", (uint32_t)resistance);
  lcd_pos(lcd, 2, 10); lcd_printdigit(lcd, resistance);

  virtuall_gnd = adc_val_4;
  nernst = adc_val_5-virtuall_gnd; // напряжение нернста
  lcd_pos(lcd, 3,  0); lcd_printf(lcd, "%d", nernst);
  lcd_pos(lcd, 3, 10); lcd_printdigit(lcd, nernst);

  current_shunt_2 = adc_val_3-adc_val_2;
  current_shunt_2 = current_shunt_2 / gain_amplifier_2 / shunt_2; // проходящий ток через шунт2
  lcd_pos(lcd, 0,  0); lcd_printdigit(lcd, current_shunt_2);

  // show_ina219(lcd, ina);
  // show_ads1115(lcd, adc);

  // ILI9341_setCursor(10,  10); ILI9341_draw_value(adc_val_1);
  // ILI9341_setCursor(10,  45); ILI9341_draw_value(adc_val_2);
  // ILI9341_setCursor(10,  80); ILI9341_draw_value(adc_val_3);
  // ILI9341_setCursor(10, 115); ILI9341_draw_value(adc_val_4);
  // ILI9341_setCursor(10, 150); ILI9341_draw_value(adc_val_5);
  // ILI9341_setCursor(10, 185); ILI9341_draw_value(adc_val_6);

  osDelay(10000);
  /* USER CODE END StartTask04 */
}
