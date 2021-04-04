/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "dma.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_tick.h"
#include "ll_spi_ili9341.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef SEMIHOSTING_ENABLE
#include "stdio.h"
extern void initialise_monitor_handles(void);
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint16_t dsp_width;
extern uint16_t dsp_height;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void CalcAndDisplayFractal(int16_t offset_x, int16_t offset_y, uint16_t zoom);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void test(void)
{
  dsp_fill_screen(DSP_BLACK);
  for(uint16_t i=0; i<0xffff; i+=0x1111)
  {
    dsp_fill_screen(i);
  }
  // --
  dsp_fill_screen(DSP_BLACK);
  for(uint16_t i =0; i<300; i++)
  {
    fillRoundRect(rand16()%dsp_width,
                  rand16()%dsp_height,
                  rand16()%dsp_width,
                  rand16()%dsp_height,
                  10,
                  rand16());
    delay_ms(1);
  }
  for(uint16_t i =0; i<300; i++)
  {
    fillTriangle(rand16()%dsp_width,
                 rand16()%dsp_height,
                 rand16()%dsp_width,
                 rand16()%dsp_height,
                 rand16()%dsp_width,
                 rand16()%dsp_height,
                 rand16());
    delay_ms(1);
  }
  delay_ms(1e3);
  for(uint16_t i =0; i<300; i++)
  {
    FillEllipse( rand16()%dsp_width,
                 rand16()%dsp_height,
                 (rand16()%dsp_width)/4,
                 (rand16()%dsp_height)/4,
                 rand16());
    delay_ms(1);
  }
  delay_ms(1e3);
  // --
  dsp_fill_screen(DSP_BLACK);
  for(uint16_t i=0; i<0xffff; i+=0x1111)
  {
    dsp_fill_rect(0, 0, dsp_width/2-1, dsp_height/2-1, rand16());
    delay_ms(1);
    dsp_fill_rect(dsp_width/2, 0, dsp_width-1, dsp_height/2-1, rand16());
    delay_ms(1);
    dsp_fill_rect(0, dsp_height/2, dsp_width/2-1, dsp_height-1, rand16());
    delay_ms(1);
    dsp_fill_rect(dsp_width/2, dsp_height/2, dsp_width-1, dsp_height-1, i);
    delay_ms(1);
  }
  delay_ms(1e3);
  // --
  dsp_fill_screen(DSP_BLACK);
  for(uint32_t i=0;i<1e4L;i++)
  {
    for(uint8_t j=0;j<2;j++)
    {
      dsp_draw_pixel(rand16()%dsp_width, rand16()%dsp_height, DSP_BLACK);
    }
    dsp_draw_pixel(rand16()%dsp_width, rand16()%dsp_height, rand16());
    delay_us(1);
  }
  delay_ms(1e3);
  // --
  for(uint8_t j=0;j<16;j++)
  {
    for(uint16_t i=0;i<dsp_width;i++)
    {
      dsp_draw_line(i,0,i,dsp_height-1,rand16());
      delay_us(10);
    }
  }
  delay_ms(1e3);
  // --
  dsp_fill_screen(DSP_BLACK);
  for(uint16_t i=0;i<2e3;i++)
  {
    dsp_draw_line(rand16()%dsp_width, rand16()%dsp_height,
                     rand16()%dsp_width, rand16()%dsp_height,
                     rand16());
    delay_us(100);
  }
  delay_ms(1e3);
  // --
  dsp_fill_screen(DSP_BLACK);
  dsp_set_rotation(0);
  for(uint8_t j=0;j<48;j++)
  {
    for(uint16_t i=0;i<dsp_width/2;i++)
    {
      dsp_draw_rect(i, i, dsp_width-(i<<1), dsp_height-(i<<1), rand16());
      delay_ms(1);
    }
  }
  delay_ms(1e3);
  // --
  dsp_fill_screen(DSP_BLACK);
  for(uint16_t i=0;i<2e3;i++)
  {
    dsp_draw_circle(rand16()%(dsp_width-40)+20, rand16()%(dsp_height-40)+20, rand16()>>10, rand16());
    delay_ms(1);
  }
  delay_ms(1e3);
  // --
  dsp_fill_screen(DSP_BLACK);
  for(uint16_t i=0;i<500;i++)
  {
    fillCircle(rand16()%(dsp_width-40)+20, rand16()%(dsp_height-40)+20, rand16()>>10, rand16());
    delay_ms(1);
  }
  delay_ms(1e3);
  // --
  for(uint16_t i=0;i<4;i++)
  {
    LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    // --
    dsp_set_rotation(i%4);
    dsp_fill_screen(DSP_BLACK);
    // --
    dsp_set_text_color(DSP_YELLOW);
    dsp_set_back_color(DSP_BLUE);
    dsp_set_font(&Font24);
    dsp_draw_char(10,10,'S');
    dsp_draw_char(27,10,('t'));
    dsp_draw_char(44,10,('m'));
    dsp_draw_char(61,10,('3'));
    dsp_draw_char(78,10,('2'));
    dsp_set_text_color(DSP_GREEN);
    dsp_set_back_color(DSP_RED);
    dsp_set_font(&Font20);
    dsp_draw_char(10,34,('S'));
    dsp_draw_char(24,34,('t'));
    dsp_draw_char(38,34,('m'));
    dsp_draw_char(52,34,('3'));
    dsp_draw_char(66,34,('2'));
    dsp_set_text_color(DSP_BLUE);
    dsp_set_back_color(DSP_YELLOW);
    dsp_set_font(&Font16);
    dsp_draw_char(10,54,('S'));
    dsp_draw_char(21,54,('t'));
    dsp_draw_char(32,54,('m'));
    dsp_draw_char(43,54,('3'));
    dsp_draw_char(54,54,('2'));
    dsp_set_text_color(DSP_CYAN);
    dsp_set_back_color(DSP_BLACK);
    dsp_set_font(&Font12);
    dsp_draw_char(10,70,('S'));
    dsp_draw_char(17,70,('t'));
    dsp_draw_char(24,70,('m'));
    dsp_draw_char(31,70,('3'));
    dsp_draw_char(38,70,('2'));
    dsp_set_text_color(DSP_RED);
    dsp_set_back_color(DSP_GREEN);
    dsp_set_font(&Font8);
    dsp_draw_char(10,82,('S'));
    dsp_draw_char(15,82,('t'));
    dsp_draw_char(20,82,('m'));
    dsp_draw_char(25,82,('3'));
    dsp_draw_char(30,82,('2'));
    delay_ms(1e3);
    // --
    dsp_fill_screen(DSP_BLACK);
    dsp_set_text_color(DSP_CYAN);
    dsp_set_back_color(DSP_BLACK);
    dsp_set_font(&Font24);
    dsp_string(1,100,"ABCDEF12345678");
    dsp_set_font(&Font20);
    dsp_string(1,124,"ABCDEFGHI12345678");
    dsp_set_font(&Font16);
    dsp_string(1,144,"ABCDEFGHIKL123456789");
    dsp_set_font(&Font12);
    dsp_string(1,160,"ABCDEFGHIKLMNOPQRSTUVWXY 123456789");
    dsp_set_font(&Font8);
    dsp_string(1,172,"ABCDEFGHIKLMNOPQRSTUVWXYZ 123456789ABCDEFGHIKL");
    delay_ms(1e3);
  }
  // --
}

