/*
 * soft_i2c, software i2c for stm32
 * 
 * see original code:
 * https://github.com/electronicayciencia/wPi_soft_i2c
 * autor Reinoso G.
 */

#include <stdio.h> 
#include <stdint.h>
#include <gpio.h>
// #include "stm32f1xx_hal.h"
#include "pins_ext.h"
#include "soft_i2c.h"

#define LOW 0

void delayMicroseconds(uint32_t cycle_count)
{
  uint32_t count=0;
  for(count=0;count<cycle_count;count++)
  {
    __asm volatile ("nop");
  }
}

/* Pull: drives the line to level LOW */
void _i2c_pull(GPIO_TypeDef* GPIOx, int pin) {
    pin_write(GPIOx, pin, LOW);
	delayMicroseconds((1e6/I2C_FREQ)/2);
}

/* Release: releases the line and return line status */
int _i2c_release(GPIO_TypeDef* GPIOx, int pin) {
	delayMicroseconds((1e6/I2C_FREQ)/2);
	return pin_read(GPIOx, pin);
}

/* In case of clock stretching or busy bus we must wait */
/* But not wait forever though */
void _i2c_release_wait(GPIO_TypeDef* GPIOx, int pin) {
	int n = 0;

	delayMicroseconds((1e6/I2C_FREQ)/2);
	while (!pin_read(GPIOx, pin)) {
		if (++n >= 50)	{
			if (WARN) fprintf(stderr, "Warning: I2C Bus busy or defective. Pin %d is LOW for 5s.\n", pin);
			return;
		}
		// HAL_Delay(100);
        delayMicroseconds(1e6);
	}
	delayMicroseconds((1e6/I2C_FREQ)/2);
}

/* Initializes software emulated i2c */
i2c_t i2c_init(GPIO_TypeDef* _GPIOx, int _scl, int _sda) {
	i2c_t port;

	port.scl = _scl;
	port.sda = _sda;
	port.GPIOx = _GPIOx;

    pin_pulse_hi_low(port.GPIOx, port.scl, 100);
    pin_pulse_hi_low(port.GPIOx, port.sda, 100);

	i2c_reset(port);

	return port;
}

/* Start: pull SDA while SCL is up*/
/* Best practice is to ensure the bus is not busy before start */
void i2c_start(i2c_t port) {
	if (!_i2c_release(port.GPIOx, port.sda)) 
		i2c_reset(port);
    _i2c_release_wait(port.GPIOx, port.scl);

	_i2c_pull(port.GPIOx, port.sda);
	_i2c_pull(port.GPIOx, port.scl);
}

/* Stop: release SDA while SCL is up */
void i2c_stop(i2c_t port) {
	_i2c_release_wait(port.GPIOx, port.scl);
	if (!_i2c_release(port.GPIOx, port.sda))
		i2c_reset(port);
}

/* Reset bus sequence */
void i2c_reset(i2c_t port) {
	int i;
	int n = 0;

	_i2c_release(port.GPIOx, port.sda);

	do {
		for (i = 0; i < 10; i++) {
			_i2c_pull(port.GPIOx, port.scl);
			_i2c_release(port.GPIOx, port.scl);
		}
		if (++n >= 100) {
			if (WARN) fprintf(stderr, "Warning: I2C Bus busy or defective. SDA doesn't go UP after reset.\n");
			return;
		}
		// HAL_Delay(10);
        delayMicroseconds(1e5);
	} while (!pin_read(port.GPIOx, port.sda));

	_i2c_pull(port.GPIOx, port.scl);
	_i2c_pull(port.GPIOx, port.sda);

	i2c_stop(port);
}

/* Sends 0 or 1: 
 * Clock down, send bit, clock up, wait, clock down again 
 * In clock stretching, slave holds the clock line down in order
 * to force master to wait before send more data */
void i2c_send_bit(i2c_t port, int bit) {
	if (bit)
		_i2c_release(port.GPIOx, port.sda);
	else
		_i2c_pull(port.GPIOx, port.sda);

	_i2c_release_wait(port.GPIOx, port.scl);
	_i2c_pull(port.GPIOx, port.scl);

	_i2c_pull(port.GPIOx, port.sda);
}

/* Reads a bit from sda */
int i2c_read_bit(i2c_t port) {
	int s;

	_i2c_release(port.GPIOx, port.sda);
	_i2c_release_wait(port.GPIOx, port.scl);
	s = pin_read(port.GPIOx, port.sda);
	_i2c_pull(port.GPIOx, port.scl);
	_i2c_pull(port.GPIOx, port.sda);

	return s;
}

/* Sends 8 bit in a row, MSB first and reads ACK.
 * Returns I2C_ACK if device ack'ed */
int i2c_send_byte(i2c_t port, uint8_t byte) {
	int i;

	for (i = 0; i < 8; i++) {
		i2c_send_bit(port, byte & 0x80);
		byte = byte << 1;
	}

	return i2c_read_bit(port);
}

/* Reads a byte, MSB first */
uint8_t i2c_read_byte(i2c_t port) {
	int byte = 0x00;
	int i;

	for (i=0; i<8; i++) 
		byte = (byte << 1) | i2c_read_bit(port);

	return byte;
}

