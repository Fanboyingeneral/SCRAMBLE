// rubik_SolveF2L_atmega32.c
// ATMega32 C port of rubik_SolveF2L.ino
// All Arduino/PROGMEM/Serial code replaced with standard C and project abstractions

#include "a_constants_atmega32.c" // Rubik, Rubik_Colors, etc.
#include "prints_atmega32.c"      // UART print routines (for debug)
#include <avr/pgmspace.h>

// F2L move sequences as flash-resident PROGMEM strings
const char F2L_01[] PROGMEM = "";
const char F2L_02[] PROGMEM = "RRuuFrrfUUrUr";
const char F2L_03[] PROGMEM = "urururURUR";
const char F2L_04[] PROGMEM = "URurufUF";
const char F2L_05[] PROGMEM = "rururURUR";
const char F2L_06[] PROGMEM = "UURurufUF";
const char F2L_07[] PROGMEM = "UrururURUR";
const char F2L_08[] PROGMEM = "uuURurufUF";
const char F2L_09[] PROGMEM = "UUrururURUR";
const char F2L_10[] PROGMEM = "uURurufUF";
const char F2L_11[] PROGMEM = "RUruRUUruRUr";
const char F2L_12[] PROGMEM = "RFURurfur";
const char F2L_13[] PROGMEM = "uRurURur";
const char F2L_14[] PROGMEM = "rFRfrFRf";
const char F2L_15[] PROGMEM = "RurURur";
const char F2L_16[] PROGMEM = "UrFRfrFRf";
const char F2L_17[] PROGMEM = "URurURur";
const char F2L_18[] PROGMEM = "UUrFRfrFRf";
const char F2L_19[] PROGMEM = "uuRurURur";
const char F2L_20[] PROGMEM = "urFRfrFRf";
const char F2L_21[] PROGMEM = "RurURuurURur";
const char F2L_22[] PROGMEM = "RUFRUrufr";
const char F2L_23[] PROGMEM = "uRUruRUr";
const char F2L_24[] PROGMEM = "RUruufUF";
const char F2L_25[] PROGMEM = "RUruRUr";
const char F2L_26[] PROGMEM = "URUruufUF";
const char F2L_27[] PROGMEM = "RUruRUr";
const char F2L_28[] PROGMEM = "uuRUruufUF";
const char F2L_29[] PROGMEM = "uuRUruRUr";
const char F2L_30[] PROGMEM = "uRUruufUF";
const char F2L_31[] PROGMEM = "uRurUURur";
const char F2L_32[] PROGMEM = "uRUrUfuF";
const char F2L_33[] PROGMEM = "RBLulbr";
const char F2L_34[] PROGMEM = "RurUrFRfRur";
const char F2L_35[] PROGMEM = "URur";
const char F2L_36[] PROGMEM = "uRuurUfuF";
const char F2L_37[] PROGMEM = "uRUruuRur";
const char F2L_38[] PROGMEM = "uRurUfuF";
const char F2L_39[] PROGMEM = "uRUUruRuur";
const char F2L_40[] PROGMEM = "fuF";
const char F2L_41[] PROGMEM = "RUruRUruRUr";
const char F2L_42[] PROGMEM = "urFRfRur";
const char F2L_43[] PROGMEM = "URuruRurURur";
const char F2L_44[] PROGMEM = "rFRfRurURur";
const char F2L_45[] PROGMEM = "RuuruRUr";
const char F2L_46[] PROGMEM = "FURurfRur";
const char F2L_47[] PROGMEM = "URuurURur";
const char F2L_48[] PROGMEM = "fluuLF";
const char F2L_49[] PROGMEM = "RuruuRUr";
const char F2L_50[] PROGMEM = "ufUUffrfR";
const char F2L_51[] PROGMEM = "URUruuRUr";
const char F2L_52[] PROGMEM = "UfuFuRUr";
const char F2L_53[] PROGMEM = "UfuuFuRUr";
const char F2L_54[] PROGMEM = "ufUF";
const char F2L_55[] PROGMEM = "uRurURUr";
const char F2L_56[] PROGMEM = "RurUUfuF";
const char F2L_57[] PROGMEM = "RUr";
const char F2L_58[] PROGMEM = "UfuuFuFrfR";
const char F2L_59[] PROGMEM = "uRUrURUr";
const char F2L_60[] PROGMEM = "rrBUbuRR";

const char* const F2L_LUT[60] PROGMEM = {
    F2L_01, F2L_02, F2L_03, F2L_04, F2L_05, F2L_06, F2L_07, F2L_08, F2L_09, F2L_10,
    F2L_11, F2L_12, F2L_13, F2L_14, F2L_15, F2L_16, F2L_17, F2L_18, F2L_19, F2L_20,
    F2L_21, F2L_22, F2L_23, F2L_24, F2L_25, F2L_26, F2L_27, F2L_28, F2L_29, F2L_30,
    F2L_31, F2L_32, F2L_33, F2L_34, F2L_35, F2L_36, F2L_37, F2L_38, F2L_39, F2L_40,
    F2L_41, F2L_42, F2L_43, F2L_44, F2L_45, F2L_46, F2L_47, F2L_48, F2L_49, F2L_50,
    F2L_51, F2L_52, F2L_53, F2L_54, F2L_55, F2L_56, F2L_57, F2L_58, F2L_59, F2L_60
};

