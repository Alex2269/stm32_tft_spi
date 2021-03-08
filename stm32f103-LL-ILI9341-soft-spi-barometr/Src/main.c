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
#include "adc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
//#include <stdlib.h>
#include "utoa.h"
#include "ILI9341.h"

#define M_PI 3.14159

uint32_t adcResult = 0;
uint32_t max_result = 0;
float real_result[320] = {0};

uint16_t adc_counter = 0;
uint16_t i = 0;

uint16_t coord_x[320] = {0};
uint16_t coord_y[320] = {0};

uint32_t tim_counter = 0;
void fill_text(uint16_t i);

void fill_buff(uint16_t i)
{
  if(i>318)
  {
    max_result = 0;
    for(uint16_t i = 0; i < 320; i++)
    {
    //ILI9341_fillCircle(coord_x[i], coord_y[i], 2, ILI9341_BLACK);
    //ILI9341_drawLine(coord_x[i], coord_y[i], coord_x[i-1], coord_y[i-1], ILI9341_BLACK); //(  x0,y0,x1,y1,color);
    //ILI9341_drawLine(coord_x[i], coord_y[i]+1, coord_x[i-1], coord_y[i-1]+1, ILI9341_BLACK); //(  x0,y0,x1,y1,color);
    ILI9341_drawLine(coord_x[i], coord_y[i]-1, coord_x[i-1], coord_y[i-1]-1, ILI9341_BLACK); //(  x0,y0,x1,y1,color);
    //ILI9341_drawLine(coord_x[i]+1, coord_y[i], coord_x[i-1]+1, coord_y[i-1], ILI9341_BLACK); //(  x0,y0,x1,y1,color);
    ILI9341_drawLine(coord_x[i]-1, coord_y[i], coord_x[i-1]-1, coord_y[i-1], ILI9341_BLACK); //(  x0,y0,x1,y1,color);
    }

    //ILI9341_fillScreen(ILI9341_BLACK);
    for(uint16_t i = 5; i<320; i+=23) ILI9341_drawLine( i, 10, i, 230, ILI9341_WHITE); //draw vertical lines
    for(uint16_t i = 10; i<240; i+=22) ILI9341_drawLine( 10, i, 300, i, ILI9341_WHITE); //draw horizontal lines
    ILI9341_drawLine( 10, 119, 300, 119, ILI9341_WHITE); //draw horizontal lines    
    //ILI9341_drawLine( 10, 121, 300, 121, ILI9341_WHITE); //draw horizontal lines
    fill_text(i);
  }

  while(adc_counter < 128) // Пока количество измерений меньше N
  {
    adcResult += HAL_ADC_GetValue(&hadc1); // Сумируем полученные значения ацп
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
    fill_text(i);
    //ILI9341_fillCircle(coord_x[i], coord_y[i], 2, ILI9341_YELLOW);
    //ILI9341_drawLine(coord_x[i], coord_y[i], coord_x[i-1], coord_y[i-1], ILI9341_CYAN); //(  x0,y0,x1,y1,color);
    //ILI9341_drawLine(coord_x[i], coord_y[i]+1, coord_x[i-1], coord_y[i-1]+1, ILI9341_GREEN); //(  x0,y0,x1,y1,color);
    ILI9341_drawLine(coord_x[i], coord_y[i]-1, coord_x[i-1], coord_y[i-1]-1, ILI9341_CYAN); //(  x0,y0,x1,y1,color);
    //ILI9341_drawLine(coord_x[i]+1, coord_y[i], coord_x[i-1]+1, coord_y[i-1], ILI9341_GREEN); //(  x0,y0,x1,y1,color);
    ILI9341_drawLine(coord_x[i]-1, coord_y[i], coord_x[i-1]-1, coord_y[i-1], ILI9341_CYAN); //(  x0,y0,x1,y1,color);
  }
}

void fill_text(uint16_t i)
{
  ILI9341_setTextBgColor(ILI9341_BLACK);

  ILI9341_setCursor(10, 15);
  ILI9341_setTextSize(2);
  ILI9341_setTextColor(ILI9341_CYAN);
  char lb_real[16] = " bar ";
  ILI9341_writeString(lb_real);

  ILI9341_setTextSize(3);
  ILI9341_setTextColor(ILI9341_YELLOW);

  char real_mv[16];
  utoa((int)real_result[i], real_mv, 10);
  if(real_result[i]<1000)
      ILI9341_write('0');
  if(real_result[i]<100)
      ILI9341_write('0');
  ILI9341_writeString(real_mv);

  ILI9341_setCursor(160, 15);
  ILI9341_setTextSize(2);
  ILI9341_setTextColor(ILI9341_CYAN);
  char lb_max[16] = " Max ";
  ILI9341_writeString(lb_max);

  ILI9341_setTextSize(3);
  ILI9341_setTextColor(ILI9341_YELLOW);

  char max_mv[16];
  utoa((int)max_result, max_mv, 10);
  if(max_result<1000)
      ILI9341_write('0');
  if(max_result<100)
      ILI9341_write('0');
  ILI9341_writeString(max_mv);
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
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  lcd_reset();

  stled_off();
  
  ILI9341_begin();
  
  ILI9341_setRotation(0); // clear display & draw grid
  ILI9341_fillScreen(ILI9341_BLACK);
  ILI9341_setRotation(3);
  ILI9341_fillScreen(ILI9341_BLACK);

  for(uint16_t i = 5; i<320; i+=23) ILI9341_drawLine( i, 10, i, 230, ILI9341_WHITE); //draw vertical lines
  for(uint16_t i = 10; i<240; i+=22) ILI9341_drawLine( 10, i, 300, i, ILI9341_WHITE); //draw horizontal lines
  ILI9341_drawLine( 10, 119, 300, 119, ILI9341_WHITE); //draw horizontal lines    
  //ILI9341_drawLine( 10, 121, 300, 121, ILI9341_WHITE); //draw horizontal lines

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HAL_TIM_Base_Init(&htim3);
  HAL_TIM_PeriodElapsedCallback(&htim3);
  HAL_TIM_Base_Start_IT(&htim3); //запустим таймер

  // HAL_TIM_Base_Init(&htim4);
  // HAL_TIM_Base_Start_IT(&htim4); //запустим таймер

  for(uint8_t i=0;i<31;i++)
  {
    LL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
    LL_mDelay(25);
  }
  
  HAL_ADC_Start(&hadc1);//запустим аналогово-цифровое преобразование

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    for(uint16_t i = 0; i < 320; i++) // measurement and display redraw
    {
      LL_mDelay(2);
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

   if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
    Error_Handler();  
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
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
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
