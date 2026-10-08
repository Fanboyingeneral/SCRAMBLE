/*
 * ATmega32 Rubik's Cube Solver Motor Test
 *
 * This program controls four SG90 servo motors connected to the hardware
 * PWM pins of an ATmega32. It initializes all motors to a 90-degree
 * position, waits for 15 seconds, and then performs a test sequence
 * on each motor one by one (90 -> 0 -> 90 degrees).
 *
 * MCU:   ATmega32
 * Clock: 1MHz Internal Oscillator
 * Pins:
 *  - Front Motor: PD4 (OC1B)
 *  - Back Motor:  PD5 (OC1A)
 *  - Left Motor:  PD7 (OC2)
 *  - Right Motor: PB3 (OC0)
 */

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

// --- Calibration Values ---

// Front Motor (PD4 - Timer1B)
#define FRONT_MOTOR_0_DEG   64
#define FRONT_MOTOR_90_DEG  191

// Back Motor (PD5 - Timer1A)
#define BACK_MOTOR_0_DEG    78
#define BACK_MOTOR_90_DEG   204

// Left Motor (PD7 - Timer2)
#define LEFT_MOTOR_0_DEG    7
#define LEFT_MOTOR_90_DEG   24

// Right Motor (PB3 - Timer0)
// Note: The user-provided value 22.5 is rounded to 23.
#define RIGHT_MOTOR_0_DEG   7
#define RIGHT_MOTOR_90_DEG  23


// --- Delays ---
#define INITIAL_DELAY_MS 15000 // Time to place the cube
#define MOVE_DELAY_MS    1000  // Delay between motor movements


/**
 * @brief Configures the DDR registers for the motor pins.
 */
void setup_pins(void) {
    // Set motor pins as outputs
    DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD7);
    DDRB |= (1 << PB3);
}

/**
 * @brief Configures Timer0, Timer1, and Timer2 for servo PWM.
 */
void setup_timers(void) {
    // --- Timer 1 (16-bit) for Front (OC1B) and Back (OC1A) Motors ---
    // Mode 14: Fast PWM with ICR1 as TOP.
    // We are matching the ~61Hz frequency of the 8-bit timers for consistency.
    // Prescaler = 8 -> F_PWM = 1MHz / 8 / 2048 = ~61Hz
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1); // Non-inverting on OC1A/OC1B
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);    // Prescaler 8
    ICR1 = 2048; // TOP value for ~61Hz

    // --- Timer 2 (8-bit) for Left Motor (OC2) ---
    // Fast PWM, non-inverting, Prescaler 64
    // F_PWM = 1MHz / 64 / 256 = ~61Hz
    TCCR2 = (1 << WGM21) | (1 << WGM20) | (1 << COM21) | (1 << CS22);

    // --- Timer 0 (8-bit) for Right Motor (OC0) ---
    // Fast PWM, non-inverting, Prescaler 64
    // F_PWM = 1MHz / 64 / 256 = ~61Hz
    TCCR0 = (1 << WGM01) | (1 << WGM00) | (1 << COM01) | (1 << CS01) | (1 << CS00);
}

int main(void) {
    setup_pins();
    setup_timers();

    // --- Initial Position (90 Degrees) ---
    OCR1B = FRONT_MOTOR_90_DEG; // Front
    OCR1A = BACK_MOTOR_90_DEG;  // Back
    OCR2  = LEFT_MOTOR_90_DEG;  // Left
    OCR0  = RIGHT_MOTOR_90_DEG; // Right

    // --- Wait for user to place the cube ---
    _delay_ms(INITIAL_DELAY_MS);

    // --- Main Loop for Test Sequence ---
    while (1) {
        // 1. Front Motor (PD4)
        OCR1B = FRONT_MOTOR_0_DEG;
        _delay_ms(MOVE_DELAY_MS);
        OCR1B = FRONT_MOTOR_90_DEG;
        _delay_ms(MOVE_DELAY_MS);

        // 2. Back Motor (PD5)
        OCR1A = BACK_MOTOR_0_DEG;
        _delay_ms(MOVE_DELAY_MS);
        OCR1A = BACK_MOTOR_90_DEG;
        _delay_ms(MOVE_DELAY_MS);

        // 3. Left Motor (PD7)
        OCR2 = LEFT_MOTOR_0_DEG;
        _delay_ms(MOVE_DELAY_MS);
        OCR2 = LEFT_MOTOR_90_DEG;
        _delay_ms(MOVE_DELAY_MS);

        // 4. Right Motor (PB3)
        OCR0 = RIGHT_MOTOR_0_DEG;
        _delay_ms(MOVE_DELAY_MS);
        OCR0 = RIGHT_MOTOR_90_DEG;
        _delay_ms(MOVE_DELAY_MS);
    }

    return 0; // Should not be reached
}
