/*
 * ============================================================================
 * MASTER CONTROLLER (MASTER_CONTROLLER.C) - V3 - Refactored
 * ============================================================================
 *
 * This is the MASTER ATmega32. It is the "brain" of the operation.
 *
 * RESPONSIBILITIES:
 * 1. Listens for all commands from the user via the HC-05 Bluetooth module.
 * 2. Controls the four TOP motors directly based on single-character commands.
 * 3. Controls the four BASE motors via 4-wire digital communication to the Slave.
 *
 * HARDWARE:
 * - HC-05 connected to the Hardware USART (PD0/RX, PD1/TX).
 * - Top motors connected to hardware PWM pins (PB3, PD4, PD5, PD7).
 * - 4 digital output pins (PC0, PC1, PC2, PC3) connected to Slave input pins.
 * - Must share a common GND with the Slave ATmega32.
 *
 * COMMAND PROTOCOL:
 * - Top Motors (1 char): F/f, B/b, L/l, R/r (uppercase=0deg, lowercase=90deg)
 * - Base Motors (1 char): W/w, X/x, Y/y, Z/z (uppercase=min, lowercase=max)
 * - Base communication: HIGH=max position, LOW=min position on each pin
 */

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

// --- HARDWARE USART (for Bluetooth) ---
#define HW_BAUD_RATE 9600
#define HW_UBRR_VALUE ((F_CPU / (8UL * HW_BAUD_RATE)) - 1)

// --- BASE MOTOR CONTROL PINS (Master->Slave communication) ---
// These pins will be HIGH for max position, LOW for min position
#define BASE_FRONT_PIN   PC0
#define BASE_BACK_PIN    PC1
#define BASE_LEFT_PIN    PA2
#define BASE_RIGHT_PIN   PA3


// --- TOP MOTOR CALIBRATION VALUES ---
#define FRONT_MOTOR_0_DEG   75
#define FRONT_MOTOR_90_DEG  210
#define BACK_MOTOR_0_DEG    78
#define BACK_MOTOR_90_DEG   204
#define LEFT_MOTOR_0_DEG    7
#define LEFT_MOTOR_90_DEG   24
#define RIGHT_MOTOR_0_DEG   8
#define RIGHT_MOTOR_90_DEG  24

// --- Function Prototypes ---
void hw_usart_init(void);
void hw_usart_transmit_string(const char* str);
char hw_usart_receive_char(void);
void setup_all(void);

// Top Motor Functions
void Top_Front_0_Deg(void);
void Top_Front_90_Deg(void);
void Top_Back_0_Deg(void);
void Top_Back_90_Deg(void);
void Top_Left_0_Deg(void);
void Top_Left_90_Deg(void);
void Top_Right_0_Deg(void);
void Top_Right_90_Deg(void);

// Base Motor Functions
void Base_Front_Min(void);
void Base_Front_Max(void);
void Base_Back_Min(void);
void Base_Back_Max(void);
void Base_Left_Min(void);
void Base_Left_Max(void);
void Base_Right_Min(void);
void Base_Right_Max(void);

void L1(void);
void L2(void);
void L3(void);

void F1(void);
void F2(void);
void F3(void);

void R1(void);
void R2(void);
void R3(void);

void B1(void);
void B2(void);
void B3(void);

void U1(void);
void U2(void);
void U3(void);

void D1(void);
void D2(void);
void D3(void);

void Rotate_top();
void Rotate_bottom();


