#ifndef __touch_H
#define __touch_H

#include "stdbool.h"
#include "stdint.h"
#include "draw.h"
#include "kedei.h"
#include "xpt2046.h"
#include "utoa.h"
#include "adc.h"

#ifdef __cplusplus
 extern "C" {
#endif

void trigger_synhro_on(void);
void frequency_measure(void);
void trigger_synhro_off(void);
void test_touch(void);
void trigger_amplitude(void);
void draw_trigger(uint32_t trg);
void draw_touch(uint16_t coord_X, uint16_t coord_Y, uint16_t coord_Z); // for text data

#ifdef __cplusplus
}
#endif

#endif /* __touch_H */
