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
#include "adc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc_cfg.h"
#include <math.h>
//#include <stdlib.h>
// #include "ILI9341.h"
#include "soft_lcd.h"
#include "soft_adc.h"
#include "soft_ina219.h"
#include "soft_pcf8591.h"

#include "pins_ext.h"

#define m_PI 3.14159

uint32_t adcResult = 0;
uint32_t max_result = 0;
float real_result[320] = {0};

uint16_t adc_counter = 0;
uint16_t i = 0;

uint32_t tim_counter = 0;

lcd_t *lcd; // pointer to i2c object, for lcd module
adc_t *adc; // pointer to i2c object, for ads1115
ina_t *ina; // pointer to i2c object, for ina219
pcf8591_t *pcf8591; // pointer to i2c object, for pcf8591

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
  #define adc_scl LL_GPIO_PIN_1
  #define adc_sda LL_GPIO_PIN_0

  adc = ads1115_create(GPIOB, adc_scl, adc_sda, adc_address);
}

/*!
** the create i2c objects
**/
void i2c_ina219_bus(void)
{
  #define ina_address 0x40
  #define ina_scl LL_GPIO_PIN_5
  #define ina_sda LL_GPIO_PIN_4

  ina = ina219_create(GPIOB, ina_scl, ina_sda, ina_address);
}

/*!
** the create i2c objects
**/
void i2c_pcf8591_bus(void)
{
  #define pcf8591_address 0x48
  #define pcf8591_scl LL_GPIO_PIN_7
  #define pcf8591_sda LL_GPIO_PIN_6

  pcf8591 = pcf8591_create(GPIOB, pcf8591_scl, pcf8591_sda, pcf8591_address);
}

/*!
** the create i2c objects
**/
void i2c_lcd_bus(void)
{
  #define i2c_lcd_address 0x3f
  #define lcd_scl LL_GPIO_PIN_9
  #define lcd_sda LL_GPIO_PIN_8

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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  ADC_Init(ADC1);

  stled_off();
  

  /**
   * @uncomment active:
   */
  i2c_lcd_bus();
  lcd_pos(lcd, 0,6); lcd_print(lcd, "test");
  // i2c_ads1115_bus();
  // i2c_ina219_bus();
  i2c_pcf8591_bus();

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
    for(uint16_t i = 0; i < 320; i++) // measurement and display redraw
    {
      LL_mDelay(1);
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
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(72000000);
  LL_SetSystemCoreClock(72000000);
  LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_6);
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
