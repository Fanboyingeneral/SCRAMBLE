// a_constants_atmega32.c
// Rubik's Cube Solver - ATMega32 version
// This file defines enums and data structures for the cube state.
// It is adapted from the Arduino version for use with AVR-GCC and ATMega32.
// No Arduino-specific code is present here.
//
// Changes from Arduino version:
// - Uses stdint.h for fixed-width types (uint8_t instead of unsigned char)
// - All code is standard C, portable to AVR microcontrollers
// - Added comments for clarity

#include <stdint.h> // For uint8_t

// Enumeration for the faces of the cube
// Values: 0=FRONT, 1=UP, 2=LEFT, 3=RIGHT, 4=DOWN, 5=BACK, 6=TOTAL_FACES
typedef enum {
    RF_FRONT,   // 0 - The face initially facing the user
    RF_UP,      // 1 - The top face
    RF_LEFT,    // 2 - The left face
    RF_RIGHT,   // 3 - The right face
    RF_DOWN,    // 4 - The bottom face
    RF_BACK,    // 5 - The back face
    RF_TOTAL_FACES // 6 - Total number of faces
} Rubik_Faces;

// Enumeration for the colors of the stickers
// Values: 0=RED, 1=YELLOW, 2=BLUE, 3=GREEN, 4=WHITE, 5=ORANGE, 6=TOTAL_COLORS
typedef enum {
    RC_RED,
    RC_YELLOW,
    RC_BLUE,
    RC_GREEN,
    RC_WHITE,
    RC_ORANGE,
    RC_TOTAL_COLORS
} Rubik_Colors;

// Look-Up Table (LUT) mapping face to its center color on a solved cube
// Example: Rubik_FaceToColorLUT[RF_FRONT] == RC_RED
const int Rubik_FaceToColorLUT[RF_TOTAL_FACES] = {
    RC_RED,    // FRONT
    RC_YELLOW, // UP
    RC_BLUE,   // LEFT
    RC_GREEN,  // RIGHT
    RC_WHITE,  // DOWN
    RC_ORANGE  // BACK
};

// Main data structure for the virtual cube
// Rubik[face][sticker] stores the color of that sticker (0-5)
uint8_t Rubik[RF_TOTAL_FACES][9];

// End of a_constants_atmega32.c
