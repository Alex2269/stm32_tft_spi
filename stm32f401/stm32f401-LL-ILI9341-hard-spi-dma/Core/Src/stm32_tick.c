#include "stm32_tick.h"

uint32_t SysTick_config(uint32_t reload)
{
  if ((reload - 1UL) > 10e6L)
  {
    return (1UL); /* Reload value impossible */
  }

  NVIC_SetPriority(SysTick_IRQn, 7UL);  /* set Priority for Systick Interrupt */
  SysTick->CTRL = 0b0110;
  SysTick->LOAD = reload;
  // SysTick->VAL  = 0x0UL;
  SysTick->CTRL = 0b0111;

  return (0UL);                         /* Function successful */
}

void ticks_init()
{
  SCB_DEMCR   |= 0x01000000;
  DWT_CONTROL &= ~1; // disable the counter
  DWT_CYCCNT   = 0xF0000000ul;
  DWT_CONTROL |= 1; // enable the counter
}

void delay_next_us(uint16_t time_us)
{
  static uint32_t old_time = 0;
  uint32_t time = DWT_CYCCNT;
  uint32_t limit = (((uint64_t)GetSystemClockHz()*time_us) / 1000000);

  while ((time - old_time) < limit)
    time = DWT_CYCCNT;
  old_time = time;
}

void __delay_ms(uint32_t time_ms)
{
  while (time_ms--)
    delay_next_us(1000);
}

void delay_ms(uint32_t time_ms)
{
  delay_next_us(0);
  __delay_ms(time_ms);
}

void delay_seconds(uint16_t seconds)
{
  delay_next_us(0);
  while (seconds--)
    __delay_ms(1000);
}
