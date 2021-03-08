#include "port_io.h"

void PINS_ini(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // Disable JTAG for use PB3

  GPIO_InitTypeDef PORT;
  PORT.GPIO_Mode = GPIO_Mode_Out_PP;
  PORT.GPIO_Speed = GPIO_Speed_50MHz;

  PORT.GPIO_Pin = ST7735_SDA_PIN;
  GPIO_Init(ST7735_SDA_PORT,&PORT);
  PORT.GPIO_Pin = ST7735_SCK_PIN;
  GPIO_Init(ST7735_SCK_PORT,&PORT);

  PORT.GPIO_Pin = ST7735_CS_PIN;
  GPIO_Init(ST7735_CS_PORT,&PORT);
  PORT.GPIO_Pin = ST7735_RST_PIN;
  GPIO_Init(ST7735_RST_PORT,&PORT);
  PORT.GPIO_Pin = ST7735_A0_PIN;
  GPIO_Init(ST7735_A0_PORT,&PORT);
}

void soft_spi_write(uint8_t data)
{
  GPIOB_LO = 1<<CS;
  uint8_t i;
  for(i = 0; i < 8; i++)
  {
    if (data & 0x80)  GPIOB_HI = 1<<SDA; else GPIOB_LO = 1<<SDA;
    data = data << 1;
    GPIOB_LO = 1<<SCK;
    GPIOB_HI = 1<<SCK;
  }
  GPIOB_HI = 1<<CS;
}
