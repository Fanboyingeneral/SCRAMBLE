// Rubik_atmega32.c
// ATMega32 C port of Rubik.ino (main cube logic and move parser)
// All logic preserved, Arduino/Serial/String replaced with C/AVR-GCC routines
// All cube state and move buffers are statically allocated

#include <avr/pgmspace.h>
#include <string.h>
#include <ctype.h>
#include "a_constants_atmega32.c" // Rubik, color constants, etc.

// --- Cube state and move buffers ---
int Rubik[RF_TOTAL_FACES][9];
int Rubik_ToSolve[RF_TOTAL_FACES][9];
char resolutionStr[256] = {0}; // Solution buffer (increase if needed)
int HTM = 0, QTM = 0;

// --- Helper: Copy cube state ---
void CopyCube(int src[RF_TOTAL_FACES][9], int dst[RF_TOTAL_FACES][9]) {
    for (int f = 0; f < RF_TOTAL_FACES; f++)
        for (int i = 0; i < 9; i++)
            dst[f][i] = src[f][i];
}

// --- Cube initialization ---
void Rubik_Init(void) {
    for (int face = 0; face < RF_TOTAL_FACES; face++) {
        int color = Rubik_FaceToColorLUT[face];
        for (int i = 0; i < 9; i++)
            Rubik[face][i] = color;
    }
}

// --- Print cube state (stub, implement as needed) ---
void Rubik_Print(void) {
    // Implement UART/printf output as needed for debugging
}

// --- Move execution (virtual + physical) ---
void Rubik_Rotate(char rot, int motorsMove) {
    int MotorRotate = motorsMove;
    switch(rot) {
        case 'f': RotateFrontCCW(); if(MotorRotate) { ServosCube_Movex(); ServosFace_BackCCW(); ServosCube_MoveX(); } break;
        case 'F': RotateFrontCW();  if(MotorRotate) { ServosCube_Movex(); ServosFace_BackCW();  ServosCube_MoveX(); } break;
        case 'r': RotateRightCCW(); if(MotorRotate) { ServosFace_RightCCW(); } break;
        case 'R': RotateRightCW();  if(MotorRotate) { ServosFace_RightCW();  } break;
        case 'u': RotateUpCCW();    if(MotorRotate) { ServosFace_FrontCW();  } break;
        case 'U': RotateUpCW();     if(MotorRotate) { ServosFace_FrontCCW(); } break;
        case 'l': RotateLeftCCW();  if(MotorRotate) { ServosFace_LeftCW();   } break;
        case 'L': RotateLeftCW();   if(MotorRotate) { ServosFace_LeftCCW();  } break;
        case 'd': RotateDownCCW();  if(MotorRotate) { ServosFace_BackCCW();  } break;
        case 'D': RotateDownCW();   if(MotorRotate) { ServosFace_BackCW();   } break;
        case 'b': RotateBackCCW();  if(MotorRotate) { ServosCube_Movex(); ServosFace_FrontCW(); ServosCube_MoveX(); } break;
        case 'B': RotateBackCW();   if(MotorRotate) { ServosCube_Movex(); ServosFace_FrontCCW(); ServosCube_MoveX(); } break;
        case 'Y': RotateY();        if(MotorRotate) { ServosCube_MoveZ(); if(HTM>0 && QTM>0) { HTM--; QTM--; } } break;
    }
    // Optionally print state for debugging
    // if(MotorRotate) Rubik_Print();
}

// --- Execute a move string (RAM) ---
void rubik_RotateStr(const char* sequence) {
    size_t len = strlen(sequence);
    for (size_t i = 0; i < len; i++) {
        Rubik_Rotate(sequence[i], 0); // Virtual only
        size_t curr = strlen(resolutionStr);
        if (curr < sizeof(resolutionStr) - 2) {
            resolutionStr[curr] = sequence[i];
            resolutionStr[curr+1] = '\0';
        }
    }
}

// --- Execute a move string from PROGMEM (flash) ---
void rubik_RotateStr_Progmem(const char* progmem_str) {
    char buff[30]; // Temporary RAM buffer
    strcpy_P(buff, progmem_str);
    rubik_RotateStr(buff);
}

