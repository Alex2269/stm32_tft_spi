#include "ll_ili9341_init.h"

extern uint16_t dsp_width;
extern uint16_t dsp_height;

void dsp_ini(uint16_t w_size, uint16_t h_size)
{
  uint8_t data[15];
  chip_select(0);
  dsp_reset();
  dsp_send_command(0x01); // Software Reset
  delay_ms(100);
  dsp_send_command(0xCF); dsp_write_data((uint8_t*)"\x00\xC1\x30", 3); // Power Control B
  dsp_send_command(0xED); dsp_write_data((uint8_t*)"\x64\x03\x12\x81", 4); // Power on Sequence control
  dsp_send_command(0xE8); dsp_write_data((uint8_t*)"\x85\x00\x78", 3); // Driver timing control A
  dsp_send_command(0xCB); dsp_write_data((uint8_t*)"\x39\x2C\x00\x34\x02", 5); // Power Control A
  dsp_send_command(0xEA); dsp_write_data((uint8_t*)"\x00\x00", 2); // Driver timing control B
  dsp_send_command(0xF7); dsp_write_data((uint8_t*)"\x20", 1); // Pump ratio control
  dsp_send_command(0xC0); dsp_write_data((uint8_t*)"\x10", 1); // Power Control,VRH[5:0]
  dsp_send_command(0xC1); dsp_write_data((uint8_t*)"\x10", 1); // Power Control,SAP[2:0];BT[3:0]
  dsp_send_command(0xC5); dsp_write_data((uint8_t*)"\x3E\x28", 2); // VCOM Control 1
  dsp_send_command(0xC7); dsp_write_data((uint8_t*)"\x86", 1); // VCOM Control 2
  dsp_send_command(0x36); dsp_write_data((uint8_t*)"\x48", 1); // Memory Acsess Control
  dsp_send_command(0x3A); dsp_write_data((uint8_t*)"\x55", 1); // Pixel Format Set 16bit
  dsp_send_command(0xB1); dsp_write_data((uint8_t*)"\x00\x18", 2); // Frame Rratio Control, Standard RGB Color
  dsp_send_command(0xB6); dsp_write_data((uint8_t*)"\x00\x82\x27\x00", 4); // Display Function Control
  dsp_send_command(0xF2); dsp_write_data((uint8_t*)"\x00", 1); // 3Gamma Function Disable
  dsp_send_command(0x26); dsp_write_data((uint8_t*)"\x01", 1); // Gamma curve selected
  dsp_send_command(0xE0); // Positive Gamma Correction
  dsp_write_data((uint8_t*)"\x0F\x31\x2B\x0C\x0E\x08\x4E\xF1\x37\x07\x10\x03\x0E\x09\x00", 15);
  dsp_send_command(0xE1); // Negative Gamma Correction
  dsp_write_data((uint8_t*)"\x00\x0E\x14\x03\x11\x07\x31\xC1\x48\x08\x0F\x0C\x31\x36\x0F", 15);
  dsp_send_command(0x11); // sleep out
  delay_ms(120);
  data[0] = DSP_ROTATION;
  dsp_send_command(0x29); // Display ON
  dsp_write_data(data, 1);
  dsp_width = w_size;
  dsp_height = h_size;
  dsp_fonts_ini();
}


