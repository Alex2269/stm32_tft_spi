#ifndef __LL_SPI_IO_H
#define __LL_SPI_IO_H

#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_dma.h"
#include "ll_spi_dma_io.h"
#include "stm32_tick.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


uint32_t dma_spi_part=1;
extern uint8_t frame_buffer[];

uint16_t rand16(void)
{
  // ticks_set(0); // start chronometry
  uint16_t rnd = random()*ticks_get();
  return rnd;
}

void chip_select(bool on)
{
  if(on) SetPin(TFT_CS_GPIO_Port, TFT_CS_Pin);
  else ResetPin(TFT_CS_GPIO_Port, TFT_CS_Pin);
}

void data_command(bool on)
{
  if(on) SetPin(TFT_DC_GPIO_Port, TFT_DC_Pin);
  else ResetPin(TFT_DC_GPIO_Port, TFT_DC_Pin);
}

void led(bool on)
{
  if(on) ResetPin(TFT_LED_GPIO_Port, TFT_LED_Pin);
  else SetPin(TFT_LED_GPIO_Port, TFT_LED_Pin);
}

void dma_stream(DMA_TypeDef* DMAx, uint32_t stream, uint8_t * buffer, uint32_t size)
{
  LL_DMA_DisableStream(DMAx, stream);
  LL_DMA_SetDataLength(DMAx, stream, size);
  LL_DMA_ConfigAddresses(DMAx, stream,
                         (uint32_t)buffer, LL_SPI_DMA_GetRegAddr(SPI1),
                         LL_DMA_GetDataTransferDirection(DMAx, stream));
  LL_DMA_EnableStream(DMAx, stream);
  // --
  while(LL_DMA_IsEnabledStream(DMAx, stream)) {}
  delay_us(1);
}

void dsp_send_command(uint8_t cmd)
{
  frame_buffer[0] = cmd;
  chip_select(0);
  data_command(0);
  dma_stream(DMA2, LL_DMA_STREAM_3, frame_buffer, 1);
  chip_select(1);
}

void dsp_send_data(uint8_t data)
{
  frame_buffer[0] = data;
  chip_select(0);
  data_command(1);
  dma_stream(DMA2, LL_DMA_STREAM_3, frame_buffer, 1);
  chip_select(1);
}

void dsp_write_data(uint8_t* buff, size_t buff_size)
{
  chip_select(0);
  data_command(1);
  dma_stream(DMA2, LL_DMA_STREAM_3, buff, buff_size);
  chip_select(1);
}

void dsp_reset(void)
{
  led(0);
  ResetPin(TFT_RST_GPIO_Port, TFT_RST_Pin);
  LL_mDelay(20);
  SetPin(TFT_RST_GPIO_Port, TFT_RST_Pin);
  LL_mDelay(20);
  led(1);
}

void DMA2_Stream3_TransferComplete(void)
{
  LL_DMA_ClearFlag_TC3(DMA2);
  dma_spi_part--;
  if(dma_spi_part==0)
  {
    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_3);
    dma_spi_part=1;
  }
}

#endif /* __LL_SPI_IO_H */
