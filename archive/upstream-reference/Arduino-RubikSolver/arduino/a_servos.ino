#include <EEPROM.h>


#include <Adafruit_PWMServoDriver.h> // Library for the 16-channel PWM/Servo driver board
#include <Wire.h> // Library for I2C communication, used by the PWM driver library

// Create an object to represent the PWM driver board
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  140 // This is the 'minimum' pulse length count (out of 4096). Represents 0 degrees.
#define SERVOMAX  670 // This is the 'maximum' pulse length count (out of 4096). Represents 180 degrees.

#define DELAY_BETWEEN_STEPS (150) // A standard delay in milliseconds for servo movements.
#define DELAY_BETWEEN_STEPS2 (500) // A longer delay for more complex or critical movements.

// An array of position offsets for the Pusher/Gripper servos.
// These values are relative to the servo's calibrated center point (phy_med).
// For example, place[1] (0) is the neutral hold position. place[3] (-24) is fully retracted.
int place[] = {15, 0, -18, -24};

//Base class for all servos in the project.
//It contains the common properties and methods.
class RServo {
  protected: // These variables can be accessed by RServo and its derived classes (Pusher, Rotater)

    int channel;        // The channel number (0-15) on the PWM driver board.
    int pwm_min;        // The minimum PWM pulse value for this servo.
    int pwm_max;        // The maximum PWM pulse value for this servo.
    int phy_min;        // The minimum physical angle (e.g., 0 degrees).
    int phy_max;        // The maximum physical angle (e.g., 180 degrees).
    int phy_current;    // Stores the current physical angle.
    int pwm_current;    // Stores the current PWM value sent to the servo.

  public:
    // Constructor for the RServo class. Runs when an RServo object is created.
    RServo() { 
      // Initialize the min/max PWM values from the global constants.
      this->pwm_min = SERVOMIN;
      this->pwm_max = SERVOMAX;
    }

    // A utility function for manually tuning/calibrating a servo via the Serial Monitor.
    void afinar() { // tune
      static int PWM = 400; // Start with a default middle PWM value.
      Serial.println(PWM);

      if (Serial.available()) { // Check if a command has been sent from the Serial Monitor.
        Serial.println("Aqui\n"); // "Here" - a debug message.
        switch (Serial.read()) { // Read the command character.
          case 'h': // 'h' for high, large increment
            setPWM(PWM += 10);
            Serial.println(PWM);
            break;
          case 'l': // 'l' for low, large decrement
            setPWM(PWM -= 10);
            Serial.println(PWM);
            break;
          case '+': // small increment
            setPWM(PWM++);
            Serial.println(PWM);
            break;
          case '-': // small decrement
            setPWM(PWM--);
            Serial.println(PWM);
            break;
        }
      }
    }

    // This function sends the final PWM command to the servo.
    void setPWM(int value) {
      // Tell the pwm object to set the PWM signal on a specific channel.
      // The '0' is the "on" tick (out of 4096), and 'value' is the "off" tick.
      // This creates a pulse of a specific length.
      pwm.setPWM(this->channel, 0, value);
      // Store the value that was just sent.
      pwm_current = value;
    }
};

// A specialized class for the servos that ROTATE the cube faces.
// It inherits from the base RServo class.
class Rotater : public RServo {
  private:

    int pwm_med; // The specific PWM value for the 90-degree (neutral) position.
    int phy_med; // The physical angle for the neutral position (always 90).

  public:
    // Constructor for the Rotater class.
    // Takes the channel and calibration offsets for min, med, and max positions.
    Rotater(int channel, int pwm_min, int pwm_med, int pwm_max) {
      this->channel = channel;
      this->pwm_min += pwm_min; // Apply a specific offset to the default SERVOMIN.
      this->pwm_med = pwm_med;   // Set the calibrated 90-degree PWM value.
      this->pwm_max += pwm_max; // Apply a specific offset to the default SERVOMAX.
      phy_min = 0;
      phy_med = 90;
      phy_max = 180;
    }

