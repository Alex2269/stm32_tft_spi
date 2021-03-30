#ifndef _TIME_H
#define _TIME_H

#include "FreeRTOS.h"
#include "stdbool.h"

uint32_t get_time_reload(void);
uint32_t get_time_current(void);
uint32_t get_time_us(void);
uint32_t get_time_ms(void);
uint32_t get_time_sec(void);

#endif // _TIME_H




