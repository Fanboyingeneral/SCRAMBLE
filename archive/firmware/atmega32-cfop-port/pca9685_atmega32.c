// pca9685_atmega32.c
// ATMega32 I2C (TWI) routines and PCA9685 servo driver interface
// Step 1: Basic I2C (TWI) master routines for ATMega32
//
// This file provides basic I2C master functions for ATMega32, which are needed to communicate with the PCA9685 servo driver board.
//
// Usage:
//   - Call i2c_init() once at startup
//   - Use i2c_start(addr), i2c_write(data), i2c_stop() to send data
//
// Next step: Add PCA9685-specific functions (set PWM frequency, set PWM value)

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define F_CPU 16000000UL
#define I2C_FREQ 100000UL // 100kHz standard
#define TWBR_VAL ((((F_CPU / I2C_FREQ) - 16 ) / 2))

void i2c_init(void) {
    // Set bit rate
    TWBR = (uint8_t)TWBR_VAL;
    // No prescaler
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
}

// Send START condition and slave address (write mode)
uint8_t i2c_start(uint8_t addr) {
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    // Send address
    TWDR = addr;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    // Check status (should be 0x18 for SLA+W transmitted, ACK received)
    uint8_t status = TWSR & 0xF8;
    return (status == 0x18);
}

// Write a byte
uint8_t i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    // Check status (should be 0x28 for data transmitted, ACK received)
    uint8_t status = TWSR & 0xF8;
    return (status == 0x28);
}

// Send STOP condition
void i2c_stop(void) {
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
    _delay_us(10);
}

// End of Step 1: Basic I2C routines
// Next: Add PCA9685 register interface
