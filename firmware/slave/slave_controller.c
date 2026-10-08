/*
 * ============================================================================
 * SLAVE CONTROLLER (SLAVE_CONTROLLER.C) - V2
 * ============================================================================
 *
 * This is the SLAVE ATmega32. It is responsible for controlling the base motors.
 *
 * RESPONSIBILITIES:
 * 1. Monitors 4 input pins from the Master ATmega32.
 * 2. Controls the four BASE motors based on the digital signals received.
 *
 * HARDWARE:
 * - 4 input pins (PC0, PC1, PC2, PC3) connected to Master's output pins.
 * - Must share a common GND with the Master ATmega32.
 * - Base motors connected to hardware PWM pins (PB3, PD4, PD5, PD7).
 *
 * CONTROL PROTOCOL:
 * - HIGH signal = Motor moves to MAX position
 * - LOW signal = Motor moves to MIN position
 * - PC0 = Base Front Motor
 * - PC1 = Base Back Motor
 * - PC2 = Base Left Motor
 * - PC3 = Base Right Motor
 */

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

// --- INPUT PINS (from Master) ---
#define MASTER_FRONT_PIN  PC0
#define MASTER_BACK_PIN   PC1
#define MASTER_LEFT_PIN   PC2
#define MASTER_RIGHT_PIN  PC3

// ============================================================================
// !!! IMPORTANT: CALIBRATION REQUIRED !!!
// You must replace these placeholder values with the correct values for your
// four BASE motors. These values correspond to the MIN and MAX positions.
// ============================================================================


// Front Motor (PD4 - Timer1B)
#define BASE_FRONT_MIN   70 // Placeholder for 0 degrees
#define BASE_FRONT_MAX   230 // Placeholder for 60 degrees

// Back Motor (PD5 - Timer1A)
#define BASE_BACK_MIN    120 // Placeholder for 0 degrees
#define BASE_BACK_MAX    280 // Placeholder for 60 degrees

// Left Motor (PD7 - Timer2)
#define BASE_LEFT_MIN    10 // Placeholder for 0 degrees
#define BASE_LEFT_MAX    30 // Placeholder for 60 degrees

// Right Motor (PB3 - Timer0)
#define BASE_RIGHT_MIN   10 // Placeholder for 0 degrees
#define BASE_RIGHT_MAX   30 // Placeholder for 60 degrees


// --- Function Prototypes ---
void setup_all(void);

int main(void) {
    setup_all();

    // Initialize base motors to their minimum position
    OCR1B = BASE_FRONT_MIN; // Base Front
    OCR1A = BASE_BACK_MIN;  // Base Back
    OCR2  = BASE_LEFT_MIN;  // Base Left
    OCR0  = BASE_RIGHT_MIN; // Base Right

    while (1) {
        // Read the input pins from Master and control motors accordingly
        
        // Base Front Motor (controlled by PC0)
        if (PINC & (1 << MASTER_FRONT_PIN)) {
            OCR1B = BASE_FRONT_MAX; // HIGH = Max position
        } else {
            OCR1B = BASE_FRONT_MIN; // LOW = Min position
        }
        
        // Base Back Motor (controlled by PC1)
        if (PINC & (1 << MASTER_BACK_PIN)) {
            OCR1A = BASE_BACK_MAX;
        } else {
            OCR1A = BASE_BACK_MIN;
        }
        
        // Base Left Motor (controlled by PC2)
        if (PINC & (1 << MASTER_LEFT_PIN)) {
            OCR2 = BASE_LEFT_MAX;
        } else {
            OCR2 = BASE_LEFT_MIN;
        }
        
        // Base Right Motor (controlled by PC3)
        if (PINC & (1 << MASTER_RIGHT_PIN)) {
            OCR0 = BASE_RIGHT_MAX;
        } else {
            OCR0 = BASE_RIGHT_MIN;
        }
        
        // Small delay to avoid excessive polling
        _delay_ms(10);
    }
    return 0;
}

/**
 * @brief Initializes all hardware components for the Slave MCU.
 */
void setup_all(void) {
    // --- Pin Setup ---
    // Base motor pins as outputs
    DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD7);
    DDRB |= (1 << PB3);
    // Input pins from Master (PC0, PC1, PC2, PC3) - already inputs by default
    DDRC &= ~((1 << MASTER_FRONT_PIN) | (1 << MASTER_BACK_PIN) | (1 << MASTER_LEFT_PIN) | (1 << MASTER_RIGHT_PIN));

    // --- Timer Setup ---
    // Timer 1 (~61Hz) for PD4 (Base Front), PD5 (Base Back)
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    ICR1 = 2048;
    // Timer 2 (~61Hz) for PD7 (Base Left)
    TCCR2 = (1 << WGM21) | (1 << WGM20) | (1 << COM21) | (1 << CS22);
    // Timer 0 (~61Hz) for PB3 (Base Right)
    TCCR0 = (1 << WGM01) | (1 << WGM00) | (1 << COM01) | (1 << CS01) | (1 << CS00);
}