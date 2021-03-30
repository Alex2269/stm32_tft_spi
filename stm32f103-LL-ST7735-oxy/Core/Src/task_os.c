#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "ST7735.h"
#include "adc_cfg.h"
#include "task_os.h"

#include "soft_lcd.h"
#include "soft_adc.h"
#include "soft_ina219.h"
#include "soft_pcf8591.h"

#include "pins_ext.h"

#include "time.h"
#include "pid.h"

struct pid_controller_t pid_pump;
struct pid_controller_t pid_heater;

// --
float U_power = 12600; // power voltage
float resistance;
// --
float current_shunt_1;
float shunt_1 = 0.010; // resistance shunt_1
float correction_1 = +0.0;
#define R1 2575.0 // resistor feedback to ground
#define R2 47000.0 // resistor feedback
#define DELTA_GAIN_1 1.0 // calibration of an error
// --
float gain_amplifier_1 = R2 / R1 * DELTA_GAIN_1;
// --
volatile float current_shunt_2;
float shunt_2 = 63.0;
float correction_2 = +0.0;
#define R3 43000.0 // resistor feedback to ground
#define R4 100000.0 // resistor feedback
#define DELTA_GAIN_2 1.0 // calibration of an error
float gain_amplifier_2 = R4 / R3 * DELTA_GAIN_2;

// --
volatile float pumping=250;
volatile float nernst;
// --

extern lcd_t *lcd; // pointer to i2c object, for lcd module
extern adc_t *adc; // pointer to i2c object, for ads1115
extern ina_t *ina; // pointer to i2c object, for ina219
// extern pcf8591_t *pcf8591; // pointer to i2c object, for pcf8591

void show_ina219(lcd_t *lcd, ina_t *ina);

float adc_val_0, adc_val_1, adc_val_2, adc_val_3, adc_val_4, adc_val_5, adc_val_6, adc_temp, adc_vref;
float h_cycle=0;

/**
 * it is clocked from system quanta of rtos
 * width base quants_base for example is 100 quanta
 */
void soft_pwm(GPIO_TypeDef *GPIOx, uint32_t Pin, uint16_t duty_cycle, uint16_t quants_base)
{
  osDelay(duty_cycle);
  LL_GPIO_ResetOutputPin(GPIOx, Pin);

  osDelay(quants_base-duty_cycle);
  LL_GPIO_SetOutputPin(GPIOx, Pin);
}

/**
 * it is clocked from system quanta of rtos
 * width base quants_base for example is 100 quanta
 */
void soft_pwm_heating(GPIO_TypeDef *GPIOx, uint32_t Pin, uint16_t duty_cycle, uint16_t quants_base)
{
  LL_GPIO_SetOutputPin(GPIOx, Pin); // enable heating pin

  osDelay(2); // stabilization current heating

  /*
   * Measured resistance heater
   */
  adc_val_0 = read_adc(ADC1, 0);
  // adc_val_0 = adc_get_value(ADC1, 0) * 3300.0 / 4096.0;
  current_shunt_1 = ((float)adc_val_0 / gain_amplifier_1) / shunt_1 ; // Current through shunt 1
  resistance = U_power / current_shunt_1 + correction_1; // resistance heating
  if(resistance>99.0) resistance=99.0;
  /*
   * if exit from range
   * protection heating
   * setting the low level on gate mosfet
   */
  if( (resistance < 1.05) || (resistance > 20.5 ))
  {
    LL_GPIO_ResetOutputPin(GPIOx, Pin);
    h_cycle = 0;
  }
  osDelay(duty_cycle);

  LL_GPIO_ResetOutputPin(GPIOx, Pin); // disable heating pin
  osDelay(quants_base-duty_cycle);

  /*
   * The rest is measured when the heater is turned off
   */
  adc_val_2 = read_adc(ADC1, 2); // read adc channel 2, pin PA2 for current pump value
  adc_val_1 = read_adc(ADC1, 1); // read adc channel 1, pin PA1 for nernst different voltage
  adc_val_3 = read_adc(ADC1, 3); // read adc channel 3, pin PA3 for virtual ground value
}

void DefaultTask(void const * argument)
{
  /* BEGIN StartDefaultTask */

  /* Infinite loop */
  for(;;)
  {

    if(h_cycle > 65) h_cycle = 65;

    soft_pwm_heating(STATUS_LED_GPIO_Port, STATUS_LED_Pin, h_cycle, 80);
    h_cycle += 0.01; // heating cycle duration

    if(pumping < 25) pumping = 25; // min duty_cycle pwm pump current
    if(pumping > 475) pumping = 475; // max duty_cycle pwm pump current
    if(nernst>450) pumping -= 1;
    if(nernst<450) pumping += 1;

    // LL_GPIO_SetOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
    
    // soft_pwm_heating(STATUS_LED_GPIO_Port, STATUS_LED_Pin, pid_get_output(&pid_heater), 80);
    soft_pwm_heating(STATUS_LED_GPIO_Port, STATUS_LED_Pin, h_cycle, 100);

    osDelay(5);
  }
  /* END StartDefaultTask */
}

