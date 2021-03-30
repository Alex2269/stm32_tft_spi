/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc_cfg.h"
#include <math.h>
//#include <stdlib.h>
#include "ILI9488.h"
#include "soft_lcd.h"
#include "soft_adc.h"
#include "soft_ina219.h"
#include "soft_pcf8591.h"

#include "pins_ext.h"

#define m_PI 3.14159

uint32_t adcResult = 0;
uint32_t max_result = 0;
float real_result[160] = {0};

uint16_t adc_counter = 0;
uint16_t i = 0;

uint16_t coord_x[160] = {0};
uint16_t coord_y[160] = {0};

uint32_t tim_counter = 0;
void fill_text(uint16_t i);

lcd_t *lcd; // pointer to i2c object, for lcd module
adc_t *adc; // pointer to i2c object, for ads1115
ina_t *ina; // pointer to i2c object, for ina219

void fill_buff(uint16_t i)
{
  if(i>158)
  {
    max_result = 0;
    for(uint16_t i = 0; i < 160; i++)
    {
      // ILI9488_fillCircle(coord_x[i], coord_y[i], 2, ILI9488_BLACK);
      // ILI9488_drawLine(coord_x[i], coord_y[i], coord_x[i-1], coord_y[i-1], ILI9488_BLACK); //(  x0,y0,x1,y1,color);
      // ILI9488_drawLine(coord_x[i], coord_y[i]+1, coord_x[i-1], coord_y[i-1]+1, ILI9488_BLACK); //(  x0,y0,x1,y1,color);
      ILI9488_drawLine(coord_x[i], coord_y[i]-1, coord_x[i-1], coord_y[i-1]-1, ILI9488_BLACK); //(  x0,y0,x1,y1,color);
      // ILI9488_drawLine(coord_x[i]+1, coord_y[i], coord_x[i-1]+1, coord_y[i-1], ILI9488_BLACK); //(  x0,y0,x1,y1,color);
      ILI9488_drawLine(coord_x[i]-1, coord_y[i], coord_x[i-1]-1, coord_y[i-1], ILI9488_BLACK); //(  x0,y0,x1,y1,color);
    }

    // ILI9488_fillScreen(ILI9488_BLACK);
    for(uint16_t i = 5; i<160; i+=23) ILI9488_drawLine( i, 10, i, 230, ILI9488_WHITE); //draw vertical lines
    for(uint16_t i = 10; i<128; i+=22) ILI9488_drawLine( 10, i, 300, i, ILI9488_WHITE); //draw horizontal lines
    ILI9488_drawLine( 10, 119, 300, 119, ILI9488_WHITE); //draw horizontal lines    
    // ILI9488_drawLine( 10, 121, 300, 121, ILI9488_WHITE); //draw horizontal lines
    fill_text(i);
  }

  while(adc_counter < 12) // Пока количество измерений меньше N
  {
    adcResult += read_adc(ADC1, 0);
    adc_counter++; // Увеличиваем счетчик измерений
  }

  coord_x[i] = i; // сохраняем координаты <cord_x/y> в буфер для последующей очистки дисплея
  coord_y[i] = 230-adcResult/adc_counter/18; // Вычисляем среднее значение
  real_result[i] = adcResult/adc_counter*0.82; // 3300/4096=0.80566 Вычисляем среднее значение
  
  if(real_result[i]<500) real_result[i] = 0;
  if(real_result[i]>500) real_result[i] -= 500;
  real_result[i] *= .4; // значение в барах
  
  adcResult = 0; // Обнуляем значение
  adc_counter = 0; // Обнуляем счетчик измерений
  if(real_result[i]>max_result) max_result = real_result[i]; // сохранение максимального числа
}

