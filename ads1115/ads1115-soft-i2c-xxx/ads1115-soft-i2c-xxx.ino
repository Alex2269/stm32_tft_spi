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
const float VPS = 6.144 / 32768.0; // volts per step
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
  delay(300);
}

void readiness_a0(void)
{
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write(ASD1115<<1);
  soft_i2c_write(0x01);
  soft_i2c_write(0xC1);
  soft_i2c_write(0x83);
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write(ASD1115<<1);
  soft_i2c_write(0x0);  
}

void readiness_a1(void)
{
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write(ASD1115<<1);
  soft_i2c_write(0x01);
  soft_i2c_write(0xD1);
  soft_i2c_write(0x83);
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write(ASD1115<<1);
  soft_i2c_write(0x0);  
}

void readiness_a2(void)
{
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write(ASD1115<<1);
  soft_i2c_write(0x01);
  soft_i2c_write(0xE1);               // E1 ch A2
  soft_i2c_write(0x83);
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write(ASD1115<<1);
  soft_i2c_write(0x0);  
}

void readiness_a3(void)
{
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write(ASD1115<<1);
  soft_i2c_write(0x01);
  soft_i2c_write(0xF1);               // E1 ch A2
  soft_i2c_write(0x83);
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write(ASD1115<<1);
  soft_i2c_write(0x0);  
}

void asd1115_read(void)
{
  soft_i2c_start();                   // Wire.begin(); // begin I2C
  soft_i2c_write((ASD1115<<1)|0x01);
  // soft_i2c_start_read(ASD1115, 2); // Wire.requestFrom(ASD1115, 2);
  buffer[1] = soft_i2c_read();        // buffer[1] = Wire.read();
  buffer[2] = soft_i2c_read();        // buffer[2] = Wire.read();
  soft_i2c_stop();                    // Wire.endTransmission();
}

void loop() {
  single_a1();
  asd1115_read();

  // convert display results
  val = buffer[1] << 8 | buffer[2]; 

  if (val > 32768) val = 0;

  Serial.println(val * VPS);
  // Serial.println(val);
  
  // just an indicator
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
  delay(50);
}
