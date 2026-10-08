// z_main_atmega32.c
// Main entry point for Rubik's Cube Solver (ATMega32 version)
// Ported from Arduino z_main.ino

#include <avr/io.h>
#include <util/delay.h>
#include "prints_atmega32.c"           // UART print routines
#include "a_constants_atmega32.c"      // Rubik, Rubik_Colors, etc.
#include "pwm_servo_atmega32.c"        // PCA9685 and PWM routines
#include "a_servos_atmega32.c"         // Servo abstraction and movement
#include "bluethoot_receiver_atmega32.c" // Bluetooth (if needed)
#include "guardar_cores_atmega32.c"    // Cube color storage

int main(void) {
    // Initialize hardware
    servos_init_hw(); // PCA9685 and I2C
    uart_init(9600);  // UART for debug/HC-06
    _delay_ms(2000);
    //SetColorPoints(); // If needed
    rubik_Init();
    sequencia_de_armazenamento(); // Read cube colors
    rubik_Print();
    //servos_test(); // Optional
    Menu_Print();

    // Main loop (single run, like Arduino loop)
    // Menu_Process(); // Optional
    // servos_test(); // Optional
    uart_println("Solving the cube...");
    _delay_ms(600);
    uart_print("."); _delay_ms(600);
    uart_print("."); _delay_ms(800);
    uart_print("\n\nMoves to solve the cube:\n");
    rubik_SolveSequence();
    servos_init_hw(); // Re-init servos if needed
    rubik_Print();
    while(1) {
        // Infinite loop (halt)
    }
    return 0;
}

// End of z_main_atmega32.c
