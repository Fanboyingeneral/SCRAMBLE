// rubik_SolveCross_atmega32.c
// ATMega32 C port of rubik_SolveCross.ino
// All Arduino/Serial code replaced with standard C and project abstractions

#include "a_constants_atmega32.c" // Rubik, Rubik_Colors, etc.
#include "prints_atmega32.c"      // UART print routines (for debug)

// NOTE: All calls to rubik.RotateStr() are assumed to be implemented in your ATMega32 Rubik abstraction.
// If not, you must provide a C function: void rubik_RotateStr(const char* moves);
// Replace rubik.RotateStr(...) with rubik_RotateStr(...)

void Solve_WhiteCross_1(void) {
    // Find edges with white on the bottom face
    if (Rubik[RF_DOWN][1] == RC_WHITE) {
        switch (Rubik[RF_BACK][1]) {
            case RC_RED:    rubik_RotateStr("dd"); break;
            case RC_BLUE:   rubik_RotateStr("D");  break;
            case RC_ORANGE: rubik_RotateStr("");   break;
            case RC_GREEN:  rubik_RotateStr("d");  break;
        }
    }
    if (Rubik[RF_DOWN][3] == RC_WHITE) {
        switch (Rubik[RF_LEFT][1]) {
            case RC_RED:    rubik_RotateStr("D");  break;
            case RC_BLUE:   rubik_RotateStr("");   break;
            case RC_ORANGE: rubik_RotateStr("d");  break;
            case RC_GREEN:  rubik_RotateStr("dd"); break;
        }
    }
    if (Rubik[RF_RIGHT][5] == RC_WHITE) {
        switch (Rubik[RF_DOWN][1]) {
            case RC_RED:    rubik_RotateStr("d");  break;
            case RC_BLUE:   rubik_RotateStr("dd"); break;
            case RC_ORANGE: rubik_RotateStr("D");  break;
            case RC_GREEN:  rubik_RotateStr("");   break;
        }
    }
    if (Rubik[RF_FRONT][7] == RC_WHITE) {
        switch (Rubik[RF_DOWN][1]) {
            case RC_RED:    rubik_RotateStr("");   break;
            case RC_BLUE:   rubik_RotateStr("d");  break;
            case RC_ORANGE: rubik_RotateStr("dd"); break;
            case RC_GREEN:  rubik_RotateStr("D");  break;
        }
    }
}

void Solve_WhiteCross_2(void) {
    while (1) {
        int side_color;
        int rot = 0;
        // Find edges with white on the top face
        if (Rubik[RF_UP][1] == RC_WHITE) {
            side_color = Rubik[RF_FRONT][7];
            rot -= 0;
        } else if (Rubik[RF_UP][3] == RC_WHITE) {
            side_color = Rubik[RF_LEFT][7];
            rot -= 3;
        } else if (Rubik[RF_UP][5] == RC_WHITE) {
            side_color = Rubik[RF_RIGHT][7];
            rot -= 1;
        } else if (Rubik[RF_UP][7] == RC_WHITE) {
            side_color = Rubik[RF_BACK][7];
            rot -= 2;
        } else {
            break;
        }
        switch (side_color) {
            case RC_RED:    rot += 0; break;
            case RC_GREEN:  rot += 1; break;
            case RC_ORANGE: rot += 2; break;
            case RC_BLUE:   rot += 3; break;
        }
        if (rot == 0) {
            rubik_RotateStr("");
        } else if (rot == -3 || rot == 1) {
            rubik_RotateStr("u");
        } else if (rot == 2 || rot == -2) {
            rubik_RotateStr("UU");
        } else if (rot == -1 || rot == 3) {
            rubik_RotateStr("U");
        }
        switch (side_color) {
            case RC_RED:    rubik_RotateStr("FF"); break;
            case RC_GREEN:  rubik_RotateStr("RR"); break;
            case RC_ORANGE: rubik_RotateStr("BB"); break;
            case RC_BLUE:   rubik_RotateStr("LL"); break;
        }
        // Loop again if there are still white edges on top
    }
}

void Solve_WhiteCross_3(void) {
    while (1) {
        int found = 0;
        // Find edges with white on the sides of the top layer
        if (Rubik[RF_FRONT][7] == RC_WHITE) {
            found = 1;
            switch (Rubik[RF_UP][1]) {
                case RC_RED:    rubik_RotateStr("urFR"); break;
                case RC_BLUE:   rubik_RotateStr("fLF"); break;
                case RC_ORANGE: rubik_RotateStr("uRbr"); break;
                case RC_GREEN:  rubik_RotateStr("Frf"); break;
            }
        } else if (Rubik[RF_LEFT][7] == RC_WHITE) {
            found = 1;
            switch (Rubik[RF_UP][3]) {
                case RC_RED:    rubik_RotateStr("Lfl"); break;
                case RC_BLUE:   rubik_RotateStr("ufLF"); break;
                case RC_ORANGE: rubik_RotateStr("lBL"); break;
                case RC_GREEN:  rubik_RotateStr("uFrf"); break;
            }
        } else if (Rubik[RF_RIGHT][7] == RC_WHITE) {
            found = 1;
            switch (Rubik[RF_UP][5]) {
                case RC_RED:    rubik_RotateStr("rFR"); break;
                case RC_BLUE:   rubik_RotateStr("UfLF"); break;
                case RC_ORANGE: rubik_RotateStr("Rbr"); break;
                case RC_GREEN:  rubik_RotateStr("UFrf"); break;
            }
        } else if (Rubik[RF_BACK][7] == RC_WHITE) {
            found = 1;
            switch (Rubik[RF_UP][7]) {
                case RC_RED:    rubik_RotateStr("uLfl"); break;
                case RC_BLUE:   rubik_RotateStr("Blb"); break;
                case RC_ORANGE: rubik_RotateStr("ulBL"); break;
                case RC_GREEN:  rubik_RotateStr("bRB"); break;
            }
        }
        if (!found) break;
        Solve_WhiteCross_2();
    }
}

