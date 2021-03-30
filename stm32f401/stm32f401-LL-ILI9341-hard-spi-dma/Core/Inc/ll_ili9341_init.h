#ifndef __INIT_ILI9341_H
#define __INIT_ILI9341_H

#include "stdint.h"

#define DSP_MADCTL_MY  0x80
#define DSP_MADCTL_MX  0x40
#define DSP_MADCTL_MV  0x20
#define DSP_MADCTL_ML  0x10
#define DSP_MADCTL_RGB 0x00
#define DSP_MADCTL_BGR 0x08
#define DSP_MADCTL_MH  0x04
#define DSP_ROTATION (DSP_MADCTL_MX | DSP_MADCTL_BGR)
#define DSP_BLACK   0x0000
#define DSP_BLUE    0x001F
#define DSP_RED     0xF800
#define DSP_GREEN   0x07E0
#define DSP_CYAN    0x07FF
#define DSP_MAGENTA 0xF81F
#define DSP_YELLOW  0xFFE0
#define DSP_WHITE   0xFFFF

void dsp_ini(uint16_t w_size, uint16_t h_size);

#endif /* __INIT_ILI9341_H */


