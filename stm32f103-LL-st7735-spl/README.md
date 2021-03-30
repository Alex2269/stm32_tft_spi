# st7735_spi-clock-bb


``` cpp

# http://www.st.com/en/embedded-software/stm32-standard-peripheral-libraries.html?querycriteria=productId=LN1939

# STM32F10x_StdPeriph_Lib_V3.5.0

depends: make cmake gcc g++ gcc-arm-none-eabi gdb-arm-none-eabi libusb-dev

wget http://www.st.com/content/ccc/resource/technical/software/firmware/48/ab/e5/17/0d/79/43/74/stsw-stm32054.zip/files/stsw-stm32054.zip/jcr:content/translations/en.stsw-stm32054.zip -O en.stsw-stm32054.zip

StdPeriph_Lib=~/stm32lib/
mkdir  -p "${StdPeriph_Lib}"
unzip  en.stsw-stm32054.zip -d "${StdPeriph_Lib}"
rm en.stsw-stm32054.zip

```
