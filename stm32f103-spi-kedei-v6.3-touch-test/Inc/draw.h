#ifndef __draw_H
#define __draw_H

#include "stdbool.h"
#include "stdint.h"
#include "touch.h"
#include "kedei.h"
#include "xpt2046.h"
#include "utoa.h"
#include "adc.h"

#ifdef __cplusplus
 extern "C" {
#endif


void trigger_synhro_reset(void);
void oscil_run(void);
void fill_buff(uint16_t i);
void rotate_buff(uint16_t r);
void draw_statusbar(void);
void draw_pressure(uint16_t i);
void draw_max(uint16_t i);
void draw_ms(uint32_t ms_counter);
void draw_freq(uint32_t ms_counter);
void draw_graph(uint16_t i);
void draw_dot_grid(void);
void draw_grid(void);

#ifdef __cplusplus
}
#endif

#endif /* __draw_H */
