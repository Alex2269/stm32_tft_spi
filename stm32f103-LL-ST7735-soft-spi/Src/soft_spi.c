#include "soft_spi.h"
#include "main.h"

void SetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->BSRR, (PinMask >> GPIO_PIN_MASK_POS) & 0x0000FFFFU); 
}

void ResetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->BRR, (PinMask >> GPIO_PIN_MASK_POS) & 0x0000FFFFU);
}

void lcd_reset(void)
{
  ResetPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
  
  LL_mDelay(20);
  SetPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
  LL_mDelay(20);
}

// void spiWrite(uint8_t data)
// {
//   ResetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
//   uint8_t i;
//   for(i = 0; i < 8; i++)
//   {
//     if (data & 0x80)  SetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin); else ResetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
//     data = data << 1;
//     ResetPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin);  SetPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin);
//   }
//   SetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
// }

void spiWrite(uint8_t data)
{
  ResetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
  ResetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);

  uint8_t i, flagSet, pin = 0;
  for(i = 0; i < 8; i++)
  {

    (0x80 & data) ? (flagSet = 1 ) : (flagSet = 0);
    data = data << 1;

    if(flagSet)
    {
      if(!pin) SetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
      if(!pin) pin = 1;
    }

    if(flagSet==0)
    {
      if(pin) ResetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
      if(pin) pin = 0;
    }

    ResetPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin);
    SetPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin);
  }

  SetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
  SetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
}

// void spiWrite16(uint16_t data)
// {
//   ResetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
//   uint8_t i;
//   for(i = 0; i < 16; i++)
//   {
//     if (data & 0x8000)  SetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin); else ResetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
//     data = data << 1;
//     ResetPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin);  SetPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin);
//   }
//   SetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
// }

void spiWrite16(uint16_t data)
{
  ResetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
  ResetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);

  uint8_t i, flagSet, pin = 0;
  for(i = 0; i < 16; i++)
  {

    (0x8000 & data) ? (flagSet = 1 ) : (flagSet = 0);
    data = data << 1;

    if(flagSet)
    {
      if(!pin) SetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
      if(!pin) pin = 1;
    }

    if(flagSet==0)
    {
      if(pin) ResetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
      if(pin) pin = 0;
    }

    ResetPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin);
    SetPin(LCD_SCK_GPIO_Port, LCD_SCK_Pin);
  }

  SetPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
  SetPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
}

void writeCommand(uint8_t c)
{
  ResetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  spiWrite(c);
}

void writeData(uint8_t c)
{
  SetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  spiWrite(c);
}

void writeData16(uint16_t c)
{
  SetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  spiWrite16(c);
}

void write_data_array(uint8_t* buff)
{
  SetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  size_t len = sizeof(buff)/sizeof(buff[0]);
  for(uint8_t i=0; i<len;i++)
  {
    spiWrite(buff[i]);
  }
}

void write_data_array16(uint16_t* buff)
{
  SetPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
  size_t len = sizeof(buff)/sizeof(buff[0]);
  for(uint16_t i=0; i<len;i++)
  {
    spiWrite16(buff[i]);
  }
}

