// pwm_servo_atmega32.c
// ATMega32 PWM and Servo Control Library (replacement for Adafruit_PWMServoDriver and Arduino Servo)
// Provides basic PWM output for controlling hobby servos using ATMega32 timers.
//
// This file is a minimal, portable C library for generating servo PWM signals on ATMega32.
//
// Usage:
//   - Call pwm_servo_init() once at startup to configure Timer1 for 50Hz (20ms period)
//   - Use pwm_servo_set(channel, pulse_width_us) to set the pulse width for a given channel (0-3 for OC1A/OC1B/OC2/OC0)
//   - Connect servos to the corresponding output pins
//
// Note: ATMega32 has two 16-bit timers (Timer1, Timer3) and two 8-bit timers (Timer0, Timer2).
// This example uses Timer1 (OC1A/OC1B) for two servo channels. For more channels, use additional timers or external drivers.

#include <avr/io.h>
#include <stdint.h>

// Servo PWM parameters
#define SERVO_MIN_US  1000  // Minimum pulse width in microseconds (1ms)
#define SERVO_MAX_US  2000  // Maximum pulse width in microseconds (2ms)
#define SERVO_FREQ    50    // 50Hz (20ms period)

// PCA9685 I2C address (default 0x40)
#define PCA9685_ADDR 0x40

// PCA9685 register addresses
#define PCA9685_MODE1      0x00
#define PCA9685_PRESCALE   0xFE
#define PCA9685_LED0_ON_L  0x06

// Write a value to a PCA9685 register
void pca9685_write_reg(uint8_t reg, uint8_t value) {
    i2c_start((PCA9685_ADDR << 1) | 0); // Write mode
    i2c_write(reg);
    i2c_write(value);
    i2c_stop();
}

// Set PWM frequency (freq in Hz, e.g. 50 for servos)
void pca9685_set_pwm_freq(uint8_t freq) {
    // Calculate prescale value (see PCA9685 datasheet)
    uint8_t prescale = (uint8_t)(25000000.0 / (4096 * freq) - 1 + 0.5);
    // Go to sleep
    pca9685_write_reg(PCA9685_MODE1, 0x10);
    // Set prescale
    pca9685_write_reg(PCA9685_PRESCALE, prescale);
    // Wake up and auto-increment
    pca9685_write_reg(PCA9685_MODE1, 0x20);
    _delay_ms(1);
}

// Set PWM pulse for a channel (0-15), on/off counts (0-4095)
void pca9685_set_pwm(uint8_t channel, uint16_t on, uint16_t off) {
    i2c_start((PCA9685_ADDR << 1) | 0);
    i2c_write(PCA9685_LED0_ON_L + 4 * channel);
    i2c_write(on & 0xFF);
    i2c_write(on >> 8);
    i2c_write(off & 0xFF);
    i2c_write(off >> 8);
    i2c_stop();
}

// Initialize PCA9685 for servo use
void pca9685_init(void) {
    i2c_init();
    pca9685_write_reg(PCA9685_MODE1, 0x00); // Normal mode
    pca9685_set_pwm_freq(50); // 50Hz for servos
}

// Set servo pulse width in microseconds (us) for a channel (0-15)
void pca9685_set_servo_us(uint8_t channel, uint16_t pulse_us) {
    // Map 0-20000us to 0-4095 (20ms period)
    uint16_t off = (pulse_us * 4096) / 20000;
    if (off > 4095) off = 4095;
    pca9685_set_pwm(channel, 0, off);
}

// Initialize Timer1 for 50Hz PWM on OC1A (PD5) and OC1B (PD4)
void pwm_servo_init(void) {
    // Set PD5 (OC1A) and PD4 (OC1B) as output
    DDRD |= (1 << PD5) | (1 << PD4);
    // Set Timer1 to Fast PWM, TOP=ICR1, non-inverting mode
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler 8
    ICR1 = 19999; // 20ms period at 16MHz/8
    OCR1A = 1500; // Default 1.5ms pulse (neutral)
    OCR1B = 1500;
}

// Set servo pulse width in microseconds (us) for channel 0 (OC1A) or 1 (OC1B)
void pwm_servo_set(uint8_t channel, uint16_t pulse_width_us) {
    if (pulse_width_us < SERVO_MIN_US) pulse_width_us = SERVO_MIN_US;
    if (pulse_width_us > SERVO_MAX_US) pulse_width_us = SERVO_MAX_US;
    uint16_t ticks = (pulse_width_us * 2) - 1; // 1us = 2 ticks at 16MHz/8
    if (channel == 0) {
        OCR1A = ticks;
    } else if (channel == 1) {
        OCR1B = ticks;
    }
    // For more channels, use Timer2/Timer0 or external drivers
}

// Example: To set servo on OC1A to 0 degrees (1ms): pwm_servo_set(0, 1000);
//          To set servo on OC1A to 180 degrees (2ms): pwm_servo_set(0, 2000);
//          To set servo on OC1B to 90 degrees (1.5ms): pwm_servo_set(1, 1500);

// End of pwm_servo_atmega32.c