// Helper to copy a PROGMEM string to RAM
void f2l_get_move(uint8_t idx, char* dest, uint8_t maxlen) {
    if (idx >= 60) { dest[0] = '\0'; return; }
    const char* ptr = (const char*)pgm_read_word(&(F2L_LUT[idx]));
    uint8_t i = 0;
    char c;
    while (i < (maxlen-1) && (c = pgm_read_byte(ptr++))) {
        dest[i++] = c;
    }
    dest[i] = '\0';
}

// Helper to copy a PROGMEM string to RAM and call rubik_RotateStr directly, avoiding large local arrays
void f2l_call_move(uint8_t idx) {
    if (idx >= 60) return;
    const char* ptr = (const char*)pgm_read_word(&(F2L_LUT[idx]));
    char c;
    char move[8]; // Small buffer for chunked copy
    uint8_t i = 0;
    while ((c = pgm_read_byte(ptr++))) {
        move[i++] = c;
        if (i == sizeof(move) - 1) {
            move[i] = '\0';
            rubik_RotateStr(move);
            i = 0;
        }
    }
    if (i > 0) {
        move[i] = '\0';
        rubik_RotateStr(move);
    }
}

// F2L solver logic
void F2L_CornerException(char Front_color, char Right_color);
void F2L_EdgeException(char Front_color, char Right_color);

void SolveF2LSlot(int Front_color, int Right_color) {
    char c = -1;
    char e = -1;
    F2L_CornerException(Front_color, Right_color);
    if ((Rubik[RF_DOWN][8] == RC_WHITE) && (Rubik[RF_FRONT][2] == Front_color) && (Rubik[RF_RIGHT][0] == Right_color)) {
        c = 0;
    } else if ((Rubik[RF_DOWN][8] == Right_color) && (Rubik[RF_FRONT][2] == RC_WHITE) && (Rubik[RF_RIGHT][0] == Front_color)) {
        c = 1;
    } else if ((Rubik[RF_DOWN][8] == Front_color) && (Rubik[RF_FRONT][2] == Right_color) && (Rubik[RF_RIGHT][0] == RC_WHITE)) {
        c = 2;
    } else if ((Rubik[RF_UP][2] == Front_color) && (Rubik[RF_FRONT][8] == RC_WHITE) && (Rubik[RF_RIGHT][6] == Right_color)) {
        c = 3;
    } else if ((Rubik[RF_UP][2] == RC_WHITE) && (Rubik[RF_FRONT][8] == Right_color) && (Rubik[RF_RIGHT][6] == Front_color)) {
        c = 4;
    } else if ((Rubik[RF_UP][2] == Right_color) && (Rubik[RF_FRONT][8] == Front_color) && (Rubik[RF_RIGHT][6] == RC_WHITE)) {
        c = 5;
    }
    F2L_EdgeException(Front_color, Right_color);
    if ((Rubik[RF_FRONT][5] == Front_color) && (Rubik[RF_RIGHT][3] == Right_color)) {
        e = 0;
    } else if ((Rubik[RF_FRONT][5] == Right_color) && (Rubik[RF_RIGHT][3] == Front_color)) {
        e = 1;
    } else if ((Rubik[RF_UP][1] == Front_color) && (Rubik[RF_FRONT][7] == Right_color)) {
        e = 2;
    } else if ((Rubik[RF_UP][1] == Right_color) && (Rubik[RF_FRONT][7] == Front_color)) {
        e = 3;
    } else if ((Rubik[RF_UP][5] == Front_color) && (Rubik[RF_RIGHT][7] == Right_color)) {
        e = 4;
    } else if ((Rubik[RF_UP][5] == Right_color) && (Rubik[RF_RIGHT][7] == Front_color)) {
        e = 5;
    } else if ((Rubik[RF_UP][7] == Front_color) && (Rubik[RF_BACK][7] == Right_color)) {
        e = 6;
    } else if ((Rubik[RF_UP][7] == Right_color) && (Rubik[RF_BACK][7] == Front_color)) {
        e = 7;
    } else if ((Rubik[RF_UP][3] == Front_color) && (Rubik[RF_LEFT][7] == Right_color)) {
        e = 8;
    } else if ((Rubik[RF_UP][3] == Right_color) && (Rubik[RF_LEFT][7] == Front_color)) {
        e = 9;
    }
    int i = c * 10 + e;
    if (i >= 0 && i < 60) {
        f2l_call_move(i);
    }
}