// --- Solution string optimizer (same logic as original) ---
void Rubik_ProcessStr(int caze) {
    int flag = 1;
    while(flag) {
        flag = 0;
        size_t len = strlen(resolutionStr);
        for (size_t i = 0; i < len; i++) {
            switch(caze) {
                case 0:
                    if ((resolutionStr[i] == 'r' && resolutionStr[i+1] == 'R') || (resolutionStr[i] == 'R' && resolutionStr[i+1] == 'r') ||
                        (resolutionStr[i] == 'l' && resolutionStr[i+1] == 'L') || (resolutionStr[i] == 'L' && resolutionStr[i+1] == 'l') ||
                        (resolutionStr[i] == 'f' && resolutionStr[i+1] == 'F') || (resolutionStr[i] == 'F' && resolutionStr[i+1] == 'f') ||
                        (resolutionStr[i] == 'b' && resolutionStr[i+1] == 'B') || (resolutionStr[i] == 'B' && resolutionStr[i+1] == 'b') ||
                        (resolutionStr[i] == 'u' && resolutionStr[i+1] == 'U') || (resolutionStr[i] == 'U' && resolutionStr[i+1] == 'u') ||
                        (resolutionStr[i] == 'd' && resolutionStr[i+1] == 'D') || (resolutionStr[i] == 'D' && resolutionStr[i+1] == 'd')) {
                        memmove(&resolutionStr[i], &resolutionStr[i+2], len-i-1);
                        flag = 1; break;
                    } else if ((resolutionStr[i] == resolutionStr[i+1]) && (resolutionStr[i] == resolutionStr[i+2])) {
                        resolutionStr[i] = isupper(resolutionStr[i]) ? tolower(resolutionStr[i]) : toupper(resolutionStr[i]);
                        memmove(&resolutionStr[i+1], &resolutionStr[i+3], len-i-2);
                        flag = 1; break;
                    } else if ((resolutionStr[i] == resolutionStr[i+1]) && (resolutionStr[i] == resolutionStr[i+2]) && (resolutionStr[i] == resolutionStr[i+3])) {
                        memmove(&resolutionStr[i], &resolutionStr[i+4], len-i-3);
                        flag = 1; break;
                    }
                    break;
                case 1:
                    if ((resolutionStr[i] == 'r' && resolutionStr[i+1] == 'r') || (resolutionStr[i] == 'R' && resolutionStr[i+1] == 'R') ||
                        (resolutionStr[i] == 'l' && resolutionStr[i+1] == 'l') || (resolutionStr[i] == 'L' && resolutionStr[i+1] == 'L') ||
                        (resolutionStr[i] == 'f' && resolutionStr[i+1] == 'f') || (resolutionStr[i] == 'F' && resolutionStr[i+1] == 'F') ||
                        (resolutionStr[i] == 'b' && resolutionStr[i+1] == 'b') || (resolutionStr[i] == 'B' && resolutionStr[i+1] == 'B') ||
                        (resolutionStr[i] == 'u' && resolutionStr[i+1] == 'u') || (resolutionStr[i] == 'U' && resolutionStr[i+1] == 'U') ||
                        (resolutionStr[i] == 'd' && resolutionStr[i+1] == 'd') || (resolutionStr[i] == 'D' && resolutionStr[i+1] == 'D')) {
                        resolutionStr[i+1] = '2';
                        flag = 1; break;
                    }
                    break;
                case 2:
                    return;
            }
        }
    }
}

// --- Main solver sequence (call all stages in order) ---
void Rubik_SolveSequence(void) {
    CopyCube(Rubik, Rubik_ToSolve);
    Rubik_Print();
    // delay(1000); // Omit or replace with _delay_ms if needed
    Solve_WhiteCross_1();
    Solve_WhiteCross_2();
    Solve_WhiteCross_3();
    Solve_WhiteCross_4();
    Solve_WhiteCross_5();
    Solve_WhiteCross_6();
    SolveF2L();
    Solve_OLL();
    Solve_PLL();
    Rubik_ProcessStr(0);
    CopyCube(Rubik_ToSolve, Rubik);
    size_t len = strlen(resolutionStr);
    for (size_t i = 0; i < len; i++) {
        Rubik_Rotate(resolutionStr[i], 1); // Physical move
    }
    Rubik_Print();
}

// End of Rubik_atmega32.c
