// guardar_cores_atmega32.c
// Rubik's Cube Solver - ATMega32 version
// This file defines the scanning sequence for the cube using ATMega32-compatible code.
// Adapted from Arduino code to AVR-GCC (no Arduino libraries).
//
// Changes from Arduino version:
// - Replaces delay() with _delay_ms() from <util/delay.h>
// - Assumes servo and getBit functions are already ATMega32-compatible
// - Added comments for clarity

#include <stdint.h>
#include <util/delay.h>
#include "a_constants_atmega32.c" // For Rubik, Rubik_Colors, etc.

#define DELAY_BETWEEN_STEPS1 (500) // ms
#define APERTO (4) // "Tightness" for gripping

// External functions for servo and cube control (must be implemented elsewhere)
extern void getBit(int currentFace);
extern void ServosCube_MoveZ(void);
extern void ServosCube_Movez(void);
extern struct Servo Down_Right, Down_Left, Down_Front, Down_Back, Up_Back, Up_Front, Up_Right, Up_Left;

// Helper function to move servos for a clear view
static void clearVision() {
    // 1. Retract the Right and Left pushers
    Down_Right.rmove(place[3]);
    Down_Left.rmove(place[3]);
    _delay_ms(DELAY_BETWEEN_STEPS1);
    // 2. Grip the cube tightly with the Front and Back pushers
    Down_Front.rmove(place[1] + APERTO);
    Down_Back.rmove(place[1] + APERTO);
    _delay_ms(DELAY_BETWEEN_STEPS1);
    // 3. Use the top rotators to tilt the cube
    Up_Back.Rotate(0);
    Up_Front.Rotate(180);
}

// Main scanning sequence for all 6 faces
void sequencia_de_armazenamento() {
    // --- Scan Right and Left Faces ---
    clearVision();
    getBit(RF_RIGHT);
    Up_Back.Rotate(180);
    Up_Front.Rotate(0);
    getBit(RF_LEFT);

    // --- Reorient Cube to Scan Up and Down Faces ---
    Up_Front.Rotate(90);
    Up_Back.Rotate(90);
    Down_Front.rmove(place[1]);
    Down_Back.rmove(place[1]);
    _delay_ms(DELAY_BETWEEN_STEPS1);
    Down_Right.rmove(place[1] + APERTO);
    Down_Left.rmove(place[1] + APERTO);
    _delay_ms(DELAY_BETWEEN_STEPS1);
    Down_Front.rmove(place[3]);
    Down_Back.rmove(place[3]);
    _delay_ms(DELAY_BETWEEN_STEPS1);
    Up_Right.Rotate(0);
    Up_Left.Rotate(180);
    getBit(RF_UP);
    Up_Right.Rotate(180);
    Up_Left.Rotate(0);
    getBit(RF_DOWN);

    // --- Reorient Cube to Scan Front and Back Faces ---
    Up_Right.Rotate(90);
    Up_Left.Rotate(90);
    Down_Right.rmove(place[1]);
    Down_Left.rmove(place[1]);
    _delay_ms(DELAY_BETWEEN_STEPS1);
    Down_Back.rmove(place[1]);
    Down_Front.rmove(place[1]);
    _delay_ms(DELAY_BETWEEN_STEPS1);
    ServosCube_MoveZ();

    // --- Scan Front and Back Faces ---
    clearVision();
    getBit(RF_FRONT);
    Up_Back.Rotate(180);
    Up_Front.Rotate(0);
    getBit(RF_BACK);

    // --- Final Reset ---
    Up_Back.Rotate(90);
    Up_Front.Rotate(90);
    Down_Front.rmove(place[1]);
    Down_Back.rmove(place[1]);
    _delay_ms(DELAY_BETWEEN_STEPS1);
    Down_Left.rmove(place[1]);
    Down_Right.rmove(place[1]);
    _delay_ms(DELAY_BETWEEN_STEPS1);
    ServosCube_Movez();
    _delay_ms(DELAY_BETWEEN_STEPS1);
}

// End of guardar_cores_atmega32.c
