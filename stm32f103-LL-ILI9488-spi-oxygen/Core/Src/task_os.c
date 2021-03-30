#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "ILI9488.h"
#include "graph.h"
#include "adc_cfg.h"
#include "task_os.h"

#include "soft_lcd.h"
#include "soft_adc.h"
#include "soft_ina219.h"
#include "soft_pcf8591.h"

#include "pins_ext.h"

#include "time.h"
#include "pid.h"

#define debug 0

#if debug
  volatile float debug_val;
#endif

struct pid_controller_t pid_pump;
struct pid_controller_t pid_heater;

// --
float U_power = 12600; // power voltage
float resistance;
// --
float current_shunt_1;
float shunt_1 = 0.010; // resistance shunt_1
float correction_1 = +0.0;
#define R1 1000.0 // resistor feedback to ground
#define R2 47000.0 // resistor feedback
#define DELTA_GAIN_1 0.725 // calibration heater of an error
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
volatile float virt_gnd;

uint16_t buff_nernst_draw[480] = {0};
uint16_t buff_resistance_draw[480] = {0};
uint16_t buff_pump_draw[480] = {0};

uint16_t buff_nernst_clean[480] = {0};
uint16_t buff_resistance_clean[480] = {0};
uint16_t buff_pump_clean[480] = {0};

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
  if(resistance>99.0)
  {
    resistance=99.0;
    save_time_sec();
  }

  /*
   * if exit from range
   * protection heating
   * setting the low level on gate mosfet
   */
  if( (resistance < 1.95) || (resistance > 29.05 ))
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
  adc_val_3 = read_adc(ADC1, 3); // read pin PA3 for current pump value
  // adc_val_1 = adc_get_value(ADC1, 1) * 5000.0 / 4096.0; // read pin PA1 for nernst different voltage
  // adc_val_2 = adc_get_value(ADC1, 2) * 5000.0 / 4096.0; // channel 2, pin PA2 for virtual ground value

  #define ADCX_COUNT 8
  adc_val_1=0;
  adc_val_2=0;
  for (uint8_t i =0; i<ADCX_COUNT; i++)
  {
    adc_val_1 += adc_get_value(ADC1, 1) * 5000.0 / 4096.0; // read pin PA1 for nernst different voltage
    adc_val_2 += adc_get_value(ADC1, 2) * 5000.0 / 4096.0; // channel 2, pin PA2 for virtual ground value
  }
  adc_val_1 /= ADCX_COUNT;
  virt_gnd = adc_val_2 / ADCX_COUNT;
  nernst = adc_val_1 - virt_gnd; // nernst voltage

  #if debug
   /*
    * measuring tool for debuging
    * 5.9 the resistive divider for read adc channel 4, pin PA4 debuging value
    * 18500 max voltage mV R-debug/R-GND
    */
    debug_val = adc_get_value(ADC1, 4) * 18500.0 / 4096.0;
  #endif
}

/*
 * show initial resistance, before heating
 */
void show_cold_value_shunt_1(void)
{
  if(get_time_new_sec()/50 > 1) return;
  else
  {
    uint8_t heater[] = "R = ";
    ILI9488_setRotation(0);
    ILI9488_setTextSize(2);
    ILI9488_setTextColor(ILI9488_YELLOW);
    ILI9488_setCursor(8,   24); ILI9488_writeString(heater);
    ILI9488_setCursor(56,   22);
    ILI9488_draw_fvalue(resistance, 2);
    ILI9488_setRotation(3);
    ILI9488_setTextSize(1);
  }
}

