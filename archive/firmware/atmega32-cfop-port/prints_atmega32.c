// prints_atmega32.c
// Rubik's Cube Solver - ATMega32 version
// This file provides printing functions for the cube state using ATMega32 UART.
// Adapted from Arduino's Serial.print/Serial.write to AVR-GCC UART routines.
//
// Changes from Arduino version:
// - Replaces Serial.print/Serial.write with uart_send_string and uart_send_char
// - Assumes UART is initialized elsewhere (see comments)
// - Uses standard C and AVR-GCC compatible code
// - Added comments for clarity

#include <stdint.h>
#include "a_constants_atmega32.c" // For Rubik, Rubik_Colors, etc.
#include <avr/io.h>

// UART send a single character (blocking)
void uart_send_char(char c) {
    while (!(UCSRA & (1 << UDRE)));
    UDR = c;
}

// UART send a null-terminated string (blocking)
void uart_send_string(const char *s) {
    while (*s) {
        uart_send_char(*s++);
    }
}

// Print a color as a single character (R, Y, B, G, W, O)
void Rubik_PrintColor(int color) {
    switch (color) {
        case RC_RED:    uart_send_char('R'); break;
        case RC_YELLOW: uart_send_char('Y'); break;
        case RC_BLUE:   uart_send_char('B'); break;
        case RC_GREEN:  uart_send_char('G'); break;
        case RC_WHITE:  uart_send_char('W'); break;
        case RC_ORANGE: uart_send_char('O'); break;
        default:        uart_send_char('?'); break;
    }
}

// Print three spaces (for formatting)
void Rubik_PrintSpace(void) {
    uart_send_string("   ");
}

// Print a line feed (new line)
void Rubik_PrintLineFeed(void) {
    uart_send_char('\n');
}

// Print the cube state (as in the original Cubo::Print)
void Rubik_Print(void) {
    int i;
    // Print UP face
    for (i = 6; i >= 0; i -= 3) {
        Rubik_PrintSpace(); Rubik_PrintSpace(); Rubik_PrintSpace();
        Rubik_PrintColor(Rubik[RF_UP][i + 0]); Rubik_PrintColor(Rubik[RF_UP][i + 1]); Rubik_PrintColor(Rubik[RF_UP][i + 2]);
        Rubik_PrintLineFeed();
    }
    Rubik_PrintLineFeed();
    // Print LEFT, FRONT, RIGHT, BACK faces
    for (i = 6; i >= 0; i -= 3) {
        Rubik_PrintColor(Rubik[RF_LEFT][i + 0]);  Rubik_PrintColor(Rubik[RF_LEFT][i + 1]);  Rubik_PrintColor(Rubik[RF_LEFT][i + 2]);  Rubik_PrintSpace();
        Rubik_PrintColor(Rubik[RF_FRONT][i + 0]); Rubik_PrintColor(Rubik[RF_FRONT][i + 1]); Rubik_PrintColor(Rubik[RF_FRONT][i + 2]); Rubik_PrintSpace();
        Rubik_PrintColor(Rubik[RF_RIGHT][i + 0]); Rubik_PrintColor(Rubik[RF_RIGHT][i + 1]); Rubik_PrintColor(Rubik[RF_RIGHT][i + 2]); Rubik_PrintSpace();
        Rubik_PrintColor(Rubik[RF_BACK][i + 0]);  Rubik_PrintColor(Rubik[RF_BACK][i + 1]);  Rubik_PrintColor(Rubik[RF_BACK][i + 2]);  Rubik_PrintSpace();
        Rubik_PrintLineFeed();
    }
    Rubik_PrintLineFeed();
    // Print DOWN face
    for (i = 6; i >= 0; i -= 3) {
        Rubik_PrintSpace(); Rubik_PrintSpace(); Rubik_PrintSpace();
        Rubik_PrintColor(Rubik[RF_DOWN][i + 0]); Rubik_PrintColor(Rubik[RF_DOWN][i + 1]); Rubik_PrintColor(Rubik[RF_DOWN][i + 2]);
        Rubik_PrintLineFeed();
    }
    Rubik_PrintLineFeed(); Rubik_PrintLineFeed();
}

/*
 * UART initialization should be done in your main setup code, e.g.:
 * void uart_init(void) {
 *     UCSRB = (1 << TXEN); // Enable transmitter
 *     UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0); // 8 data bits
 *     UBRRL = ...; // Set baud rate low byte
 *     UBRRH = ...; // Set baud rate high byte
 * }
 *
 * Call uart_init() before using Rubik_Print or any uart_send_* function.
 */

// End of prints_atmega32.c
