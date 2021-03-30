# st7735_spi_stm32-clock-bb

``` cpp
ST7735 Display Driver from https://github.com/LonelyWolf/stm32/tree/master/ST7735

see configuration in lib/spi/port_io.h
display connect for stm32f103c8t6 board:

ST7735 SCK (CLOCK)       GPIOB Pin_5
ST7735 SDA (MOSI/DATA)   GPIOB Pin_6
ST7735 A0 (Data/Command) GPIOB Pin_7
ST7735 RST (Reset)       GPIOB Pin_8
ST7735 CS  (Chip Select) GPIOB Pin_9

compile:
cmake .
make
make flash
```