void execute_sequence(const char* sequence) {
	const char* p = sequence; // Use a pointer to traverse the string

	while (*p != '\0' && *p != '(') {
		// 1. Skip any whitespace characters (spaces, tabs, etc.)
		while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
			p++;
		}

		// Check if we've reached the end or the stop character after skipping spaces
		if (*p == '\0' || *p == '(') {
			break;
		}

		// 2. We should now be at the start of a command.
		// Check for a valid two-character command.
		if (*p != '\0' && *(p + 1) != '\0' && *(p + 1) != ' ') {
			char command = *p;
			char number  = *(p + 1);

			// Call the appropriate function based on the command pair
			if      (command == 'L' && number == '1') { L1(); }
			else if (command == 'L' && number == '2') { L2(); }
			else if (command == 'L' && number == '3') { L3(); }
			else if (command == 'R' && number == '1') { R1(); }
			else if (command == 'R' && number == '2') { R2(); }
			else if (command == 'R' && number == '3') { R3(); }
			else if (command == 'F' && number == '1') { F1(); }
			else if (command == 'F' && number == '2') { F2(); }
			else if (command == 'F' && number == '3') { F3(); }
			else if (command == 'B' && number == '1') { B1(); }
			else if (command == 'B' && number == '2') { B2(); }
			else if (command == 'B' && number == '3') { B3(); }
			else if (command == 'U' && number == '1') { U1(); }
			else if (command == 'U' && number == '2') { U2(); }
			else if (command == 'U' && number == '3') { U3(); }
			else if (command == 'D' && number == '1') { D1(); }
			else if (command == 'D' && number == '2') { D2(); }
			else if (command == 'D' && number == '3') { D3(); }

			
			// Move pointer past the two characters of the executed command
			p += 2;
			} else {
			// Invalid format (e.g., a single character at the end), so stop.
			break;
		}
	}
}



int main(void) {
	const char* solve_sequence = "F2 U3 F2 U3 B2 D1 L2 U2 F2 L1 B3 L1 F3 U1 B3 D1 L3 F3 R1 B1 (20f)";
    setup_all();

    // Initialize top motors to 90 degrees (Clockwise position)
    Base_Right_Min();
    Base_Left_Min();
    Base_Back_Min();
    Base_Front_Min();
    Top_Front_90_Deg();
    Top_Back_90_Deg();
    Top_Left_90_Deg();
    Top_Right_90_Deg();
	_delay_ms(10000);
    Base_Back_Max();
    _delay_ms(2000);
    Base_Front_Max();
    _delay_ms(2000);
    Base_Right_Max();
    _delay_ms(2000);
    Base_Left_Max();
    _delay_ms(2000);
    hw_usart_transmit_string("\r\n--- Master Controller Ready ---\r\n");
	
	//U3();
	

    while (1) {
        // Wait for a command character from Bluetooth
        char cmd = hw_usart_receive_char();

        switch (cmd) {
            // --- TOP MOTOR COMMANDS (Anti-Clockwise) ---
            case 'F': Top_Front_0_Deg(); break;
            case 'B': Top_Back_0_Deg();  break;
            case 'L': Top_Left_0_Deg(); break;
            case 'R': Top_Right_0_Deg();  break;
			case 'A': U3(); break;
            case 'S':
            hw_usart_transmit_string("Executing solve sequence...\r\n");
            execute_sequence(solve_sequence);
			            hw_usart_transmit_string("Sequence complete.\r\n");
			            break;
			            
            // --- TOP MOTOR COMMANDS (Clockwise) ---
            case 'f': Top_Front_90_Deg(); break;
            case 'b': Top_Back_90_Deg();  break;
            case 'l': Top_Left_90_Deg(); break;
            case 'r': Top_Right_90_Deg(); break;

            // --- BASE MOTOR COMMANDS (Min Position) ---
            case 'W': Base_Front_Min(); break;
            case 'X': Base_Back_Min(); break;
            case 'Y': Base_Left_Min(); break;
            case 'Z': Base_Right_Min(); break;

            // --- BASE MOTOR COMMANDS (Max Position) ---
            case 'w': Base_Front_Max(); break;
            case 'x': Base_Back_Max(); break;
            case 'y': Base_Left_Max(); break;
            case 'z': Base_Right_Max(); break;

            default:
                hw_usart_transmit_string("Unknown command.\r\n");
                break;
        }
    }
    return 0;
}

/**
 * @brief Initializes all hardware components for the Master MCU.
 */