void Task02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */

  // INIT PID_CONTROLLER
  static volatile float input = 0.0f;
  pid_init_controller(&pid_heater);
  pid_set_current_time(&pid_heater, get_time_ms());
  pid_set_sample_time(&pid_heater, 1); // update every 10 msec
  // pid_set_tunings(&pid_heater, 0.1f, 0.3f, 0.1f);
  pid_set_tunings(&pid_heater, 0.05f, 0.15f, 0.05f);
  pid_set_output_limits(&pid_heater, 0.0f, 50.0f);
  pid_set_setpoint(&pid_heater, 2.45); // задаем желаемое значение resistance
  // END INIT PID_CONTROLLER

  /* Infinite loop */
  for(;;)
  {
    /* USER CODE BEGIN StartTask02 */
    input = resistance;
    pid_compute(&pid_heater, get_time_ms(), input); // вычисление коэфициентов
    osDelay(5);
    /* USER CODE END StartTask02 */
  }
}

void Task03(void const * argument)
{
  /* USER CODE init StartTask03 */

  // INIT PID_CONTROLLER
  static volatile float input = 0.0f;
  pid_init_controller(&pid_pump);
  pid_set_current_time(&pid_pump, get_time_ms());
  pid_set_sample_time(&pid_pump, 10); // update every 10 msec
  pid_set_tunings(&pid_pump, 0.05f, 0.15f, 0.05f);
  pid_set_output_limits(&pid_pump, -1000.0f, 1000.0f);
  pid_set_setpoint(&pid_pump,450.0f); // задаем желаемое значение nernst
  // END INIT PID_CONTROLLER

  /* Infinite loop */
  for(;;)
  {
    /* USER CODE BEGIN StartTask03 */
    input = nernst;
    pid_compute(&pid_pump, get_time_ms(), input); // вычисление коэфициентов
    osDelay(5);
    /* USER CODE END StartTask03 */
  }
}

// extern float pcf8591_value_in[4];

void Task04(void const * argument)
{
  nernst = (adc_val_1 - adc_val_3)*2; // nernst voltage
  if(nernst<100) nernst /= 4; 
  current_shunt_2 = ((adc_val_2 / gain_amplifier_2) / shunt_2) - 11.0f ; // Current through shunt 2
  if(current_shunt_2 > -0.2f && current_shunt_2 < 0.20f) current_shunt_2 = 0.0f; 

  ST7735_setRotation(0);
  ST7735_setTextSize(2);
  ST7735_setTextColor(ST7735_CYAN);
  ST7735_setCursor(8,   4); ST7735_draw_fvalue((pid_pump.output)/10.0f,0); // pid
  ST7735_setCursor(8,   24); ST7735_draw_fvalue((pid_heater.output)*1.0f,0); // pid
  

  ST7735_setTextColor(ST7735_GREEN);
  // ST7735_setCursor(72,   4); ST7735_draw_value(get_time_sec());

  ST7735_setTextColor(ST7735_GREEN);
  ST7735_fillRect(112,  4, 16, 16, ST7735_BLACK);
  ST7735_setCursor(64,   4); ST7735_draw_fvalue(pumping, (pumping>100) ? 0 : 1);
  ST7735_setCursor(72,   24); ST7735_draw_value(get_time_sec()/25);

  ST7735_setRotation(3);
  ST7735_setTextSize(3);

  ST7735_setTextColor(ST7735_RED);
  ST7735_fillRect(46,  12, 70, 24, ST7735_BLACK);
  ST7735_drawRect(8,  4, 110, 36, ST7735_RED);
  ST7735_setCursor(12,   12); ST7735_draw_fvalue(current_shunt_2,2);

  ST7735_setTextColor(ST7735_YELLOW);
  ST7735_setCursor(12,   52); (resistance < 10.0 ) ? ST7735_draw_fvalue(resistance, 2) :  ST7735_draw_fvalue(resistance, 1);

  ST7735_setTextSize(2);

  volatile static float old_nernst;
  ST7735_setTextColor(ST7735_BLACK);
  ST7735_setCursor(12,   92); ST7735_draw_fvalue(old_nernst, 0); // clear display
  ST7735_setTextColor(ST7735_BLUE);
  ST7735_setCursor(12,   92); ST7735_draw_fvalue(nernst, 0);
  old_nernst = nernst;

  osDelay(5000);
  /* USER CODE END StartTask04 */
}

void Task05(void const * argument)
{
  /* USER CODE init StartTask05 */
  /* Infinite loop */
  for(;;)
  {
    /* USER CODE BEGIN StartTask05 */
    soft_pwm(PWMVGND_GPIO_Port, PWMVGND_Pin, 30, 60); // pwm 50% pin PB0 virtual ground 2.5 v
    osDelay(1);
    /* USER CODE END StartTask05 */
  }
}

void Task06(void const * argument)
{
  /* USER CODE init StartTask06 */
  /* Infinite loop */
  for(;;)
  {
    /* USER CODE BEGIN StartTask06 */
    // soft_pwm(PWMPUMP_GPIO_Port, PWMPUMP_Pin, 50, 100); // pin PB1 pump current debug
    soft_pwm(PWMPUMP_GPIO_Port, PWMPUMP_Pin, pumping, 500); // pin PB1 pump current
    // soft_pwm(PWMPUMP_GPIO_Port, PWMPUMP_Pin, (int)(pid_get_output(&pid_pump)/10), 200); // pin PB1 pump current
    osDelay(1);
    /* USER CODE END StartTask06 */
  }
}

