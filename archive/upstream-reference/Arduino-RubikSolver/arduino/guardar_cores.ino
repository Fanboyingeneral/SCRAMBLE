#define DELAY_BETWEEN_STEPS1 (500) // A specific delay for the scanning sequence.
#define APERTO (4) // "Tightness" - a value for a firm grip during scanning moves.


// This helper function moves the servos to get a clear view of a side face for scanning.
static void clearVision() {
  
  // 1. Retract the Right and Left pushers.
  Down_Right.rmove(place[3]);
  Down_Left.rmove(place[3]);
  delay(DELAY_BETWEEN_STEPS1);
  // 2. Grip the cube tightly with the Front and Back pushers.
  Down_Front.rmove(place[1] + APERTO);
  Down_Back.rmove(place[1] + APERTO);
  delay(DELAY_BETWEEN_STEPS1);
  // 3. Use the top rotators to tilt the cube, presenting a side face.
  Up_Back.Rotate(0);
  Up_Front.Rotate(180);
  
}

// This function defines the entire mechanical sequence for scanning all 6 faces of the cube.
void sequencia_de_armazenamento() { // "storage sequence"

   // --- Scan Right and Left Faces ---
   clearVision(); // Prepare the cube.
   getBit(RF_RIGHT); // Scan the Right face.
   Up_Back.Rotate(180); // Rotate the top servos...
   Up_Front.Rotate(0);
   getBit(RF_LEFT); // ...to scan the Left face.
   
   // --- Reorient Cube to Scan Up and Down Faces ---
   Up_Front.Rotate(90); // Reset top rotators.
   Up_Back.Rotate(90);
   Down_Front.rmove(place[1]); // Reset Front/Back pushers to neutral.
   Down_Back.rmove(place[1]);
   delay(DELAY_BETWEEN_STEPS1);
   Down_Right.rmove(place[1] + APERTO); // Grip with Left/Right pushers.
   Down_Left.rmove(place[1] + APERTO);
   delay(DELAY_BETWEEN_STEPS1);
   Down_Front.rmove(place[3]); // Retract Front/Back pushers.
   Down_Back.rmove(place[3]);
   delay(DELAY_BETWEEN_STEPS1);
   Up_Right.Rotate(0); // Use top rotators to present the Up face.
   Up_Left.Rotate(180);
   getBit(RF_UP); // Scan the Up face.
   Up_Right.Rotate(180); // Rotate to present the Down face.
   Up_Left.Rotate(0);
   getBit(RF_DOWN); // Scan the Down face.

   // --- Reorient Cube to Scan Front and Back Faces ---
   Up_Right.Rotate(90); // Reset top rotators.
   Up_Left.Rotate(90);
   Down_Right.rmove(place[1]); // Reset all pushers to neutral.
   Down_Left.rmove(place[1]);
   delay(DELAY_BETWEEN_STEPS1);
   Down_Back.rmove(place[1]);
   Down_Front.rmove(place[1]);
   delay(DELAY_BETWEEN_STEPS1);
   ServosCube_MoveZ(); // Perform a whole-cube 'z' rotation.
   
   // --- Scan Front and Back Faces ---
   clearVision(); // Prepare the cube in its new orientation.
   getBit(RF_FRONT); // Scan the Front face.
   Up_Back.Rotate(180); // Rotate top servos...
   Up_Front.Rotate(0);
   getBit(RF_BACK); // ...to scan the Back face.

   // --- Final Reset ---
   Up_Back.Rotate(90); // Reset top rotators.
   Up_Front.Rotate(90);
   Down_Front.rmove(place[1]); // Reset all pushers to neutral.
   Down_Back.rmove(place[1]);
   delay(DELAY_BETWEEN_STEPS1);
   Down_Left.rmove(place[1]);
   Down_Right.rmove(place[1]);
   delay(DELAY_BETWEEN_STEPS1);
   ServosCube_Movez(); // Rotate the cube back to its starting orientation.
   delay(DELAY_BETWEEN_STEPS1);
}