    // Moves the servo to a precise, final angle (0, 90, or 180).
    // Used for setting a state, not for the turn itself.
    void Rotate(int graus) { // degrees
      int p;
      switch (graus) { // degrees
        case 0:
          p = pwm_min;
          break;
        case 90:
          p = pwm_med;
          break;
        case 180:
          p = pwm_max;
          break;
      }
      setPWM(p);
    }

// A small value to compensate for mechanical slack/slop in the servo gears.
#define backlash (10)

    // Performs a rotation, compensating for backlash to ensure accuracy.
    // This is used for the actual turning of the cube faces.
    void rotate(int graus) {
      int p;
      switch (graus) {
        case 0:
          // Overshoot the target slightly to engage the gears firmly.
          p = pwm_min - backlash;
          break;
        case 90:
          p = pwm_med;
          // Apply backlash depending on the direction of approach to 90 degrees.
          if (pwm_current < pwm_med) {
            p += backlash;
          } else if (pwm_current > pwm_med) {
            p -= backlash;
          }
          break;
        case 180:
          // Overshoot the target slightly.
          p = pwm_max + backlash;
          break;
      }

      setPWM(p);
    }

    void afinar() { // tune
      static int PWM = 400;
      Serial.println(PWM);

      if (Serial.available()) {
        Serial.println("Aqui\n"); // Here
        switch (Serial.read()) {
          case 'h':
            setPWM(PWM += 10);
            Serial.println(PWM);
            break;
          case 'l':
            setPWM(PWM -= 10);
            Serial.println(PWM);
            break;
          case '+':
            setPWM(PWM++);
            Serial.println(PWM);
            break;
          case '-':
            setPWM(PWM--);
            Serial.println(PWM);
            break;
        }
      }
    }
};

// A specialized class for the servos that PUSH or GRIP the cube.
// It inherits from the base RServo class.
class Pusher : public RServo {
  private:
    int phy_med; // The calibrated center position for this pusher servo.

  public:
    // Constructor for the Pusher class.
    Pusher(int channel, int phy_med) {
      this->channel = channel;
      this->phy_med = phy_med;
      phy_min = 0;
      // Pushers have a smaller required range of motion.
      phy_max = 60;
    }

    // Performs a "relative move". Takes a physical offset and applies it.
    void rmove(int phy) {
      // map(value, fromLow, fromHigh, toLow, toHigh)
      // Add the desired relative movement to the calibrated center point.
      phy += phy_med;
      // Translate the desired physical position (e.g., 0-60) into a raw PWM value.
      int p = map(phy, this->phy_min, this->phy_max, this->pwm_min, this->pwm_max);
      setPWM(p);
    }

    void afinar() { // tune
      static int PWM = 400;
      Serial.println(PWM);
      int p = map(PWM, this->pwm_min, this->pwm_max, this->phy_min, this->phy_max);
      Serial.println();
      Serial.println(p);

      if (Serial.available()) {
        Serial.println("Aqui\n"); // Here
        switch (Serial.read()) {
          case 'h':
            setPWM(PWM += 10);
            break;
          case 'l':
            setPWM(PWM -= 10);
            break;
          case '+':
            setPWM(PWM++);
            break;
          case '-':
            setPWM(PWM--);
            break;
        }
      }
    }
};


// Create the 8 servo objects for the robot.
// Each line defines the object name, its PWM channel, and its specific calibration values.

// Rotater(channel, pwm_min_offset, pwm_med, pwm_max_offset)
Rotater Up_Right(0, 5, 405, 0);             //motor para rodar a face da direita no pino 0 -> motor to rotate the right face on pin 0
Rotater Up_Back(1, 0, 400, 0);              //motor para rodar a face de trás no pino 1 -> motor to rotate the back face on pin 1
Rotater Up_Left(2, 0, 400, 0);              //motor para rodar a face da esquerda no pino 2 -> motor to rotate the left face on pin 2
Rotater Up_Front(3, -20, 380, 0);           //motor para rodar a face da frente no pino 3 -> motor to rotate the front face on pin 3

