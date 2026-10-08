/*
 * ATmega32 Rubik's Cube Solver Motor Control via Bluetooth
 *
 * This program controls four SG90 servo motors connected to the hardware
 * PWM pins of an ATmega32. It initializes all motors to a 90-degree
 * position, then waits for single-character commands via USART (Bluetooth)
 * to perform specific motor rotations.
 *
 * MCU:   ATmega32
 * Clock: 1MHz Internal Oscillator
 * Pins:
 *  - Front Motor: PD4 (OC1B)
 *  - Back Motor:  PD5 (OC1A)
 *  - Left Motor:  PD7 (OC2)
 *  - Right Motor: PB3 (OC0)
 *  - USART:       PD0 (RX), PD1 (TX)
 */

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h> // Required for itoa()

// --- USART Configuration ---
#define BAUD_RATE 9600
#define UBRR_VALUE ((F_CPU / (8UL * BAUD_RATE)) - 1)

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
#define RIGHT_MOTOR_0_DEG   7
#define RIGHT_MOTOR_90_DEG  23

// --- Delays ---
#define MOVE_DELAY_MS 500 // A standard delay after a move completes

// --- Function Prototypes ---
void usart_init(void);
void usart_transmit_string(const char* str);
char usart_receive_char(void);
void setup_pins(void);
void setup_timers(void);

int main(void) {
    setup_pins();
    setup_timers();
    usart_init();

    // --- Initial Position (90 Degrees) ---
    OCR1B = FRONT_MOTOR_90_DEG; // Front
    OCR1A = BACK_MOTOR_90_DEG;  // Back
    OCR2  = LEFT_MOTOR_90_DEG;  // Left
    OCR0  = RIGHT_MOTOR_90_DEG; // Right

    usart_transmit_string("\r\n--- Rubik's Cube Solver Ready ---\r\n");
    usart_transmit_string("Commands: F/f, B/b, L/l, R/r\r\n");

    // --- Main Command Loop ---
    while (1) {
        char command = usart_receive_char();

        switch (command) {
            // --- Front Motor ---
            case 'F':
                OCR1B = FRONT_MOTOR_0_DEG;
                usart_transmit_string("Front: ACW\r\n");
                break;
            case 'f':
                OCR1B = FRONT_MOTOR_90_DEG;
                usart_transmit_string("Front: CW\r\n");
                break;

            // --- Back Motor ---
            case 'B':
                OCR1A = BACK_MOTOR_0_DEG;
                usart_transmit_string("Back: ACW\r\n");
                break;
            case 'b':
                OCR1A = BACK_MOTOR_90_DEG;
                usart_transmit_string("Back: CW\r\n");
                break;

            // --- Left Motor ---
            case 'L':
                OCR2 = LEFT_MOTOR_0_DEG;
                usart_transmit_string("Left: ACW\r\n");
                break;
            case 'l':
                OCR2 = LEFT_MOTOR_90_DEG;
                usart_transmit_string("Left: CW\r\n");
                break;

            // --- Right Motor ---
            case 'R':
                OCR0 = RIGHT_MOTOR_0_DEG;
                usart_transmit_string("Right: ACW\r\n");
                break;
            case 'r':
                OCR0 = RIGHT_MOTOR_90_DEG;
                usart_transmit_string("Right: CW\r\n");
                break;
            
            default:
                usart_transmit_string("Unknown command\r\n");
                break;
        }
        _delay_ms(MOVE_DELAY_MS); // Small delay to let servo settle
    }

    return 0; // Should not be reached
}

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
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    ICR1 = 2048;

    // --- Timer 2 (8-bit) for Left Motor (OC2) ---
    TCCR2 = (1 << WGM21) | (1 << WGM20) | (1 << COM21) | (1 << CS22);

    // --- Timer 0 (8-bit) for Right Motor (OC0) ---
    TCCR0 = (1 << WGM01) | (1 << WGM00) | (1 << COM01) | (1 << CS01) | (1 << CS00);
}

/**
 * @brief Initializes the USART for serial communication.
 */
void usart_init(void) {
    UBRRH = (uint8_t)(UBRR_VALUE >> 8);
    UBRRL = (uint8_t)UBRR_VALUE;
    UCSRA = (1 << U2X);
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

/**
 * @brief Transmits a single character.
 */
void usart_transmit_char(char data) {
    while (!(UCSRA & (1 << UDRE)));
    UDR = data;
}

/**
 * @brief Transmits a null-terminated string.
 */
void usart_transmit_string(const char* str) {
    while (*str) {
        usart_transmit_char(*str++);
    }
}

/**
 * @brief Receives a single character.
 */
char usart_receive_char(void) {
    while (!(UCSRA & (1 << RXC)));
    return UDR;
}