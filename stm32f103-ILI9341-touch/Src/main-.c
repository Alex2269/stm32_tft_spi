/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "pins_ext.h"
#include "bit_band.h"
#include "soft_spi.h"
#include <math.h>
// #include <stdlib.h>
#include "ILI9341.h"
#include "xpt2046.h"
#include "draw.h"
#include "utoa.h"

#define M_PI 3.14159

void stled_on(void)
{
  HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
}

void stled_off(void)
{
  HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
}

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint16_t ofset_ray=12; // vertical ofset ray variable by Src/draw.c
uint16_t accuracy=128; // speed - accuracy ray

extern bitband_t mosi; // mosi = BITBAND_PERIPH(&GPIOB->ODR, 6);
extern bitband_t clock; // clock = BITBAND_PERIPH(&GPIOB->ODR, 5);
extern bitband_t miso; // miso = BITBAND_PERIPH(&GPIOB->IDR, 4);
extern bitband_t lcd_rst; // lcd_rst = BITBAND_PERIPH(&GPIOB->ODR, 8);
extern bitband_t lcd_cs; // lcd_cs = BITBAND_PERIPH(&GPIOB->ODR, 9);
extern bitband_t lcd_dc; // lcd_dc = BITBAND_PERIPH(&GPIOB->ODR, 7);

extern bitband_t touch_cs;// = BITBAND_PERIPH(&GPIOB->ODR, 3);
extern bitband_t mosi_2;// = BITBAND_PERIPH(&GPIOB->ODR, 6);
extern bitband_t miso_2;// = BITBAND_PERIPH(&GPIOB->IDR, 4);
extern bitband_t clock_2;// = BITBAND_PERIPH(&GPIOB->ODR, 5);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  mosi = BITBAND_PERIPH(&GPIOB->ODR, 6);
  miso = BITBAND_PERIPH(&GPIOB->IDR, 4);
  clock = BITBAND_PERIPH(&GPIOB->ODR, 5);

  lcd_rst = BITBAND_PERIPH(&GPIOB->ODR, 8);
  lcd_cs = BITBAND_PERIPH(&GPIOB->ODR, 9);
  lcd_dc = BITBAND_PERIPH(&GPIOB->ODR, 7);

  mosi_2 = BITBAND_PERIPH(&GPIOA->ODR, 15);
  miso_2 = BITBAND_PERIPH(&GPIOA->IDR, 12);
  touch_cs = BITBAND_PERIPH(&GPIOB->ODR, 3);
  clock_2 = BITBAND_PERIPH(&GPIOB->ODR, 4);

  pin_mode(GPIOB, GPIO_PIN_3, GPIO_MODE_OUTPUT_PP); // lcd light "LCD_LED_Pin"
  pin_mode(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT); // lcd miso
  pin_mode(GPIOB, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP); // lcd clk
  pin_mode(GPIOB, GPIO_PIN_6, GPIO_MODE_OUTPUT_PP); // lcd mosi
  pin_mode(GPIOB, GPIO_PIN_7, GPIO_MODE_OUTPUT_PP); // lcd data/command
  pin_mode(GPIOB, GPIO_PIN_8, GPIO_MODE_OUTPUT_PP); // lcd reset
  pin_mode(GPIOB, GPIO_PIN_9, GPIO_MODE_OUTPUT_PP); // lcd cs
//   bitband_t mosi = BITBAND_PERIPH(&GPIOB->ODR, 6);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */

  ILI9341_reset();
  // HAL_GPIO_WritePin(GPIOA, LCD_LED_Pin, GPIO_PIN_SET);
  soft_spi_set_bit_order(SPI_MSB_FIRST);
  soft_spi_set_data_mode(SPI_MODE0);
  soft_spi_set_clock_divider(SPI_CLOCK_DIV16);
  
  soft_spi_2_set_bit_order(SPI_MSB_FIRST);
  soft_spi_2_set_data_mode(SPI_MODE0);
  soft_spi_2_set_clock_2_divider(SPI_CLOCK_DIV16);

  ILI9341_begin(/*&hspi1*/);

  ILI9341_setRotation(0); // clear display & draw grid
  ILI9341_fillScreen(ILI9341_BLACK);
  ILI9341_setRotation(3);
  ILI9341_fillScreen(ILI9341_BLACK);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  HAL_TIM_Base_Init(&htim3);
  HAL_TIM_PeriodElapsedCallback(&htim3);
  HAL_TIM_Base_Start_IT(&htim3); // timer run
  
// HAL_TIM_Base_Init(&htim4);
// HAL_TIM_Base_Start_IT(&htim4); // timer run
  
  HAL_ADC_Start(&hadc1); // adc run
  write_grid();
  TS_Point p;
  p.x=0;
  p.y=0;
  p.z=0;
  //draw_touch(p.x,p.y,p.z); // for text data

  while (1)
  {
    get_point(&p); // get coordinates x y z from xpt2046
//     ILI9341_setRotation(3);
//     draw_touch(p.x,p.y,p.z); // for text data
    if(p.z>100)
    {
      if(p.x>3000) ofset_ray+=4;
      if(p.x<1000) ofset_ray-=4;

      if(p.y>2800) accuracy+=8; // adc counter in cycle
      if(p.y<1200) accuracy-=8; // adc counter in cycle
      if(accuracy>512) accuracy=512; // adc counter in cycle
      if(accuracy<8) accuracy=8; // adc counter in cycle

    }
   
    oscil_run();
    /*for(uint16_t i = 0; i < 320; i++) // measurement and display redraw
    {
      fill_buff(i);
      write_graph(i);
      if(i==1)
       write_text(i);
      if(!i)
      {
        write_grid();
      }
    }*/

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3)
{
  HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
   *    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
