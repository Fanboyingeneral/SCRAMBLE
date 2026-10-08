/*
 * ATmega32 Rubik's Cube Solver BASE MOTOR Control via Bluetooth
 *
 * This program controls the four base motors of the Rubik's Cube solver.
 * It waits for two-character commands via USART (Bluetooth) to perform
 * specific motor rotations between a min (0 deg) and max (60 deg) position.
 *
 * MCU:   ATmega32
 * Clock: 1MHz Internal Oscillator
 *
 * COMMANDS:
 *   FF/FB: Front motor to max/min
 *   BF/BB: Back motor to max/min
 *   LF/LB: Left motor to max/min
 *   RF/RB: Right motor to max/min
 */

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

// --- USART Configuration ---
#define BAUD_RATE 9600
#define UBRR_VALUE ((F_CPU / (8UL * BAUD_RATE)) - 1)

// --- !! PLACEHOLDER CALIBRATION VALUES !! ---
// --- !! YOU MUST REPLACE THESE WITH YOUR MEASURED VALUES !! ---

// Front Motor (PD4 - Timer1B)
#define FRONT_MOTOR_MIN   70 // Placeholder for 0 degrees
#define FRONT_MOTOR_MAX   230 // Placeholder for 60 degrees

// Back Motor (PD5 - Timer1A)
#define BACK_MOTOR_MIN    120 // Placeholder for 0 degrees
#define BACK_MOTOR_MAX    280 // Placeholder for 60 degrees

// Left Motor (PD7 - Timer2)
#define LEFT_MOTOR_MIN    10 // Placeholder for 0 degrees
#define LEFT_MOTOR_MAX    30 // Placeholder for 60 degrees

// Right Motor (PB3 - Timer0)
#define RIGHT_MOTOR_MIN   10 // Placeholder for 0 degrees
#define RIGHT_MOTOR_MAX   30 // Placeholder for 60 degrees


// --- Delays ---
#define MOVE_DELAY_MS 1000 // A standard delay after a move completes

// --- Function Prototypes ---
void usart_init(void);
void usart_transmit_string(const char* str);
char usart_receive_char(void);
void setup_pins(void);
void setup_timers(void);

int main(void) {
    // Initialization
    setup_pins();
    usart_init();
    setup_timers();

    // --- Set Initial Position (MIN) ---
    // All motors start at the 0-degree (min) position.
    OCR1B = FRONT_MOTOR_MIN;
    OCR1A = BACK_MOTOR_MIN;
    OCR2  = LEFT_MOTOR_MIN;
    OCR0  = RIGHT_MOTOR_MIN;

    usart_transmit_string("\r\n--- Base Motors Ready ---\r\n");

    // --- Main Command Loop ---
    while (1) {
        char motor_char = usart_receive_char();
        char pos_char = usart_receive_char();

        switch (motor_char) {
            case 'F':
                if (pos_char == 'F') {
                    OCR1B = FRONT_MOTOR_MAX;
                    usart_transmit_string("Front -> MAX\r\n");
                } else if (pos_char == 'B') {
                    OCR1B = FRONT_MOTOR_MIN;
                    usart_transmit_string("Front -> MIN\r\n");
                }
                break;
            case 'B':
                if (pos_char == 'F') {
                    OCR1A = BACK_MOTOR_MAX;
                    usart_transmit_string("Back -> MAX\r\n");
                } else if (pos_char == 'B') {
                    OCR1A = BACK_MOTOR_MIN;
                    usart_transmit_string("Back -> MIN\r\n");
                }
                break;
            case 'L':
                if (pos_char == 'F') {
                    OCR2 = LEFT_MOTOR_MAX;
                    usart_transmit_string("Left -> MAX\r\n");
                } else if (pos_char == 'B') {
                    OCR2 = LEFT_MOTOR_MIN;
                    usart_transmit_string("Left -> MIN\r\n");
                }
                break;
            case 'R':
                if (pos_char == 'F') {
                    OCR0 = RIGHT_MOTOR_MAX;
                    usart_transmit_string("Right -> MAX\r\n");
                } else if (pos_char == 'B') {
                    OCR0 = RIGHT_MOTOR_MIN;
                    usart_transmit_string("Right -> MIN\r\n");
                }
                break;
        }
        _delay_ms(MOVE_DELAY_MS); // Let servo settle
    }
    return 0;
}

/**
 * @brief Configures the DDR registers for the motor and USART pins.
 */
void setup_pins(void) {
    DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD7); // Motor pins
    DDRB |= (1 << PB3);                          // Motor pin
    DDRD |= (1 << PD1);                          // USART TX pin
}

/**
 * @brief Configures Timers for consistent ~61Hz PWM.
 */
void setup_timers(void) {
    // Timer 1 (~61Hz)
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    ICR1 = 2048;

    // Timer 2 (~61Hz)
    TCCR2 = (1 << WGM21) | (1 << WGM20) | (1 << COM21) | (1 << CS22);

    // Timer 0 (~61Hz)
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
 * @brief Transmits a null-terminated string.
 */
void usart_transmit_string(const char* str) {
    while (*str) {
        char c = *str++;
        while (!(UCSRA & (1 << UDRE)));
        UDR = c;
    }
}

/**
 * @brief Receives a single character.
 */
char usart_receive_char(void) {
    while (!(UCSRA & (1 << RXC)));
    return UDR;
}