static inline long
map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

unsigned int rainbow(int value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = blue through to red = blue
  // nt value = random (128);
  uint8_t red = 0; // Red is the top 5 bits of a 16 bit colour value
  uint8_t green = 0;// Green is the middle 6 bits
  uint8_t blue = 0; // Blue is the bottom 5 bits
  uint8_t quadrant = value / 32;
  if (quadrant == 0) 
  {
    blue = 31;
    green = 2 * (value % 32);
    red = 0;
  }
  if (quadrant == 1) 
  {
    blue = 31 - (value % 32);
    green = 63;
    red = 0;
  }
  if (quadrant == 2) 
  {
    blue = 0;
    green = 63;
    red = value % 32;
  }
  if (quadrant == 3) 
  {
    blue = 0;
    green = 63 - 2 * (value % 32);
    red = 31;
  }
  return (red << 11) + (green << 5) + blue;
}

void drawMandelbrot(void) 
{
  for (int px = 1; px < 320; px++)
  {
    for (int py = 0; py < 240; py++)
    {
      float x0 = (map(px, 0, 320, -250000/2, -242500/2)) / 100000.0; // caled x coordinate of pixel (scaled to lie in the Mandelbrot X scale (-2.5, 1))
      float yy0 = (map(py, 0, 240, -70000/4, -60000/4)) / 100000.0; // 00000.0; -75000/4, -61000/4 // caled y coordinate of pixel (scaled to lie in the Mandelbrot Y scale (-1, 1))
      float xx = 0.0;
      float yy = 0.0;
      int iteration = 0;
      int max_iteration = 128;
      while ( ((xx * xx + yy * yy) < 4) && (iteration < max_iteration) )
      {
        float xtemp = xx * xx - yy * yy + x0;
        yy = 2 * xx * yy + yy0;
        xx = xtemp;
        iteration++;
      }
      int color = rainbow((3*iteration)%128);
      dsp_draw_pixel(px, py, color);
    }
  }
}