// Pusher(channel, phy_med)
Pusher Down_Right(4, 46);            //motor para andar a face da direita no pino 4 -> motor to move the right face on pin 4
Pusher Down_Back(5, 47);             //motor para andar a face de trás no pino 5 -> motor to move the back face on pin 5
Pusher Down_Left(6, 48);             //motor para andar a face da esquerda no pino 6 -> motor to move the left face on pin 6
Pusher Down_Front(7, 46);            //motor para andar a face da frente no pino 7 -> motor to move the front face on pin 7



void servos_init();
// This function initializes all servos to their starting positions.
void servos_init() {
  // 1. Set all top rotators to their neutral 90-degree position.
  Up_Right.Rotate(90);
  Up_Back.Rotate(90);
  Up_Left.Rotate(90);
  Up_Front.Rotate(90);
  // 2. Retract all bottom pushers to make space for the cube.
  Down_Right.rmove(-30);
  Down_Back.rmove(-30);
  Down_Left.rmove(-30);
  Down_Front.rmove(-30);

  // 3. Pause for 2.5 seconds to allow the user to place the cube.
  delay(2500);
  // 4. Gently grip the cube by moving the pushers to their neutral position (place[1] = 0).
  // This is done in two steps for stability.
  Down_Back.rmove(place[1]);
  Down_Right.rmove(place[1]);
  delay(500);
  Down_Front.rmove(place[1]);
  Down_Left.rmove(place[1]);

//  delay(500);
//  Down_Back.rmove(place[3]);
}

// A function to test all mechanical movements of the robot.
// It is called if uncommented in the main loop.
void servos_test() {

//  Down_Right.rmove(place[0]);
//  Down_Back.rmove(place[0]);
//  Down_Left.rmove(place[0]);
//  Down_Front.rmove(place[0]);
//  for(int i = 10; i < 60; i++) {
//    Down_Front.rmove(i);
//    Down_Back.rmove(i);
//    Serial.println(i);
//    delay(50);
//  }
//  delay(2000);
  // Test all clockwise face turns
  ServosFace_RightCW();
  delay(500);
  ServosFace_BackCW();
  delay(500);
  ServosFace_LeftCW();
  delay(500);
  ServosFace_FrontCW();
  delay(500);
  // Test cube rotations
  ServosCube_MoveX();
  delay(1000);
  ServosCube_Movex();
  delay(1000);
  ServosCube_MoveZ();
  delay(1000);
  ServosCube_Movez();
  delay(1000);
  // Test all counter-clockwise face turns
  ServosFace_RightCCW();
  delay(1000);
  ServosFace_BackCCW();
  delay(500);
  ServosFace_LeftCCW();
  delay(500);
  ServosFace_FrontCCW();
  delay(500);
}



//Rotações dos servos da direita: -> Rotations of the right servos:
//______________________________________________________________________

// Sequence to turn the Right face Clockwise.
void ServosFace_RightCW() {
  Up_Right.rotate(180);            // 1. Engage the face by turning the rotator.
  delay(DELAY_BETWEEN_STEPS2);
  Down_Right.rmove(place[3]);     // 2. Release the pusher to allow movement.
  delay(DELAY_BETWEEN_STEPS);
  Up_Right.Rotate(90);             // 3. Rotate the face to the 90-degree position.
  delay(DELAY_BETWEEN_STEPS);
  Down_Right.rmove(place[1]);     // 4. Re-grip with the pusher.
}

// Sequence to turn the Right face Counter-Clockwise.
void ServosFace_RightCCW() {
  Up_Right.rotate(0);              // 1. Engage the face.
  delay(DELAY_BETWEEN_STEPS2);
  Down_Right.rmove(place[3]);     // 2. Release pusher.
  delay(DELAY_BETWEEN_STEPS);
  Up_Right.Rotate(90);             // 3. Rotate face.
  delay(DELAY_BETWEEN_STEPS);
  Down_Right.rmove(place[1]);     // 4. Re-grip.
}


//Rotações dos servos da esquerda: -> Rotations of the left servos:
//___________________________________________________________________