void write_graph(uint16_t i)
{
  if(i) // draw values
  {
    // fill_text(i);
    // ILI9488_fillCircle(coord_x[i], coord_y[i], 2, ILI9488_YELLOW);
    // ILI9488_drawLine(coord_x[i], coord_y[i], coord_x[i-1], coord_y[i-1], ILI9488_CYAN); //(  x0,y0,x1,y1,color);
    // ILI9488_drawLine(coord_x[i], coord_y[i]+1, coord_x[i-1], coord_y[i-1]+1, ILI9488_GREEN); //(  x0,y0,x1,y1,color);
    ILI9488_drawLine(coord_x[i], coord_y[i]-1, coord_x[i-1], coord_y[i-1]-1, ILI9488_CYAN); //(  x0,y0,x1,y1,color);
    // ILI9488_drawLine(coord_x[i]+1, coord_y[i], coord_x[i-1]+1, coord_y[i-1], ILI9488_GREEN); //(  x0,y0,x1,y1,color);
    ILI9488_drawLine(coord_x[i]-1, coord_y[i], coord_x[i-1]-1, coord_y[i-1], ILI9488_CYAN); //(  x0,y0,x1,y1,color);
  }
}

void fill_text(uint16_t i)
{
  ILI9488_setTextBgColor(ILI9488_BLACK);

  ILI9488_setTextSize(2);
  ILI9488_setTextColor(ILI9488_YELLOW);

  read_adc(ADC1, 0); // heed for rank count
  ILI9488_setCursor(10,  10); ILI9488_draw_value(read_adc(ADC1, 1));
  ILI9488_setCursor(10,  45); ILI9488_draw_value(read_adc(ADC1, 2));
  ILI9488_setCursor(10,  80); ILI9488_draw_value(read_adc(ADC1, 3));
  ILI9488_setCursor(10, 115); ILI9488_draw_value(read_adc(ADC1, 4));
  ILI9488_setCursor(10, 150); ILI9488_draw_value(read_adc(ADC1, 5));
  ILI9488_setCursor(10, 185); ILI9488_draw_value(read_adc(ADC1, 6));
}

void stled_off(void)
{
  LL_GPIO_SetOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
}

void stled_on(void)
{
  LL_GPIO_ResetOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
}

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

void i2c_lcd_bus(void);
void i2c_ads1115_bus(void);
void i2c_ina219_bus(void);

/*!
** the callback function from TIM4_IRQHandler
** period the tick timer one clock makes 10 mksec
**/

/*!
** the create i2c objects
**/
void i2c_ads1115_bus(void)
{
  #define adc_address 0x48
  #define adc_scl LL_GPIO_PIN_5
  #define adc_sda LL_GPIO_PIN_4

  adc = ads1115_create(GPIOB, adc_scl, adc_sda, adc_address);
}

/*!
** the create i2c objects
**/
void i2c_ina219_bus(void)
{
  #define ina_address 0x40
  #define ina_scl LL_GPIO_PIN_7
  #define ina_sda LL_GPIO_PIN_6

  ina = ina219_create(GPIOB, ina_scl, ina_sda, ina_address);
}

/*!
** the create i2c objects
**/
void i2c_lcd_bus(void)
{
  #define i2c_lcd_address 0x3f
  #define lcd_scl LL_GPIO_PIN_4
  #define lcd_sda LL_GPIO_PIN_3

  lcd = lcd_create(GPIOB, lcd_scl, lcd_sda, i2c_lcd_address, 4);
}

/*!
** the runnind measure and show result
**/
void show_ina219(lcd_t *lcd, ina_t *ina)
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float power_mW = 0;

  shuntvoltage = ina219_getShuntVoltage_mV(ina);
  busvoltage = ina219_getBusVoltage_V(ina);
  current_mA = ina219_getCurrent_mA(ina);
  power_mW = ina219_getPower_mW(ina);

  lcd_pos(lcd, 0,6); lcd_print(lcd, "ShuntV"); lcd_pos(lcd, 0,14);lcd_printdigit(lcd, shuntvoltage);
  lcd_pos(lcd, 1,6); lcd_print(lcd, "Bus  V"); lcd_pos(lcd, 1,14);lcd_printdigit(lcd, busvoltage);
  lcd_pos(lcd, 2,6); lcd_print(lcd, "cur mA"); lcd_pos(lcd, 2,14);lcd_printdigit(lcd, current_mA);
  lcd_pos(lcd, 3,6); lcd_print(lcd, "pwr mW"); lcd_pos(lcd, 3,14);lcd_printdigit(lcd, power_mW);
}