void drawMandelbrotAlternative(const uint16_t scale, const uint16_t iterations, const float c_re_0, const float c_im_0)
{
  const uint16_t height = 240;
  const uint8_t halfHeight = 120;
  const uint16_t width = 320;
  const uint8_t halfWidth = 240;
  const float scaleFactor = 4.0 / width / scale;
  const float halfIterations = iterations / 2;

  uint16_t color;
  uint16_t color_inside;
  uint16_t color_midle;
  char isInside = 1;
  float c_re, c_im, z_re, z_re_temp, z_im_temp, z_im, n;
  uint16_t y, x;
  // --
  color = rand16();
  color_midle = rand16();
  color_inside = rand16();
  // --
  for (y = 0; y < height; y++) 
  {
    c_im = (y - halfHeight) * scaleFactor + c_im_0;
    for (x = 0; x < width; x++) 
    {
      c_re = (x - halfWidth) * scaleFactor + c_re_0;
      z_re = 0;
      z_re_temp = 0;
      z_im_temp = 0;
      z_im = 0;
      isInside = 1;
      for (n = 0; n < iterations; n++) 
      {
        z_re_temp = z_re * z_re;
        z_im_temp = z_im * z_im;
        if (z_re_temp + z_im_temp > 4) 
        {
          isInside = 0;
          break;
        }
        z_im = 2 * z_re * z_im + c_im;
        z_re = z_re_temp - z_im_temp + c_re;
      }
      if (isInside) 
      {
        dsp_draw_pixel(x, y, color_inside);
      }
      else if (n > halfIterations) 
      {
        dsp_draw_pixel(x, y, color_midle);
      }
      else 
      {
        dsp_draw_pixel(x, y, color);
      }
    }
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
#ifdef SEMIHOSTING_ENABLE
  initialise_monitor_handles();
#endif

    #ifdef SEMIHOSTING_ENABLE
    printf("\nhello printf\n");
    puts("hello puts");
    delay_ms(100);
    printf("hello printf\n");
    printf("hello world!\n");
    delay_ms(100);
    #endif
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  ticks_init(); // init chronometry
  ticks_set(0); // start chronometry
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  LL_DMA_DisableStream(DMA2, 3);
  LL_DMA_ClearFlag_TC3(DMA2);
  LL_DMA_ClearFlag_TE3(DMA2);
  LL_SPI_EnableDMAReq_TX(SPI1);
  LL_DMA_EnableIT_TC(DMA2, 3);
  LL_DMA_EnableIT_TE(DMA2, 3);
  LL_SPI_Enable(SPI1);
  dsp_ini(240, 320);
  
  uint16_t globalScale = 1;
  uint16_t iterations = 1;
  float c_re_0 = 0;
  float c_im_0 = 0;
    
  for(uint8_t i=0;i<20;i++)
  {
    LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    delay_ms(25);
  }
  // --
  dsp_fill_screen(DSP_BLACK);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  dsp_set_rotation(1);
  while (1)
  {
    test();
    drawMandelbrot();
    delay_ms(1000);

    for(uint8_t i=6;i<10;i++)
    {
      drawMandelbrotAlternative(globalScale, i+iterations++, c_re_0, c_im_0);
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_8, 84, LL_RCC_PLLP_DIV_2);
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
  LL_Init1msTick(84000000);
  LL_SetSystemCoreClock(84000000);
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/* USER CODE BEGIN 4 */
//-----------------------------------------------

//-----------------------------------------------
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
