#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h> // timer
#include <stm32f10x_rcc.h>
#include <misc.h> // interrupts
#include "lib/st7735/delay.h"
#include "lib/spi/port_io.h"
#include "lib/st7735/st7735.h"
#include "lib/st7735/garmin-digits.h"
#include <math.h>

#define M_PI 3.14159

void InitializeTimer()
{
  // see ARM Timer tutorial https://visualgdb.com/tutorials/arm/stm32/timers/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseInitTypeDef timerInitStructure;
  timerInitStructure.TIM_Prescaler = 7200; // the main clock is 72,000,000 so we'll prescale to 72,000,000 / 7,200 = 10,000 Hz
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 1000; // of the TIM_Prescaler/1000 scaled clock, we actually have (10 tick per second)
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &timerInitStructure);
  TIM_Cmd(TIM2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // enable timter interrupt updates
}

void InitializeLED()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void EnableTimerInterrupt(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

int hours = 20, minutes = 51, seconds = 30, mseconds = 0;

void timeToDigits()
{
  ST7735_BigDig(hours/10, 0, 35, RGB565(212, 246, 190));
  ST7735_BigDig(hours%10, 16, 35, RGB565(212, 246, 190));
  ST7735_FillRect(33, 42, 35, 44, RGB565(177, 211, 190));
  ST7735_FillRect(33, 67, 35, 69, RGB565(177, 211, 190));
  ST7735_BigDig(minutes/10, 37, 35, RGB565(212, 246, 190));
  ST7735_BigDig(minutes%10, 53, 35, RGB565(212, 246, 190));
  ST7735_FillRect(70, 42, 72, 44, RGB565(177, 211, 190));
  ST7735_FillRect(70, 67, 72, 69, RGB565(177, 211, 190));
  ST7735_BigDig(seconds/10, 75, 35, RGB565(212, 246, 190));
  ST7735_BigDig(seconds%10, 91, 35, RGB565(212, 246, 190));
  ST7735_MidDig(mseconds, 110, 55, RGB565(212, 246, 190));
}

void timeIncrement()
{
  mseconds++;
  if(mseconds >= 10)
  {
    mseconds=0;
    seconds++;
  }
  if (seconds >= 60)
  {
    seconds = 0; // the 60'th second is actually 0
    minutes++;
    if (minutes >= 60)
    {
      minutes = 0; // the 60'th minute is actually 0
      hours++;
      if (hours >= 24)
      {
        hours = 0; // the 24'th hour is actually 0
      }
    }
  }
}

volatile int ledStatus = 0;

//extern "C" void TIM2_IRQHandler() //Note that you only need extern C if you are building a C++ program.
void TIM2_IRQHandler() //Note that you only need extern C if you are building a C++ program.
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    // GPIO_ToggleBits(GPIOC, GPIO_Pin_13); // appears to be no ToggleBits for stm32f10x, so we'll do this a bit more manually with ledStatus
    if (ledStatus == 0)
    {
      ledStatus = 1;
      GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
    }
    else
    {
      ledStatus = 0;
      GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
    }
  }
  timeIncrement();
  timeToDigits();
}

int main(void)
{
  ST7735_Init();
  ST7735_AddrSet(0,0,159,127);
  ST7735_Clear(0x0000);
  ST7735_Orientation(scr_normal);

  ST7735_PutStr5x7(0,0,"Hello world!",RGB565(255,0,0));
  ST7735_PutStr5x7(0,10,"This is 5x7 font",RGB565(0,255,0));
  ST7735_PutStr_asc2_1608(0,20,"128x160",RGB565(255,255,0));
  ST7735_PutStr_SmallFont(0,85,"128x160",RGB565(255,255,255));

  for(uint16_t x = 2; x<160; x+=16) for(uint16_t i = 2; i<128; i+=16) ST7735_Pixel( i, x, RGB565(0,255,255)); //draw point grid
  for(uint16_t x = 2; x<160; x+=32) for(uint16_t i = 2; i<128; i+=32) ST7735_Pixel( i, x, RGB565(255,0,255)); //draw point grid

  uint16_t tft_W = 160;

  uint16_t array_sin[tft_W];
  uint16_t array_cos[tft_W];
  for(uint16_t i = 0; i < tft_W; i++) // count point
  {
    array_cos[i]=(uint16_t)24*cos(30*0.5*M_PI*i/180)+125;
    array_sin[i]=(uint16_t)16*sin(30*0.2*M_PI*i/180)+125;
  }

  for(uint16_t i = 1; i < tft_W; i++)
  {
    ST7735_Line(i, array_cos[i],i-1,array_cos[i-1], RGB565(255,255,0));
    ST7735_Line(i, array_sin[i],i-1,array_sin[i-1], RGB565(0,255,255));
  }

  InitializeLED();
  InitializeTimer();
  EnableTimerInterrupt();
  while(1)
  {
  }
}
