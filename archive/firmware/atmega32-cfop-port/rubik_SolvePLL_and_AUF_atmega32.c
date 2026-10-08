// rubik_SolvePLL_and_AUF_atmega32.c
// ATMega32 C port of rubik_SolvePLL_and_AUF.ino
// All Arduino/Serial code replaced with standard C and project abstractions
// Calls rubik_RotateStr for move execution

#include "a_constants_atmega32.c" // Rubik, color constants, etc.

void AUF(void); // Adjust Up Face

void Solve_PLL(void) {
    int caso = -1;
    // PLL Skip (already solved)
    if ((Rubik[RF_FRONT][6] == Rubik[RF_FRONT][7]) && (Rubik[RF_FRONT][7] == Rubik[RF_FRONT][8]) &&
        (Rubik[RF_LEFT][6] == Rubik[RF_LEFT][7]) && (Rubik[RF_LEFT][7] == Rubik[RF_LEFT][8]) &&
        (Rubik[RF_BACK][6] == Rubik[RF_BACK][7]) && (Rubik[RF_BACK][7] == Rubik[RF_BACK][8]) &&
        (Rubik[RF_RIGHT][6] == Rubik[RF_RIGHT][7]) && (Rubik[RF_RIGHT][7] == Rubik[RF_RIGHT][8])) {
        caso = 0;
    } else {
        for (int i = 0; i < 4; i++) {
            // Cases with headlights on the left (most cases)
            if (Rubik[RF_LEFT][6] == Rubik[RF_LEFT][8] && Rubik[RF_FRONT][6] != Rubik[RF_FRONT][8]) {
                // T case
                if (Rubik[RF_FRONT][6] == Rubik[RF_FRONT][7] && Rubik[RF_BACK][7] == Rubik[RF_BACK][8]) {
                    rubik_RotateStr("RUrurFRRuruRUrf"); caso = 1; break;
                }
                // F case
                else if (Rubik[RF_LEFT][7] == Rubik[RF_LEFT][6] &&
                         (Rubik[RF_FRONT][8] == Rubik[RF_BACK][6]) && (Rubik[RF_BACK][6] == Rubik[RF_RIGHT][7])) {
                    rubik_RotateStr("rufRUrurFRRuruRUrUR"); caso = 2; break;
                }
                // A clockwise
                else if (Rubik[RF_FRONT][7] == Rubik[RF_FRONT][8] && Rubik[RF_RIGHT][6] == Rubik[RF_RIGHT][7]) {
                    rubik_RotateStr("UrFrBBRfrBBrr"); caso = 3; break;
                }
                // A counterclockwise
                else if (Rubik[RF_RIGHT][7] == Rubik[RF_RIGHT][8] && Rubik[RF_BACK][6] == Rubik[RF_BACK][7]) {
                    rubik_RotateStr("uRbRffrBRffRR"); caso = 4; break;
                }
                // Ra
                else if (Rubik[RF_FRONT][6] == Rubik[RF_FRONT][7] &&
                         (Rubik[RF_FRONT][8] == Rubik[RF_BACK][6]) && (Rubik[RF_BACK][6] == Rubik[RF_RIGHT][7])) {
                    rubik_RotateStr("RUrfRUUrUUrFRURuur"); caso = 5; break;
                }
                // Rb
                else if (Rubik[RF_BACK][7] == Rubik[RF_BACK][8] &&
                         (Rubik[RF_FRONT][8] == Rubik[RF_BACK][6]) && (Rubik[RF_BACK][6] == Rubik[RF_RIGHT][7])) {
                    rubik_RotateStr("urUUrdRurDRURuruR"); caso = 6; break;
                }
                // Ja
                else if (Rubik[RF_FRONT][6] == Rubik[RF_FRONT][7] && Rubik[RF_RIGHT][6] == Rubik[RF_RIGHT][7] &&
                         Rubik[RF_BACK][6] == Rubik[RF_BACK][7] && Rubik[RF_LEFT][6] == Rubik[RF_LEFT][8]) {
                    rubik_RotateStr("UUluLFluLULfllUL"); caso = 7; break;
                }
                // Jb
                else if (Rubik[RF_FRONT][7] == Rubik[RF_FRONT][8] && Rubik[RF_RIGHT][7] == Rubik[RF_RIGHT][8] &&
                         Rubik[RF_BACK][7] == Rubik[RF_BACK][8] && Rubik[RF_LEFT][6] == Rubik[RF_LEFT][8]) {
                    rubik_RotateStr("RUUruRUUlUruL"); caso = 8; break;
                }
                // G1
                else if (Rubik[RF_FRONT][7] == Rubik[RF_FRONT][8]) {
                    rubik_RotateStr("dRRUrUruRuRRuDrUR"); caso = 9; break;
                }
                // G2
                else if (Rubik[RF_BACK][6] == Rubik[RF_BACK][7]) {
                    rubik_RotateStr("DRRuRuRUrUrrUdRur"); caso = 10; break;
                }
                // G3
                else if (Rubik[RF_RIGHT][6] == Rubik[RF_RIGHT][7]) {
                    rubik_RotateStr("RUruDRRuRurUrUrrd"); caso = 11; break;
                }
                // G4
                else if (Rubik[RF_RIGHT][7] == Rubik[RF_RIGHT][8]) {
                    rubik_RotateStr("DruRUdRRUrURuRurr"); caso = 12; break;
                }
            }
            // Cases without headlights
            if (Rubik[RF_FRONT][6] == Rubik[RF_FRONT][7] && Rubik[RF_RIGHT][7] == Rubik[RF_RIGHT][8] &&
                Rubik[RF_FRONT][8] == Rubik[RF_BACK][6]) {
                rubik_RotateStr("FUfrrFufuRRUrrURR"); caso = 13; break;
            }
            // V case
            else if (Rubik[RF_FRONT][6] == Rubik[RF_FRONT][7] && Rubik[RF_LEFT][7] == Rubik[RF_LEFT][8] &&
                     Rubik[RF_FRONT][8] == Rubik[RF_BACK][6] && Rubik[RF_RIGHT][6] != Rubik[RF_RIGHT][7]) {
                rubik_RotateStr("rUURuuLurUlULuRUl"); caso = 14; break;
            }
            // E case
            else if ((Rubik[RF_FRONT][7] == RC_RED) && (Rubik[RF_LEFT][7] == RC_BLUE) &&
                     (Rubik[RF_BACK][7] == RC_ORANGE) && (Rubik[RF_RIGHT][7] == RC_GREEN) &&
                     (Rubik[RF_FRONT][8] == Rubik[RF_BACK][6])) {
                rubik_RotateStr("rUlddLuRlUrDDRuL"); caso = 15; break;
            }
            // Never appears 1
            else if (Rubik[RF_LEFT][7] == Rubik[RF_LEFT][8] && Rubik[RF_BACK][7] == Rubik[RF_BACK][8] &&
                     Rubik[RF_RIGHT][7] == Rubik[RF_RIGHT][8] && Rubik[RF_FRONT][7] == Rubik[RF_FRONT][8] &&
                     Rubik[RF_RIGHT][6] != Rubik[RF_RIGHT][8] && Rubik[RF_FRONT][6] != Rubik[RF_FRONT][8] &&
                     Rubik[RF_BACK][6] != Rubik[RF_BACK][8] && Rubik[RF_LEFT][6] != Rubik[RF_LEFT][8]) {
                rubik_RotateStr("LuRUUlUrLuRuulUr"); caso = 16; break;
            }
            // Never appears 2
            else if (Rubik[RF_LEFT][6] == Rubik[RF_LEFT][7] && Rubik[RF_BACK][6] == Rubik[RF_BACK][7] &&
                     Rubik[RF_RIGHT][6] == Rubik[RF_RIGHT][7] && Rubik[RF_FRONT][6] == Rubik[RF_FRONT][7] &&
                     Rubik[RF_RIGHT][6] != Rubik[RF_RIGHT][8] && Rubik[RF_FRONT][6] != Rubik[RF_FRONT][8] &&
                     Rubik[RF_BACK][6] != Rubik[RF_BACK][8] && Rubik[RF_LEFT][6] != Rubik[RF_LEFT][8]) {
                rubik_RotateStr("lUrUULuRlUrUULuR"); caso = 17; break;
            }
            // Cases with headlights on all faces
            if (Rubik[RF_LEFT][6] == Rubik[RF_LEFT][8] && Rubik[RF_BACK][6] == Rubik[RF_BACK][8] &&
                Rubik[RF_RIGHT][6] == Rubik[RF_RIGHT][8] && Rubik[RF_FRONT][6] == Rubik[RF_FRONT][8]) {
                // U clockwise
                if (Rubik[RF_FRONT][7] == Rubik[RF_FRONT][6] && Rubik[RF_LEFT][7] == Rubik[RF_BACK][8]) {
                    rubik_RotateStr("rUrururURUrr"); caso = 18; break;
                }
                // U counterclockwise
                else if (Rubik[RF_FRONT][7] == Rubik[RF_FRONT][6] && Rubik[RF_LEFT][7] == Rubik[RF_RIGHT][8]) {
                    rubik_RotateStr("LuLULULuluLL"); caso = 19; break;
                }
                // H
                else if (Rubik[RF_FRONT][7] == Rubik[RF_BACK][6] && Rubik[RF_LEFT][7] == Rubik[RF_RIGHT][6]) {
                    rubik_RotateStr("rrUURuuRRuuRRuuRUUrr"); caso = 20; break;
                }
                // Z
                else if (Rubik[RF_FRONT][7] == Rubik[RF_RIGHT][6] && Rubik[RF_LEFT][7] == Rubik[RF_BACK][6]) {
                    rubik_RotateStr("RUrUrurURuruRRUR"); caso = 21; break;
                }
            }
            rubik_RotateStr("u"); // Rotate U face and try again
        }
    }
    AUF(); // Adjust Up Face
}

// Adjust Up Face (AUF)
void AUF(void) {
    switch (Rubik[RF_FRONT][7]) {
        case RC_RED:    rubik_RotateStr("");   break;
        case RC_BLUE:   rubik_RotateStr("U");  break;
        case RC_ORANGE: rubik_RotateStr("uu"); break;
        case RC_GREEN:  rubik_RotateStr("u");  break;
    }
}

// End of rubik_SolvePLL_and_AUF_atmega32.c
