#ifndef __draw_H
#define __draw_H

#include "stdint.h"

#ifdef __cplusplus
 extern "C" {
#endif

void draw_grid(void);
void fill_buff(uint16_t i);
void write_graph(uint16_t i);
void fill_text(uint16_t i);

void draw_touch(uint16_t coord_X, uint16_t coord_Y, uint16_t coord_Z);
void oscil_init(void);
void oscil_run(void);
void rotate_buff(uint16_t r);
void write_text(uint16_t i);
void write_grid(void);

#ifdef __cplusplus
}
#endif

#endif /* __draw_H */
