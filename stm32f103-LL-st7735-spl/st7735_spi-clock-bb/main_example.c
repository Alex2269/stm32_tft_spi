#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h> // timer
#include <stm32f10x_rcc.h>
#include <misc.h> // interrupts
#include "lib/st7735/delay.h"
#include "lib/spi/port_io.h"
#include "lib/st7735/st7735.h"
#include "lib/st7735/garmin-digits.h"

void InitializeTimer()
{
  // see ARM Timer tutorial https://visualgdb.com/tutorials/arm/stm32/timers/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseInitTypeDef timerInitStructure;
  timerInitStructure.TIM_Prescaler = 12000; // the main clock is 72,000,000 so we'll prescale to 72,000,000 / 12,000 = 6,000 Hz
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 6000; // of the 6,000 Hz scaled clock, we actually only want 1/6000th of that (once per second)
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
}

void timeIncrement()
{
    seconds++;
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
  ST7735_PutStr5x7(0,0,"Hello world!",RGB565(255,0,0));
  ST7735_PutStr5x7(0,10,"This is 5x7 font",RGB565(0,255,0));
  ST7735_PutStr5x7(0,20,"Screen 128x160 pixels",RGB565(0,0,255));
  ST7735_Orientation(scr_normal);
  InitializeLED();
  InitializeTimer();
  EnableTimerInterrupt();
  while(1)
  {
  }
}
