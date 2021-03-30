#ifndef __LL_SPI_IO_H
#define __LL_SPI_IO_H

#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32_tick.h"
#include <stdio.h>
#include <stdlib.h>

uint16_t rand16(void);
void chip_select(bool on);
void data_command(bool on);
void led(bool on);
void dma_stream(DMA_TypeDef* DMAx, uint32_t stream, uint8_t * buffer, uint32_t size);
void dsp_send_command(uint8_t cmd);
void dsp_send_data(uint8_t data);
void dsp_write_data(uint8_t* buff, size_t buff_size);
void dsp_reset(void);
void DMA2_Stream3_TransferComplete(void);

#endif /* __LL_SPI_IO_H */
