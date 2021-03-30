//!
// based on:
// https://github.com/thebreadboard/SampleSketches
//
// software i2c from:
// https://github.com/ElektorLabs/ads1115-driver
// ADS1115_Test

// Include the bit-banged I2C driver.
#include "soft_i2c.h"

#define ASD1115 0x48

unsigned int val = 0;
byte writeBuf[3];
byte buffer[3];
const float VPS = 4.096 / 32768.0; // volts per step
int sda_pin = A4;
int scl_pin = A5;

// You must provide this function.
void soft_i2c_delay(void)
{
  delayMicroseconds(5);
}

// You must provide this function.
void soft_i2c_sda_mode(uint8_t value)
{
  if (value==0) pinMode(sda_pin,INPUT); // Use pull-ups on bus.
  else pinMode(sda_pin,OUTPUT);
}

// You must provide this function.
void soft_i2c_sda_write(uint8_t value)
{
  digitalWrite(sda_pin,value);
}

// You must provide this function.
uint8_t soft_i2c_sda_read(void)
{
  return digitalRead(sda_pin);
}

// You must provide this function.
void soft_i2c_scl_write(uint8_t value)
{
  digitalWrite(scl_pin,value);
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("ADS1115 ADC board");
  Serial.println("");
  
  // Setup bit-banged I2C pins.
  pinMode(sda_pin,OUTPUT);
  pinMode(scl_pin,OUTPUT);

  // ASD1115
  // set config register and start conversion
  // ANC1 and GND, 4.096v, 128s/s
  writeBuf[0] = 1;    // config register is 1
  writeBuf[1] = 0b11010010; // 0xC2 single shot off
  // bit 15 flag bit for single shot
  // Bits 14-12 input selection:
  // 100 ANC0; 101 ANC1; 110 ANC2; 111 ANC3
  // Bits 11-9 Amp gain. Default to 010 here 001 P19
  // Bit 8 Operational mode of the ADS1115.
  // 0 : Continuous conversion mode
  // 1 : Power-down single-shot mode (default)

  writeBuf[2] = 0b10000101; // bits 7-0  0x85
  // Bits 7-5 data rate default to 100 for 128SPS
  // Bits 4-0  comparator functions see spec sheet.

// setup ADS1115
  soft_i2c_start();
  soft_i2c_write(ASD1115<<1);
// soft_i2c_start_write(ASD1115);
  soft_i2c_write(writeBuf[0]);
  soft_i2c_write(writeBuf[1]);
  soft_i2c_write(writeBuf[2]);
  soft_i2c_stop();
  delay(300);
}

void loop() {
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write(ASD1115<<1);
  // soft_i2c_start_write(ASD1115);   // Wire.beginTransmission(ASD1115); // DAC
  soft_i2c_write(0x0);                // Wire.write(buffer[0]); // pointer
  // soft_i2c_stop();                 // Wire.endTransmission();

  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write((ASD1115<<1)|0x01);
  // soft_i2c_start_read(ASD1115, 2); // Wire.requestFrom(ASD1115, 2);
  buffer[1] = soft_i2c_read();        // buffer[1] = Wire.read();
  buffer[2] = soft_i2c_read();        // buffer[2] = Wire.read();
  soft_i2c_stop();                    // Wire.endTransmission();

  // convert display results
  val = buffer[1] << 8 | buffer[2]; 

  if (val > 32768) val = 0;

  Serial.println(val * VPS);
  Serial.println(val);
  
  // just an indicator
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
  delay(50);
}
