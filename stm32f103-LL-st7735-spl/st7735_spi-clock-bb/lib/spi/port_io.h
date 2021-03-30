#ifndef __PORTIO_H__
#define __PORTIO_H__

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

#define	SCK 5 // GPIO_Pin_5
#define	SDA 6 // GPIO_Pin_6
#define	DC  7 // GPIO_Pin_7
#define	RST 8 // GPIO_Pin_8
#define CS  9 // GPIO_Pin_9

#define GPIO_BASE_PIN 0x0001
#define	ST7735_SCK_PIN     GPIO_BASE_PIN<<SCK
#define	ST7735_SDA_PIN     GPIO_BASE_PIN<<SDA
#define ST7735_A0_PIN      GPIO_BASE_PIN<<DC
#define ST7735_RST_PIN     GPIO_BASE_PIN<<RST
#define ST7735_CS_PIN      GPIO_BASE_PIN<<CS

#define ST7735_SCK_PORT    GPIOB
#define ST7735_SDA_PORT    GPIOB
#define ST7735_A0_PORT     GPIOB
#define ST7735_RST_PORT    GPIOB
#define ST7735_CS_PORT     GPIOB

#define GPIOA_HI (*(volatile unsigned long*)(GPIOA_BASE + 0x10))
#define GPIOB_HI (*(volatile unsigned long*)(GPIOB_BASE + 0x10))
#define GPIOC_HI (*(volatile unsigned long*)(GPIOC_BASE + 0x10))
#define GPIOD_HI (*(volatile unsigned long*)(GPIOD_BASE + 0x10))
#define GPIOE_HI (*(volatile unsigned long*)(GPIOE_BASE + 0x10))
#define GPIOA_LO (*(volatile unsigned long*)(GPIOA_BASE + 0x14))
#define GPIOB_LO (*(volatile unsigned long*)(GPIOB_BASE + 0x14))
#define GPIOC_LO (*(volatile unsigned long*)(GPIOC_BASE + 0x14))
#define GPIOD_LO (*(volatile unsigned long*)(GPIOD_BASE + 0x14))
#define GPIOE_LO (*(volatile unsigned long*)(GPIOE_BASE + 0x14))

// Screen resolution in normal orientation
#define scr_w         128
#define scr_h         160

void PINS_ini(void);
void soft_spi_write(uint8_t data);

#endif //__PORTIO_H__