void ServosFace_LeftCW() {
  Up_Left.rotate(0);              delay(DELAY_BETWEEN_STEPS2);
  Down_Left.rmove(place[3]);     delay(DELAY_BETWEEN_STEPS);
  Up_Left.Rotate(90);             delay(DELAY_BETWEEN_STEPS);
  Down_Left.rmove(place[1]);     delay(DELAY_BETWEEN_STEPS);
}

void ServosFace_LeftCCW() {
  Up_Left.rotate(180);            delay(DELAY_BETWEEN_STEPS2);
  Down_Left.rmove(place[3]);     delay(DELAY_BETWEEN_STEPS);
  Up_Left.Rotate(90);             delay(DELAY_BETWEEN_STEPS);
  Down_Left.rmove(place[1]);     delay(DELAY_BETWEEN_STEPS);
}


//Rotações dos servos da frente: -> Rotations of the front servos:
//___________________________________________________________________________

void ServosFace_FrontCW() {
  Up_Front.rotate(0);            delay(DELAY_BETWEEN_STEPS2);
  Down_Front.rmove(place[3]);     delay(DELAY_BETWEEN_STEPS);
  Up_Front.Rotate(90);             delay(DELAY_BETWEEN_STEPS);
  Down_Front.rmove(place[1]);     delay(DELAY_BETWEEN_STEPS);
}
//
void ServosFace_FrontCCW() {
  Up_Front.rotate(180);            delay(DELAY_BETWEEN_STEPS2);
  Down_Front.rmove(place[3]);     delay(DELAY_BETWEEN_STEPS);
  Up_Front.Rotate(90);             delay(DELAY_BETWEEN_STEPS);
  Down_Front.rmove(place[1]);     delay(DELAY_BETWEEN_STEPS);
}


//Rotações dos servos de trás: -> Rotations of the back servos:
//______________________________________________________________________________

void ServosFace_BackCW() {
  Up_Back.rotate(180);            delay(DELAY_BETWEEN_STEPS2);
  Down_Back.rmove(place[3]);     delay(DELAY_BETWEEN_STEPS);
  Up_Back.Rotate(90);             delay(DELAY_BETWEEN_STEPS);
  Down_Back.rmove(place[1]);     delay(DELAY_BETWEEN_STEPS);
}

void ServosFace_BackCCW() {
  Up_Back.rotate(0);            delay(DELAY_BETWEEN_STEPS2);
  Down_Back.rmove(place[3]);     delay(DELAY_BETWEEN_STEPS);
  Up_Back.Rotate(90);             delay(DELAY_BETWEEN_STEPS);
  Down_Back.rmove(place[1]);     delay(DELAY_BETWEEN_STEPS);
}


// Rotations of the motors to turn the cube in the R direction (x and x` movement):
//__________________________________________________________________________

#define aperto (2) // tightness

//Tilts the cube forward. That is the top becomes the front
void ServosCube_MoveX() {
  // Step 1: Release Front/Back
  Down_Front.rmove(place[2]);
  Down_Back.rmove(place[2]);
  delay(DELAY_BETWEEN_STEPS);

  // Step 2: Grip Tightly on Left/Right
  Down_Right.rmove(aperto);
  Down_Left.rmove(aperto);
  delay(100);

  // Step 3: Perform the "Hand-off"
  Up_Right.Rotate(180);
  Up_Left.Rotate(0);
  delay(160);

  // Step 4: Re-grip with Front/Back
  Down_Front.rmove(place[1] + aperto);
  Down_Back.rmove(place[1] + aperto);
  delay(DELAY_BETWEEN_STEPS2);

  // Step 5: Release Left/Right
  Down_Right.rmove(place[3]);
  Down_Left.rmove(place[3]);
  delay(DELAY_BETWEEN_STEPS);

  // Step 6: Reset Rotators
  Up_Right.Rotate(90);
  Up_Left.Rotate(90);
  delay(DELAY_BETWEEN_STEPS);

  // Step 7: Reset all Pushers to Neutral
  Down_Right.rmove(place[1]);
  Down_Left.rmove(place[1]);
  Down_Front.rmove(place[1]);
  Down_Back.rmove(place[1]);
  delay(DELAY_BETWEEN_STEPS);
}

