// bluethoot_receiver_atmega32.c
// Rubik's Cube Solver - ATMega32 version
// This file handles Bluetooth input and LED control using ATMega32 peripherals.
// Adapted from Arduino code to AVR-GCC (no Arduino libraries).
//
// Changes from Arduino version:
// - Replaces Serial.read/Serial.available/Serial.print with UART routines
// - Replaces digitalWrite with direct port manipulation
// - Replaces delay() with _delay_ms() from <util/delay.h>
// - Added comments for clarity

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "a_constants_atmega32.c" // For Rubik, Rubik_Colors, etc.

// Pin definitions (change as needed for your hardware wiring)
#define S2         PD2
#define S3         PD3
#define COLOR_IN1  PD4
#define COLOR_IN2  PD5
#define LED1       PD6
#define LED2       PD7

// UART receive a character (blocking)
char uart_recv_char(void) {
    while (!(UCSRA & (1 << RXC)));
    return UDR;
}

// UART check if data is available (non-blocking)
uint8_t uart_data_available(void) {
    return (UCSRA & (1 << RXC));
}

// UART send a single character (already defined in prints_atmega32.c)
extern void uart_send_char(char c);
// UART send a string (already defined in prints_atmega32.c)
extern void uart_send_string(const char *s);

unsigned long redFrequency = 0;
unsigned long greenFrequency = 0;
unsigned long blueFrequency = 0;

void getBit(int currentFace) {
    int sticker = 0;
    while(sticker < 9) {
        if(uart_data_available()) {
            char Incoming_value = uart_recv_char();
            uart_send_char(Incoming_value); // Echo
            uart_send_char('\n');
            int color = 0;
            switch(Incoming_value) {
                case 'G': color = RC_GREEN; break;
                case 'R': color = RC_RED; break;
                case 'B': color = RC_BLUE; break;
                case 'Y': color = RC_YELLOW; break;
                case 'W': color = RC_WHITE; break;
                case 'O': color = RC_ORANGE; break;
                default:
                    uart_send_string("Erro + '");
                    uart_send_char(Incoming_value);
                    uart_send_string("'\n");
            }
            Rubik[currentFace][sticker++] = color;
        }
    }
}

void lightAnimation() {
    // Set LED1 and LED2 as output
    DDRD |= (1 << LED1) | (1 << LED2);
    for(int i = 0; i < 8; i++) {
        PORTD &= ~((1 << LED1) | (1 << LED2)); // LEDs LOW
        _delay_ms(200);
        PORTD |= (1 << LED1) | (1 << LED2);   // LEDs HIGH
        _delay_ms(200);
    }
}

// End of bluethoot_receiver_atmega32.c