void F2L_CornerException(char Front_color, char Right_color) {
    if ((Rubik[RF_UP][8] == RC_WHITE && Rubik[RF_RIGHT][8] == Right_color && Rubik[RF_BACK][6] == Front_color) ||
        (Rubik[RF_UP][8] == Right_color && Rubik[RF_RIGHT][8] == Front_color && Rubik[RF_BACK][6] == RC_WHITE) ||
        (Rubik[RF_UP][8] == Front_color && Rubik[RF_RIGHT][8] == RC_WHITE && Rubik[RF_BACK][6] == Right_color)) {
        rubik_RotateStr("U");
    } else if ((Rubik[RF_UP][8] == RC_WHITE && Rubik[RF_BACK][8] == Right_color && Rubik[RF_LEFT][6] == Front_color) ||
               (Rubik[RF_UP][8] == Right_color && Rubik[RF_BACK][8] == Front_color && Rubik[RF_LEFT][6] == RC_WHITE) ||
               (Rubik[RF_UP][8] == Front_color && Rubik[RF_BACK][8] == RC_WHITE && Rubik[RF_LEFT][6] == Right_color)) {
        rubik_RotateStr("UU");
    } else if ((Rubik[RF_UP][0] == RC_WHITE && Rubik[RF_FRONT][6] == Front_color && Rubik[RF_LEFT][8] == Right_color) ||
               (Rubik[RF_UP][0] == Front_color && Rubik[RF_FRONT][6] == Right_color && Rubik[RF_LEFT][8] == RC_WHITE) ||
               (Rubik[RF_UP][0] == Right_color && Rubik[RF_FRONT][6] == RC_WHITE && Rubik[RF_LEFT][8] == Front_color)) {
        rubik_RotateStr("u");
    } else if ((Rubik[RF_DOWN][2] == RC_WHITE && Rubik[RF_RIGHT][2] == Front_color && Rubik[RF_BACK][0] == Right_color) ||
               (Rubik[RF_DOWN][2] == Front_color && Rubik[RF_RIGHT][2] == Right_color && Rubik[RF_BACK][0] == RC_WHITE) ||
               (Rubik[RF_DOWN][2] == Right_color && Rubik[RF_RIGHT][2] == RC_WHITE && Rubik[RF_BACK][0] == Front_color)) {
        rubik_RotateStr("rUURu");
    } else if ((Rubik[RF_DOWN][0] == RC_WHITE && Rubik[RF_BACK][2] == Front_color && Rubik[RF_LEFT][0] == Right_color) ||
               (Rubik[RF_DOWN][0] == Front_color && Rubik[RF_BACK][2] == Right_color && Rubik[RF_LEFT][0] == RC_WHITE) ||
               (Rubik[RF_DOWN][0] == Right_color && Rubik[RF_BACK][2] == RC_WHITE && Rubik[RF_LEFT][0] == Front_color)) {
        rubik_RotateStr("LUUl");
    } else if ((Rubik[RF_DOWN][6] == RC_WHITE && Rubik[RF_LEFT][2] == Front_color && Rubik[RF_FRONT][0] == Right_color) ||
               (Rubik[RF_DOWN][6] == Front_color && Rubik[RF_LEFT][2] == Right_color && Rubik[RF_FRONT][0] == RC_WHITE) ||
               (Rubik[RF_DOWN][6] == Right_color && Rubik[RF_LEFT][2] == RC_WHITE && Rubik[RF_FRONT][0] == Front_color)) {
        rubik_RotateStr("luL");
    }
}

void F2L_EdgeException(char Front_color, char Right_color) {
    if ((Rubik[RF_RIGHT][5] == Front_color && Rubik[RF_BACK][3] == Right_color) ||
        (Rubik[RF_RIGHT][5] == Right_color && Rubik[RF_BACK][3] == Front_color)) {
        rubik_RotateStr("ruR");
    } else if ((Rubik[RF_BACK][5] == Front_color && Rubik[RF_LEFT][3] == Right_color) ||
               (Rubik[RF_BACK][5] == Right_color && Rubik[RF_LEFT][3] == Front_color)) {
        rubik_RotateStr("LulU");
    } else if ((Rubik[RF_LEFT][5] == Front_color && Rubik[RF_FRONT][3] == Right_color) ||
               (Rubik[RF_LEFT][5] == Right_color && Rubik[RF_FRONT][3] == Front_color)) {
        rubik_RotateStr("luLU");
    }
}

void SolveF2L(void) {
    SolveF2LSlot(RC_RED, RC_GREEN);
    rubik_RotateStr("Y");
    SolveF2LSlot(RC_BLUE, RC_RED);
    rubik_RotateStr("Y");
    SolveF2LSlot(RC_ORANGE, RC_BLUE);
    rubik_RotateStr("Y");
    SolveF2LSlot(RC_GREEN, RC_ORANGE);
    rubik_RotateStr("Y");
}

// End of rubik_SolveF2L_atmega32.c