//Tilts the cube backward. That is the top becomes the back
// This is the reverse of the MoveX function.
void ServosCube_Movex() {
  
  Down_Front.rmove(place[2]);
  Down_Back.rmove(place[2]);
  delay(DELAY_BETWEEN_STEPS);
  Down_Right.rmove(aperto);      
  Down_Left.rmove(aperto);       
  delay(100);                
  Up_Right.Rotate(0);
  Up_Left.Rotate(180);
  delay(200);
  Down_Front.rmove(place[1] + aperto);
  Down_Back.rmove(place[1] + aperto);
  delay(DELAY_BETWEEN_STEPS2);                      
  Down_Right.rmove(place[3]);
  Down_Left.rmove(place[3]);
  delay(DELAY_BETWEEN_STEPS);
  Up_Right.Rotate(90);
  Up_Left.Rotate(90);
  delay(DELAY_BETWEEN_STEPS);
  Down_Right.rmove(place[1]);
  Down_Left.rmove(place[1]);
  Down_Front.rmove(place[1]);
  Down_Back.rmove(place[1]);
  delay(DELAY_BETWEEN_STEPS);
}


//Rotações dos motores para rodar o cubo no sentido de F (movimento z e z`): -> Rotations of the motors to turn the cube in the F direction (z and z` movement):
//_________________________________________________________________________

void ServosCube_MoveZ() {
  
  Down_Right.rmove(place[2]);
  Down_Left.rmove(place[2]);
  delay(DELAY_BETWEEN_STEPS);
  Down_Back.rmove(aperto);      //
  Down_Front.rmove(aperto);       //
  delay(100);                //
  Up_Front.Rotate(0);
  Up_Back.Rotate(180);
  delay(180);
  Down_Right.rmove(place[1] + aperto);
  Down_Left.rmove(place[1] + aperto);
  delay(DELAY_BETWEEN_STEPS2);                      
  Down_Front.rmove(place[3]);
  Down_Back.rmove(place[3]);
  delay(DELAY_BETWEEN_STEPS);
  Up_Front.Rotate(90);
  Up_Back.Rotate(90);
  delay(DELAY_BETWEEN_STEPS);
  Down_Front.rmove(place[1]);
  Down_Back.rmove(place[1]);
  Down_Right.rmove(place[1]);
  Down_Left.rmove(place[1]);
  delay(DELAY_BETWEEN_STEPS);
}

void ServosCube_Movez() {

  Down_Right.rmove(place[2]);
  Down_Left.rmove(place[2]);
  delay(DELAY_BETWEEN_STEPS);
  Down_Back.rmove(aperto);      //
  Down_Front.rmove(aperto);       //
  delay(100);                //
  Up_Front.Rotate(180);
  Up_Back.Rotate(0);
  delay(180);
  Down_Right.rmove(place[1] + aperto);
  Down_Left.rmove(place[1] + aperto);
  delay(DELAY_BETWEEN_STEPS2);
  Down_Front.rmove(place[3]);
  Down_Back.rmove(place[3]);
  delay(DELAY_BETWEEN_STEPS);
  Up_Front.Rotate(90);
  Up_Back.Rotate(90);
  delay(DELAY_BETWEEN_STEPS);
  Down_Front.rmove(place[1]);
  Down_Back.rmove(place[1]);
  Down_Right.rmove(place[1]);
  Down_Left.rmove(place[1]);
  delay(DELAY_BETWEEN_STEPS);
}

//Rotações dos motores para rodar o cubo no sentido de U (movimento Y e Y ): -> Rotations of the motors to turn the cube in the U direction (Y and Y movement):
//_________________________________________________________________________

void ServosCube_MoveY() {
  ServosCube_MoveX();
  ServosCube_MoveZ();
  ServosCube_Movex();
}

void ServosCube_Movey() {
  ServosCube_MoveX();
  ServosCube_Movez();
  ServosCube_Movex();
}