/*!
** the runnind measure and show result
**/
void show_ads1115(lcd_t *lcd, adc_t *adc)
{
  uint16_t val = 0;
  const float VPS = 6.144 / 32768.0;  // volts per step

  float measure_1;
  float measure_2;

  ads1115_set_single_channal(adc, 3);
  val = ads1115_get(adc);

  uint8_t gain = 1;
  uint8_t diff_in = 1;
  ads1115_set_diff_channal(adc, diff_in, gain);
  measure_1 = get_diff_measure(adc, gain);

  gain = 1;
  diff_in = 2;
  ads1115_set_diff_channal(adc, diff_in, gain);
  measure_2 = get_diff_measure(adc, gain);

  lcd_pos(lcd, 0,0); lcd_printdigit(lcd, val*VPS);
  lcd_pos(lcd, 1,0); lcd_printdigit(lcd, measure_1);
  lcd_pos(lcd, 2,0); lcd_printdigit(lcd, measure_2);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* PendSV_IRQn interrupt configuration */
  NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));
  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  /** NOJTAG: JTAG-DP Disabled and SW-DP Enabled
  */
  LL_GPIO_AF_Remap_SWJ_NOJTAG();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  ADC_Init(ADC1);
  lcd_reset();
  stled_off();
  
  ILI9488_begin();
  ILI9488_setRotation(3);
  ILI9488_fillScreen(ILI9488_BLACK);

  /*
  uint8_t w_tft = 160;
  uint8_t h_tft = 128;
  uint8_t top = 8;
  uint8_t bot = h_tft-8;
  for(uint16_t i =  0; i<=w_tft; i+=w_tft/2-top/2) ILI9488_drawLine( i, top, i, bot, ILI9488_WHITE); // draw vertical lines
  for(uint16_t i =   8; i<h_tft; i+=36) ILI9488_drawLine( 14, i, w_tft-18, i, ILI9488_WHITE); // draw horizontal lines
  */

  // ILI9488_drawLine( 10, h_tft/2-1, w_tft-10, h_tft/2-1, ILI9488_WHITE); //draw horizontal lines    
  // ILI9488_drawLine( 10, 121, 300, 121, ILI9488_WHITE); //draw horizontal lines

  uint8_t time[] = "time";
  ILI9488_setRotation(0);
  ILI9488_setTextSize(2);
  ILI9488_setTextBgColor(ILI9488_BLACK);
  ILI9488_setTextColor(ILI9488_CYAN);
  ILI9488_setCursor(8,   24); ILI9488_writeString(time); // getTickCount
  ILI9488_setRotation(3);
//   ILI9488_drawRect(8,  12, 140, 52, ILI9488_RED);
//   ILI9488_drawRect(8,  64, 140, 52, ILI9488_YELLOW);
//   ILI9488_drawRect(8, 116, 140, 52, ILI9488_BLUE);
  ILI9488_setTextSize(2);

  /**
   * @uncomment active:
   */
  // i2c_lcd_bus();
  // lcd_pos(lcd, 0,6); lcd_print(lcd, "test");
  // i2c_ads1115_bus();
  // i2c_ina219_bus();
  // write_grid();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // HAL_TIM_Base_Init(&htim3);
  // HAL_TIM_PeriodElapsedCallback(&htim3);
  // HAL_TIM_Base_Start_IT(&htim3); //запустим таймер

  // HAL_TIM_Base_Init(&htim4);
  // HAL_TIM_Base_Start_IT(&htim4); //запустим таймер

  for(uint8_t i=0;i<31;i++)
  {
    LL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
    LL_mDelay(25);
  }

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    for(uint16_t i = 0; i < 160; i++) // measurement and display redraw
    {
      LL_mDelay(1);
      fill_buff(i);
      write_graph(i);
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_16);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(128000000);
  LL_SetSystemCoreClock(128000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
