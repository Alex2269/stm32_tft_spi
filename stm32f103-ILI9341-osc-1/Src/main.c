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
#include "SetSysClockTo128.h"
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
uint16_t ofset_ray=18; // vertical ofset ray variable by Src/draw.c
uint16_t accuracy=128; // speed - accuracy ray
uint16_t touch_pressed = 0; // if pressed

extern bitband_t mosi;
extern bitband_t clock;
extern bitband_t miso;
extern bitband_t lcd_rst;
extern bitband_t lcd_cs;
extern bitband_t lcd_dc;

extern bitband_t touch_cs;
extern bitband_t mosi_2;
extern bitband_t miso_2;
extern bitband_t clock_2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

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

/*
connect:
    --------------------------------------------------------------------
   |   .----------------------------.                                   |
   |  vcc gnd cs rst dc mosi clock led miso t_clk t_cs t_din t_do t_irq | ili9341 spi
   |   |   |   |  |   |  |    |         |    |     |    |     |    |    |
   |  vcc gnd  | b9  b8 b7   b6   b5   b4   b3    a15  a12   a11  a10   | stm32
   |           '-------------------'                                    |
    --------------------------------------------------------------------
*/

  mosi = BITBAND_PERIPH(&GPIOB->ODR, 7);
  miso = BITBAND_PERIPH(&GPIOB->IDR, 4);
  clock = BITBAND_PERIPH(&GPIOB->ODR, 6);

  lcd_cs = BITBAND_PERIPH(&GPIOB->ODR, 5);
  lcd_dc = BITBAND_PERIPH(&GPIOB->ODR, 8);
  lcd_rst = BITBAND_PERIPH(&GPIOB->ODR, 9);

  mosi_2 = BITBAND_PERIPH(&GPIOA->ODR, 12);
  miso_2 = BITBAND_PERIPH(&GPIOA->IDR, 11);
  clock_2 = BITBAND_PERIPH(&GPIOB->ODR, 3);
  touch_cs = BITBAND_PERIPH(&GPIOA->ODR, 15);

  pin_mode(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT);
  pin_mode(GPIOB, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP);
  pin_mode(GPIOB, GPIO_PIN_6, GPIO_MODE_OUTPUT_PP);
  pin_mode(GPIOB, GPIO_PIN_7, GPIO_MODE_OUTPUT_PP);
  pin_mode(GPIOB, GPIO_PIN_8, GPIO_MODE_OUTPUT_PP);
  pin_mode(GPIOB, GPIO_PIN_9, GPIO_MODE_OUTPUT_PP);

  pin_mode(GPIOA, GPIO_PIN_10, GPIO_MODE_INPUT);
  pin_mode(GPIOA, GPIO_PIN_11, GPIO_MODE_INPUT);
  pin_mode(GPIOA, GPIO_PIN_12, GPIO_MODE_OUTPUT_PP);
  pin_mode(GPIOA, GPIO_PIN_15, GPIO_MODE_OUTPUT_PP);
  pin_mode(GPIOB, GPIO_PIN_3, GPIO_MODE_OUTPUT_PP);

  /* USER CODE END Init */

  /* Configure the system clock */

  /* USER CODE BEGIN SysInit */
  HAL_RCC_DeInit();
  SystemClock_Config128();

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

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  HAL_TIM_Base_Init(&htim3);
  HAL_TIM_PeriodElapsedCallback(&htim3);
  HAL_TIM_Base_Start_IT(&htim3); // timer run
  
// HAL_TIM_Base_Init(&htim4);
// HAL_TIM_Base_Start_IT(&htim4); // timer run
  
  HAL_ADC_Start(&hadc1); // adc run

  ILI9341_setRotation(3);
  draw_statusbar();
  draw_dot_grid();

  while (1)
  {
    //test_touch();
    oscil_run();
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/* USER CODE BEGIN 4 */

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
