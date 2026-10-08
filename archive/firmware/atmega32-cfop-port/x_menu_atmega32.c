// x_menu_atmega32.c
// ATMega32 C port of x_menu.ino (menu and UART command handler)
// All Arduino/Serial code replaced with prints_atmega32.c UART routines
// All logic and menu structure preserved

#include "prints_atmega32.c" // UART print routines
#include "Rubik_atmega32.c"  // Rubik logic and move parser
#include <util/delay.h>

void Menu_Print(void) {
    uart_println("M E N U");
    uart_println("==============");
    uart_println("i   Iniciar cubo montado");
    uart_println("a   Afinar motores");
    uart_println("s   scramble the cube");
    uart_println("S   Solve the Cube");
    uart_println("");
}

void Menu_Process(void) {
    char cmd = uart_read(); // Blocking read from UART
    switch(cmd) {
        case 'i':
            Rubik_Init();
            Rubik_Print();
            break;
        case 'a':
            uart_println("choose the motor to tune (B/R/F/L):");
            while (!uart_available()) {}
            switch(uart_read()) {
                case 'B': while(1) { Up_Back_afinar(); } break;
                case 'R': while(1) { Up_Right_afinar(); } break;
                case 'F': while(1) { Up_Front_afinar(); } break;
                case 'L': while(1) { Up_Left_afinar(); } break;
            }
            break;
        case 'f': Rubik_Rotate('f', 1); Rubik_Print(); break;
        case 'F': Rubik_Rotate('F', 1); Rubik_Print(); break;
        case 'r': Rubik_Rotate('r', 1); Rubik_Print(); break;
        case 'R': Rubik_Rotate('R', 1); Rubik_Print(); break;
        case 'u': Rubik_Rotate('u', 1); Rubik_Print(); break;
        case 'U': Rubik_Rotate('U', 1); Rubik_Print(); break;
        case 'l': Rubik_Rotate('l', 1); Rubik_Print(); break;
        case 'L': Rubik_Rotate('L', 1); Rubik_Print(); break;
        case 'd': Rubik_Rotate('d', 1); Rubik_Print(); break;
        case 'D': Rubik_Rotate('D', 1); Rubik_Print(); break;
        case 'b': Rubik_Rotate('b', 1); Rubik_Print(); break;
        case 'B': Rubik_Rotate('B', 1); Rubik_Print(); break;
        case 'X': ServosCube_MoveX(); break;
        case 'x': ServosCube_Movex(); break;
        case 'Z': ServosCube_MoveZ(); break;
        case 'z': ServosCube_Movez(); break;
        case 'Y': ServosCube_MoveY(); break;
        case 'y': ServosCube_Movey(); break;
        case 's': Rubik_Scramble(30); Rubik_Print(); break;
        case 'm': Menu_Print(); break;
        case 'M': Rubik_Init(); break;
        case 'S':
            uart_print("Solving the cube."); _delay_ms(600);
            uart_print("."); _delay_ms(600);
            uart_print("."); _delay_ms(800); uart_print("\n\nMoves to solve the cube:\n");
            Rubik_SolveSequence();
            lightAnimation();
            break;
        case 'p': servos_init(); Rubik_Print(); break;
        default:
            uart_println("Comando Invalido");
    }
}

// End of x_menu_atmega32.c
