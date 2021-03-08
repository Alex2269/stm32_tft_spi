#include "time.h"
#include "task.h"

uint32_t get_time_reload(void)
{
  return (*((volatile uint32_t *)0xe000e014)); // SYSTICK_LOAD_REG
}

uint32_t get_time_current(void)
{
  return (*((volatile uint32_t *)0xe000e018)); // SYSTICK_CURRENT_VALUE_REG
}

uint32_t get_time_us(void)
{
  volatile uint32_t scale = 1000000 / configTICK_RATE_HZ; /* microsecond */
  return (xTaskGetTickCount() * scale + (get_time_reload() - get_time_current()) * (1.0) / (get_time_reload() / scale)) ;
}

uint32_t get_time_ms(void)
{
  volatile uint32_t scale = 1000 / configTICK_RATE_HZ; /* microsecond */
  return (xTaskGetTickCount() * scale + (get_time_reload() - get_time_current()) * (1.0) / (get_time_reload() / scale)) ;
}

uint32_t get_time_sec(void)
{
  volatile uint32_t scale = 1000 / configTICK_RATE_HZ; /* microsecond */
  return (xTaskGetTickCount() * scale + (get_time_reload() - get_time_current()) * (1.0) / (get_time_reload() / scale)) / 1000;
}
