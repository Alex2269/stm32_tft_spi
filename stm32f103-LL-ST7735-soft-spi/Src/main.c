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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc_cfg.h"
#include <math.h>
//#include <stdlib.h>
#include "ST7735.h"

#define m_PI 3.14159

extern uint16_t ST7735_WIDTH;
extern uint16_t ST7735_HEIGHT;

__STATIC_INLINE void DelayMicro(uint32_t __IO micros)
{
  micros *=(SystemCoreClock/1000000)/5;
  while(micros--);
}

void tft_test(void)
{
    /// /** <:
    for(uint16_t i=0;i<20;i++)
    {
      ST7735_FillScreen((uint16_t)random());
      LL_mDelay(150);
    }
    LL_mDelay(500);
    ST7735_FillScreen(ST7735_BLACK);
    for(uint16_t i=0;i<20;i++)
    {
      ST7735_FillRect(0, 0, ST7735_WIDTH/2-1, ST7735_HEIGHT/2-1, (uint16_t)random());
      LL_mDelay(100);
      ST7735_FillRect(ST7735_WIDTH/2, 0, ST7735_WIDTH-1, ST7735_HEIGHT/2-1, (uint16_t)random());
      LL_mDelay(100);
      ST7735_FillRect(0, ST7735_HEIGHT/2, ST7735_WIDTH/2-1, ST7735_HEIGHT-1, (uint16_t)random());
      LL_mDelay(100);
      ST7735_FillRect(ST7735_WIDTH/2, ST7735_HEIGHT/2, ST7735_WIDTH-1, ST7735_HEIGHT-1, (uint16_t)random());
      LL_mDelay(100);
    }
    LL_mDelay(500);
    ST7735_FillScreen(ST7735_BLACK);
    for(uint16_t i=0;i<1000;i++)
    {
      ST7735_FillRect((uint16_t)random()%ST7735_WIDTH,
                      (uint16_t)random()%ST7735_HEIGHT,
                      (uint16_t)random()%ST7735_WIDTH,
                      (uint16_t)random()%ST7735_HEIGHT,
                      (uint16_t)random());
      LL_mDelay(10);
    }
    LL_mDelay(500);
    ST7735_FillScreen(ST7735_BLACK);
    for(uint16_t i=0;i<15000;i++)
    {
      for(uint16_t j=0;j<100;j++)
      {
        ST7735_DrawPixel((uint16_t)random()%ST7735_WIDTH,
                         (uint16_t)random()%ST7735_HEIGHT,
                         ST7735_BLACK);
      }
      ST7735_DrawPixel((uint16_t)random()%ST7735_WIDTH,
                       (uint16_t)random()%ST7735_HEIGHT,
                       (uint16_t)random());
      DelayMicro(100);
    }
    LL_mDelay(500);
    for(uint16_t j=0;j<20;j++)
    {
      for(uint16_t i=0;i<ST7735_WIDTH;i++)
      {
        ST7735_DrawLine((uint16_t)random(),i,0,i,ST7735_HEIGHT-1);
      }
    }
    LL_mDelay(500);
    ST7735_FillScreen(ST7735_BLACK);
    for(uint16_t i=0;i<1000;i++)
    {
      ST7735_DrawLine((uint16_t)random(),
                      (uint16_t)random()%ST7735_WIDTH,
                      (uint16_t)random()%ST7735_HEIGHT,
                      (uint16_t)random()%ST7735_WIDTH,
                      (uint16_t)random()%ST7735_HEIGHT);
      LL_mDelay(10);
    }
    LL_mDelay(500);
    ST7735_FillScreen(ST7735_BLACK);
    for(uint16_t j=0;j<20;j++)
    {
      for(uint16_t i=0;i<ST7735_WIDTH/2;i++)
      {
        ST7735_DrawRect((uint16_t)random(), i, i, ST7735_WIDTH-i-1, ST7735_HEIGHT-i-1);
      }
    }
    LL_mDelay(500);
    ST7735_FillScreen(ST7735_BLACK);
    for(uint16_t i=0;i<1000;i++)
    {
      ST7735_DrawCircle((uint16_t)random()%(ST7735_WIDTH-40)+20,
                         (uint16_t)random()%(ST7735_HEIGHT-40)+20,
                         20,(uint16_t)random());
      LL_mDelay(3);
    }
    LL_mDelay(500);

    ST7735_FillScreen(ST7735_BLACK);
    LL_mDelay(500);

    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetTextColor(ST7735_YELLOW);
    ST7735_SetBackColor(ST7735_BLUE);
    ST7735_SetFont(&Font24);
    ST7735_DrawChar(10,10,'S');
    ST7735_DrawChar(27,10,('t'));
    ST7735_DrawChar(44,10,('m'));
    ST7735_DrawChar(61,10,('3'));
    ST7735_DrawChar(78,10,('2'));
    ST7735_SetTextColor(ST7735_GREEN);
    ST7735_SetBackColor(ST7735_RED);
    ST7735_SetFont(&Font20);
    ST7735_DrawChar(10,34,('S'));
    ST7735_DrawChar(24,34,('t'));
    ST7735_DrawChar(38,34,('m'));
    ST7735_DrawChar(52,34,('3'));
    ST7735_DrawChar(66,34,('2'));
    ST7735_SetTextColor(ST7735_BLUE);
    ST7735_SetBackColor(ST7735_YELLOW);
    ST7735_SetFont(&Font16);
    ST7735_DrawChar(10,54,('S'));
    ST7735_DrawChar(21,54,('t'));
    ST7735_DrawChar(32,54,('m'));
    ST7735_DrawChar(43,54,('3'));
    ST7735_DrawChar(54,54,('2'));
    ST7735_SetTextColor(ST7735_CYAN);
    ST7735_SetBackColor(ST7735_BLACK);
    ST7735_SetFont(&Font12);
    ST7735_DrawChar(10,70,('S'));
    ST7735_DrawChar(17,70,('t'));
    ST7735_DrawChar(24,70,('m'));
    ST7735_DrawChar(31,70,('3'));
    ST7735_DrawChar(38,70,('2'));
    ST7735_SetTextColor(ST7735_RED);
    ST7735_SetBackColor(ST7735_GREEN);
    ST7735_SetFont(&Font8);
    ST7735_DrawChar(10,82,('S'));
    ST7735_DrawChar(15,82,('t'));
    ST7735_DrawChar(20,82,('m'));
    ST7735_DrawChar(25,82,('3'));
    ST7735_DrawChar(30,82,('2'));
    ST7735_SetTextColor(ST7735_YELLOW);
    ST7735_SetBackColor(ST7735_BLUE);
    LL_mDelay(1000);
    ST7735_FillScreen(ST7735_BLACK);
    /// */
    
    for(uint16_t i=0;i<16;i++)
    {
      ST7735_SetRotation(i%4);
      ST7735_SetFont(&Font24);
      ST7735_FillScreen(ST7735_BLACK);
      ST7735_String(1,10,"ABCDEF12345678");
      ST7735_SetFont(&Font20);
      ST7735_String(1,30,"ABCDEFGHI12345678");
      ST7735_SetFont(&Font16);
      ST7735_String(1,50,"ABCDEFGHIKL123456789");
      ST7735_SetFont(&Font12);
      ST7735_String(1,70,"ABCDEFGHIKLMNOPQRSTUVWXY 123456789");
      ST7735_SetFont(&Font8);
      ST7735_String(1,90,"ABCDEFGHIKLMNOPQRSTUVWXYZ 123456789ABCDEFGHIKL");
      LL_mDelay(1000);
    }
    LL_mDelay(1000);
    ST7735_SetRotation(0);
}


uint32_t tim_counter = 0;

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
  /* USER CODE BEGIN 2 */
  ADC_Init(ADC1);
  stled_off();
  
  ST7735_ini(128, 160);
  ST7735_SetRotation(1);

  for(uint8_t i=0;i<10;i++)
  {
    ST7735_FillScreen((uint16_t)random());
    ST7735_FillScreen(ST7735_RED);
    ST7735_FillScreen(ST7735_GREEN);
    ST7735_FillScreen(ST7735_BLUE);
  }

  for(uint8_t i=0;i<4;i++)
      tft_test();
  
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

   if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
    Error_Handler();  
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
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(128000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
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