void Solve_WhiteCross_4(void) {
    while (1) {
        int found = 0;
        // Find edges with white at positions 3 and 5 of front/back faces
        if ((Rubik[RF_FRONT][3] == RC_WHITE) && (Rubik[RF_LEFT][5] != RC_BLUE)) {
            rubik_RotateStr("lUL"); found = 1;
        } else if ((Rubik[RF_FRONT][3] == RC_WHITE) && (Rubik[RF_LEFT][5] == RC_BLUE)) {
            rubik_RotateStr("L"); found = 1;
        } else if ((Rubik[RF_FRONT][5] == RC_WHITE) && (Rubik[RF_RIGHT][3] != RC_GREEN)) {
            rubik_RotateStr("RUr"); found = 1;
        } else if ((Rubik[RF_FRONT][5] == RC_WHITE) && (Rubik[RF_RIGHT][3] == RC_GREEN)) {
            rubik_RotateStr("r"); found = 1;
        } else if ((Rubik[RF_BACK][3] == RC_WHITE) && (Rubik[RF_RIGHT][5] != RC_GREEN)) {
            rubik_RotateStr("ruR"); found = 1;
        } else if ((Rubik[RF_BACK][3] == RC_WHITE) && (Rubik[RF_RIGHT][5] == RC_GREEN)) {
            rubik_RotateStr("R"); found = 1;
        } else if ((Rubik[RF_BACK][5] == RC_WHITE) && (Rubik[RF_LEFT][3] != RC_BLUE)) {
            rubik_RotateStr("LUl"); found = 1;
        } else if ((Rubik[RF_BACK][5] == RC_WHITE) && (Rubik[RF_LEFT][3] == RC_BLUE)) {
            rubik_RotateStr("l"); found = 1;
        }
        if (!found) break;
        Solve_WhiteCross_2();
        Solve_WhiteCross_3();
    }
}

void Solve_WhiteCross_5(void) {
    while (1) {
        int found = 0;
        // Find edges with white at positions 3 and 5 of left/right faces
        if ((Rubik[RF_LEFT][3] == RC_WHITE) && (Rubik[RF_BACK][5] != RC_ORANGE)) {
            rubik_RotateStr("buB"); found = 1;
        } else if ((Rubik[RF_LEFT][3] == RC_WHITE) && (Rubik[RF_BACK][5] == RC_ORANGE)) {
            rubik_RotateStr("B"); found = 1;
        } else if ((Rubik[RF_LEFT][5] == RC_WHITE) && (Rubik[RF_FRONT][3] != RC_RED)) {
            rubik_RotateStr("FUf"); found = 1;
        } else if ((Rubik[RF_LEFT][5] == RC_WHITE) && (Rubik[RF_FRONT][3] == RC_RED)) {
            rubik_RotateStr("f"); found = 1;
        } else if ((Rubik[RF_RIGHT][3] == RC_WHITE) && (Rubik[RF_FRONT][5] != RC_RED)) {
            rubik_RotateStr("fuF"); found = 1;
        } else if ((Rubik[RF_RIGHT][3] == RC_WHITE) && (Rubik[RF_FRONT][5] == RC_RED)) {
            rubik_RotateStr("F"); found = 1;
        } else if ((Rubik[RF_RIGHT][5] == RC_WHITE) && (Rubik[RF_BACK][3] != RC_ORANGE)) {
            rubik_RotateStr("BUb"); found = 1;
        } else if ((Rubik[RF_RIGHT][5] == RC_WHITE) && (Rubik[RF_BACK][3] == RC_ORANGE)) {
            rubik_RotateStr("b"); found = 1;
        }
        if (!found) break;
        Solve_WhiteCross_2();
        Solve_WhiteCross_3();
        Solve_WhiteCross_4();
    }
}

void Solve_WhiteCross_6(void) {
    if (Rubik[RF_FRONT][1] == RC_WHITE) {
        rubik_RotateStr("FF");
    }
    if (Rubik[RF_RIGHT][1] == RC_WHITE) {
        rubik_RotateStr("RR");
    }
    if (Rubik[RF_LEFT][1] == RC_WHITE) {
        rubik_RotateStr("LL");
    }
    if (Rubik[RF_BACK][1] == RC_WHITE) {
        rubik_RotateStr("bb");
    }
    Solve_WhiteCross_3();
    Solve_WhiteCross_4();
    Solve_WhiteCross_5();
}

// End of rubik_SolveCross_atmega32.c