void DefaultTask(void const * argument)
{
  /* BEGIN StartDefaultTask */

  /* Infinite loop */
  for(;;)
  {
    if(h_cycle > 75) h_cycle = 75;
    h_cycle += 0.01; // heating cycle duration

    if(pumping < 25) pumping = 25; // min duty_cycle pwm pump current
    if(pumping > 475) pumping = 475; // max duty_cycle pwm pump current
    if(nernst>450) pumping -= 1;
    if(nernst<450) pumping += 1;

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
  ILI9488_setRotation(3);
  ILI9488_setTextSize(3);
  ILI9488_setTextColor(ILI9488_RED); ILI9488_setCursor(180, 12); ILI9488_writeString("pump");
  ILI9488_setTextColor(ILI9488_YELLOW); ILI9488_setCursor(180, 64); ILI9488_writeString("heater");
  ILI9488_setTextColor(ILI9488_BLUE); ILI9488_setCursor(180, 116); ILI9488_writeString("nernst");

  ILI9488_drawRect(8,   4, 148, 48, ILI9488_RED);
  ILI9488_drawRect(8,  56, 148, 48, ILI9488_YELLOW);
  ILI9488_drawRect(8, 108, 148, 48, ILI9488_BLUE);

  for(;;)
  {
    if(nernst<100) nernst /= 4; 
    current_shunt_2 = ((adc_val_3 / gain_amplifier_2) / shunt_2) - 11.0f ; // Current through shunt 2
    if(current_shunt_2 > -0.2f && current_shunt_2 < 0.20f) current_shunt_2 = 0.0f; 

    static volatile uint16_t xpos;
    xpos++;
    if(xpos > ILI9488_width()) xpos = 0;

    buff_pump_draw[xpos] = 250-adc_val_3/36;
    buff_resistance_draw[xpos] = 280-adc_val_0/36;
    buff_nernst_draw[xpos] = 260-nernst/36;

    write_graph_color(xpos, buff_pump_draw, buff_pump_clean, ILI9488_RED);
    write_graph_color(xpos, buff_resistance_draw, buff_resistance_clean, ILI9488_YELLOW);
    write_graph_color(xpos, buff_nernst_draw, buff_nernst_clean, ILI9488_BLUE);

    if(!xpos)
    {
        ILI9488_setRotation(1);
        write_grid();
        ILI9488_setRotation(0);
        ILI9488_setTextSize(2);
        ILI9488_setTextColor(ILI9488_CYAN);

        /*
         * use any free adc channel for debuging
         * 
         */
        #if debug
        {
         ILI9488_setTextColor(ILI9488_RED);
         ILI9488_setCursor(8, 104);
         ILI9488_writeString("debug");
         ILI9488_setTextColor(ILI9488_GREEN);
         ILI9488_setTextSize(4);
         ILI9488_setCursor(72,   104);
         ILI9488_fillRect(72,  104, 116, 32, ILI9488_BLACK);
         ILI9488_draw_value(debug_val); // debug
         ILI9488_setTextSize(2);
        }
        #endif

        ILI9488_setTextColor(ILI9488_CYAN);
        ILI9488_setCursor(8,   48); ILI9488_draw_fvalue((pid_pump.output)/10.0f,0); // pid
        ILI9488_setCursor(8,   72); ILI9488_draw_fvalue((pid_heater.output)*1.0f,0); // pid

        ILI9488_setTextColor(ILI9488_GREEN);
        // ILI9488_setCursor(72,   4); ILI9488_draw_value(get_time_sec());

        ILI9488_fillRect(112,  4, 16, 16, ILI9488_BLACK);
        ILI9488_setCursor(64,   48); ILI9488_draw_fvalue(pumping, (pumping>100) ? 0 : 1);

        show_cold_value_shunt_1(); // initial resistance, before heating

        ILI9488_setCursor(72,   72); ILI9488_draw_value(get_time_new_sec()/50);

        ILI9488_setRotation(3);
        ILI9488_setTextSize(3);

        volatile static float old_shunt_2;
        // ILI9488_drawRect(8,   4, 148, 48, ILI9488_RED);
        ILI9488_setTextColor(ILI9488_BLACK);
        ILI9488_setCursor(16,   12); ILI9488_draw_fvalue(old_shunt_2,2);
        ILI9488_setTextColor(ILI9488_RED);
        ILI9488_setCursor(16,   12); ILI9488_draw_fvalue(current_shunt_2,2);
        old_shunt_2 = current_shunt_2;

        volatile static float old_resist;
        // ILI9488_drawRect(8,  56, 148, 48, ILI9488_YELLOW);
        ILI9488_setTextColor(ILI9488_BLACK);
        ILI9488_setCursor(16,   64); (old_resist < 10.0 ) ? ILI9488_draw_fvalue(old_resist, 2) :  ILI9488_draw_fvalue(old_resist, 1);
        ILI9488_setTextColor(ILI9488_YELLOW);
        ILI9488_setCursor(16,   64); (resistance < 10.0 ) ? ILI9488_draw_fvalue(resistance, 2) :  ILI9488_draw_fvalue(resistance, 1);
        old_resist = resistance;

        // volatile static float old_nernst;
        // ILI9488_drawRect(8, 108, 148, 48, ILI9488_BLUE);
        ILI9488_setTextColor(ILI9488_BLACK);
        // ILI9488_setCursor(16,   116); ILI9488_draw_fvalue(old_nernst, 0); // clear display
        ILI9488_setTextColor(ILI9488_CYAN);
        ILI9488_fillRect(48, 116, 64, 24, ILI9488_BLACK); // space cleanup
        ILI9488_setCursor(16,   116); ILI9488_draw_fvalue(nernst, 0);
        // old_nernst = nernst;
    }

    osDelay(100);
  }

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

