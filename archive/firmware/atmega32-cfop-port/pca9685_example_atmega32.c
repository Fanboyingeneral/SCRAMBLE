// pca9685_example_atmega32.c
// Example: Using ATMega32 to control servos via Adafruit PCA9685 board over I2C
//
// This example demonstrates how to initialize the PCA9685 and set servo positions using the interface provided in pwm_servo_atmega32.c
//
// Hardware:
//   - ATMega32 MCU
//   - Adafruit PCA9685 16-channel PWM/Servo Driver Board
//   - Servos connected to PCA9685 outputs
//   - I2C (SCL/SDA) connected between ATMega32 and PCA9685
//
// Connections:
//   - ATMega32 SDA (PC1) -> PCA9685 SDA
//   - ATMega32 SCL (PC0) -> PCA9685 SCL
//   - Both boards share GND and 5V (or 3.3V if compatible)
//
// Note: Requires pwm_servo_atmega32.c and pca9685_atmega32.c in your project

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "pwm_servo_atmega32.c" // Includes PCA9685 interface and I2C routines

int main(void) {
    // Initialize PCA9685 and I2C
    pca9685_init();

    while (1) {
        // Sweep servo on channel 0 from 0 to 180 degrees
        for (uint16_t angle = 0; angle <= 180; angle += 10) {
            // Map angle to pulse width (1000us to 2000us)
            uint16_t pulse = SERVO_MIN_US + (angle * (SERVO_MAX_US - SERVO_MIN_US)) / 180;
            pca9685_set_servo_us(0, pulse); // Channel 0
            _delay_ms(200);
        }
        // Sweep back
        for (int16_t angle = 180; angle >= 0; angle -= 10) {
            uint16_t pulse = SERVO_MIN_US + (angle * (SERVO_MAX_US - SERVO_MIN_US)) / 180;
            pca9685_set_servo_us(0, pulse);
            _delay_ms(200);
        }
        // Hold at center for 1 second
        pca9685_set_servo_us(0, 1500);
        _delay_ms(1000);
    }
}

// End of pca9685_example_atmega32.c