void setup_all(void) {
    // --- Pin Setup ---
    // Top motor pins as outputs
    DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD7);
    DDRB |= (1 << PB3);
    // Hardware USART TX pin (for Bluetooth) as output
    DDRD |= (1 << PD1);
    // Base motor control pins as outputs
    DDRC |= (1 << BASE_FRONT_PIN) | (1 << BASE_BACK_PIN);
    DDRA |= (1 << BASE_LEFT_PIN) | (1 << BASE_RIGHT_PIN);

    // --- Hardware USART Init ---
    hw_usart_init();

    // --- Timer Setup ---
    // Timer 1 (~61Hz) for PD4, PD5
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    ICR1 = 2048;
    // Timer 2 (~61Hz) for PD7
    TCCR2 = (1 << WGM21) | (1 << WGM20) | (1 << COM21) | (1 << CS22);
    // Timer 0 (~61Hz) for PB3
    TCCR0 = (1 << WGM01) | (1 << WGM00) | (1 << COM01) | (1 << CS01) | (1 << CS00);
}

// --- Standard Hardware USART Functions ---

void hw_usart_init(void) {
    UBRRH = (uint8_t)(HW_UBRR_VALUE >> 8);
    UBRRL = (uint8_t)HW_UBRR_VALUE;
    UCSRA = (1 << U2X);
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void hw_usart_transmit_char(char data) {
    while (!(UCSRA & (1 << UDRE)));
    UDR = data;
}

void hw_usart_transmit_string(const char* str) {
    while (*str) {
        hw_usart_transmit_char(*str++);
    }
}

char hw_usart_receive_char(void) {
    while (!(UCSRA & (1 << RXC)));
    return UDR;
}

// --- TOP MOTOR FUNCTIONS ---

/**
 * @brief Sets the Top Front motor to 0 degrees (Anti-Clockwise).
 */
void Top_Front_0_Deg(void) {
    OCR1B = FRONT_MOTOR_0_DEG;
    hw_usart_transmit_string("Top Front -> 0 deg\r\n");
}

/**
 * @brief Sets the Top Front motor to 90 degrees (Clockwise).
 */
void Top_Front_90_Deg(void) {
    OCR1B = FRONT_MOTOR_90_DEG;
    hw_usart_transmit_string("Top Front -> 90 deg\r\n");
}

/**
 * @brief Sets the Top Back motor to 0 degrees (Anti-Clockwise).
 */
void Top_Back_0_Deg(void) {
    OCR1A = BACK_MOTOR_0_DEG;
    hw_usart_transmit_string("Top Back -> 0 deg\r\n");
}

/**
 * @brief Sets the Top Back motor to 90 degrees (Clockwise).
 */
void Top_Back_90_Deg(void) {
    OCR1A = BACK_MOTOR_90_DEG;
    hw_usart_transmit_string("Top Back -> 90 deg\r\n");
}

/**
 * @brief Sets the Top Left motor to 0 degrees (Anti-Clockwise).
 */
void Top_Left_0_Deg(void) {
    OCR2 = LEFT_MOTOR_0_DEG;
    hw_usart_transmit_string("Top Left -> 0 deg\r\n");
}

/**
 * @brief Sets the Top Left motor to 90 degrees (Clockwise).
 */
void Top_Left_90_Deg(void) {
    OCR2 = LEFT_MOTOR_90_DEG;
    hw_usart_transmit_string("Top Left -> 90 deg\r\n");
}

/**
 * @brief Sets the Top Right motor to 0 degrees (Anti-Clockwise).
 */
void Top_Right_0_Deg(void) {
    OCR0 = RIGHT_MOTOR_0_DEG;
    hw_usart_transmit_string("Top Right -> 0 deg\r\n");
}

/**
 * @brief Sets the Top Right motor to 90 degrees (Clockwise).
 */
void Top_Right_90_Deg(void) {
    OCR0 = RIGHT_MOTOR_90_DEG;
    hw_usart_transmit_string("Top Right -> 90 deg\r\n");
}

// --- BASE MOTOR FUNCTIONS ---

/**
 * @brief Sets the Base Front motor to its minimum position.
 */
void Base_Front_Min(void) {
    PORTC &= ~(1 << BASE_FRONT_PIN);
    hw_usart_transmit_string("Base Front -> Min\r\n");
}

/**
 * @brief Sets the Base Front motor to its maximum position.
 */
void Base_Front_Max(void) {
    PORTC |= (1 << BASE_FRONT_PIN);
    hw_usart_transmit_string("Base Front -> Max\r\n");
}

/**
 * @brief Sets the Base Back motor to its minimum position.
 */
void Base_Back_Min(void) {
    PORTC &= ~(1 << BASE_BACK_PIN);
    hw_usart_transmit_string("Base Back -> Min\r\n");
}


/**
 * @brief Sets the Base Back motor to its maximum position.
 */
void Base_Back_Max(void) {
    PORTC |= (1 << BASE_BACK_PIN);
    hw_usart_transmit_string("Base Back -> Max\r\n");
}

/**
 * @brief Sets the Base Left motor to its minimum position.
 */
void Base_Left_Min(void) {
    PORTA &= ~(1 << BASE_LEFT_PIN);hw_usart_transmit_string("Base Left -> Min\r\n");
}

/**
 * @brief Sets the Base Left motor to its maximum position.
 */
void Base_Left_Max(void) {
    PORTA |= (1 << BASE_LEFT_PIN);hw_usart_transmit_string("Base Left -> Max\r\n");
}

/**
 * @brief Sets the Base Right motor to its minimum position.
 */
void Base_Right_Min(void) {
    PORTA &= ~(1 << BASE_RIGHT_PIN);hw_usart_transmit_string("Base Right -> Min\r\n");
}

/**
 * @brief Sets the Base Right motor to its maximum position.
 */
void Base_Right_Max(void) {
    PORTA |= (1 << BASE_RIGHT_PIN);hw_usart_transmit_string("Base Right -> Max\r\n");
}

void R3(void)
{
	Top_Right_0_Deg();
	_delay_ms(900);
	Base_Right_Min();
	_delay_ms(900);
	Top_Right_90_Deg();
	_delay_ms(900);
	Base_Right_Max();
	_delay_ms(900);
}
void R2(void)
{
	Top_Right_0_Deg();
	_delay_ms(900);
	Base_Right_Min();
	_delay_ms(900);
	Top_Right_90_Deg();
	_delay_ms(900);
	Base_Right_Max();
	_delay_ms(900);
	Top_Right_0_Deg();
	_delay_ms(900);
	Base_Right_Min();
	_delay_ms(900);
	Top_Right_90_Deg();
	_delay_ms(900);
	Base_Right_Max();
	_delay_ms(900);
}
void R1(void)
{
	Base_Right_Min();
	_delay_ms(900);
	Top_Right_0_Deg();
	_delay_ms(900);
	Base_Right_Max();
	_delay_ms(900);
	Top_Right_90_Deg();
	_delay_ms(900);
}
void B3(void)
{
	Top_Back_0_Deg();
	_delay_ms(900);
	Base_Front_Min();
	_delay_ms(900);
	Top_Back_90_Deg();
	_delay_ms(900);
	Base_Front_Max();
	_delay_ms(900);
}
void B2(void)
{
	Top_Back_0_Deg();
	_delay_ms(900);
	Base_Front_Min();
	_delay_ms(900);
	Top_Back_90_Deg();
	_delay_ms(900);
	Base_Front_Max();
	_delay_ms(900);
	Top_Back_0_Deg();
	_delay_ms(900);
	Base_Front_Min();
	_delay_ms(900);
	Top_Back_90_Deg();
	_delay_ms(900);
	Base_Front_Max();
	_delay_ms(900);
}
void B1(void)
{
	Base_Front_Min();
	_delay_ms(900);
	Top_Back_0_Deg();
	_delay_ms(900);
	Base_Front_Max();
	_delay_ms(900);
	Top_Back_90_Deg();
	_delay_ms(900);
}
void F3(void)
{
	Top_Front_0_Deg();
	_delay_ms(900);
	Base_Back_Min();
	_delay_ms(900);
	Top_Front_90_Deg();
	_delay_ms(900);
	Base_Back_Max();
	_delay_ms(900);
}
void F2(void)
{
	Top_Front_0_Deg();
	_delay_ms(900);
	Base_Back_Min();
	_delay_ms(900);
	Top_Front_90_Deg();
	_delay_ms(900);
	Base_Back_Max();
	_delay_ms(900);
	Top_Front_0_Deg();
	_delay_ms(900);
	Base_Back_Min();
	_delay_ms(900);
	Top_Front_90_Deg();
	_delay_ms(900);
	Base_Back_Max();
	_delay_ms(900);
}
void F1(void)
{
	Base_Back_Min();
	_delay_ms(900);
	Top_Front_0_Deg();
	_delay_ms(900);
	Base_Back_Max();
	_delay_ms(900);
	Top_Front_90_Deg();
	_delay_ms(900);
}
void L3(void)
{
	Top_Left_0_Deg();
	_delay_ms(900);
	Base_Left_Min();
	_delay_ms(900);
	Top_Left_90_Deg();
	_delay_ms(900);
	Base_Left_Max();
	_delay_ms(900);
}

void L2(void)
{
	Top_Left_0_Deg();
	_delay_ms(900);
	Base_Left_Min();
	_delay_ms(900);
	Top_Left_90_Deg();
	_delay_ms(900);
	Base_Left_Max();
	_delay_ms(900);
	Top_Left_0_Deg();
	_delay_ms(900);
	Base_Left_Min();
	_delay_ms(900);
	Top_Left_90_Deg();
	_delay_ms(900);
	Base_Left_Max();
	_delay_ms(900);
}
void L1(void)
{
	Base_Left_Min();
	_delay_ms(900);
	Top_Left_0_Deg();
	_delay_ms(900);
	Base_Left_Max();
	_delay_ms(900);
	Top_Left_90_Deg();
	_delay_ms(900);
}


void Rotate_top()
   {
	Base_Left_Min();
	_delay_ms(900);
	Top_Left_0_Deg();
	_delay_ms(900);
	Base_Left_Max();
	_delay_ms(900);
	Base_Front_Min();
	_delay_ms(900);
	Base_Back_Min();
	_delay_ms(900);
	Top_Left_90_Deg();
	Top_Right_0_Deg();
	_delay_ms(900);
	Base_Front_Max();
	_delay_ms(900);
	Base_Back_Max();
	_delay_ms(900);
	Base_Right_Min();
	_delay_ms(900);
	Top_Right_90_Deg();
	_delay_ms(900);
	Base_Right_Max();
	_delay_ms(900);
}

void Rotate_bottom()
{
	Base_Right_Min();
	_delay_ms(900);
	Top_Right_0_Deg();
	_delay_ms(900);
	Base_Right_Max();
	_delay_ms(900);
	Base_Front_Min();
	_delay_ms(900);
	Base_Back_Min();
	_delay_ms(900);
	Top_Left_0_Deg();
	Top_Right_90_Deg();
	_delay_ms(900);
	Base_Front_Max();
	_delay_ms(900);
	Base_Back_Max();
	_delay_ms(900);
	Base_Left_Min();
	_delay_ms(900);
	Top_Left_90_Deg();
	_delay_ms(900);
	Base_Left_Max();
	_delay_ms(900);
}

void U1(void)
{
	Rotate_top();
	F1();
	Rotate_bottom();
}

void U2(void)
{
	Rotate_top();
	F2();
	Rotate_bottom();
}

void U3(void)
{
	Rotate_top();
	F3();
	Rotate_bottom();
}
void D1(void)
{
	Rotate_bottom();
	F1();
	Rotate_top();
}

void D2(void)
{
	Rotate_bottom();
	F2();
	Rotate_top();
}

void D3(void)
{
	Rotate_bottom();
	F3();
	Rotate_top();
}