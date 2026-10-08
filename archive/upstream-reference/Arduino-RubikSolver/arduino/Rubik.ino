#include <avr/pgmspace.h>

#define ProgStrArray_Index(prog_str_array, i)  (char*)pgm_read_word(&(prog_str_array[i]))

// The main class for managing the cube's state and operations.
class Cubo { // "Cubo" means "Cube"
  public:
    // Constructor for the Cubo class.
    Cubo() {
      
    }
    // Initializes the virtual cube to a perfectly solved state.
    void Init() {
      // preencher cores -> fill colors
      //________________________________________________________________
      int face;
      int i;
      // For each of the 6 faces...
      for (face = 0; face < RF_TOTAL_FACES; face ++) {
        // ...and for each of the 9 stickers on that face...
        for (i = 0; i < 9; i++) {
            // ...assign it the correct color based on the Look-Up Table.
            int color = Rubik_FaceToColorLUT[face];
            Rubik[face][i] = color;
        }
      }
    }

    void Print();

    
    // This is the main dispatcher function. It executes a single move, both virtually and, if requested, physically.
    // 'rot' is the character for the move (e.g., 'R', 'f').
    // 'motorsMove' determines if the physical robot should perform the action.
    void Rubik_Rotate(char rot, bool motorsMove) {
 // static char storeMove;
  bool MotorRotate = motorsMove;
/*
  if((storeMove == rot) && (rot != 'Y')){
    Serial.print("2 ");   
    MotorRotate = false; 
    HTM--;
  } else {
    Serial.print(" ");    
    MotorRotate = true; 
    HTM++;  QTM++;
  }
  storeMove = rot;
 */ 
  // The switch statement translates a move character into specific actions.
  // NOTE on orientation: The cube is oriented inside the robot such that the logical UP face
  // is handled by the physical FRONT rotator, and the logical DOWN face by the BACK rotator.
  // The logical FRONT face points "skyward" and has no direct rotator.
  switch(rot) {
    // A 'f' move is a counter-clockwise turn of the Front face.
    case 'f':  
      RotateFrontCCW();  // 1. Update the virtual cube array.
      if(MotorRotate == true){ // 2. If a physical move is requested...
        // ...the robot must tilt the cube back, turn the face, and tilt it forward again.
        ServosCube_Movex();
        ServosFace_BackCCW();
        ServosCube_MoveX();
      } 
      Serial.print(F("F'")); // Log the move to the serial monitor.
      break;
    // An 'F' move is a clockwise turn of the Front face.
    case 'F':  
      RotateFrontCW();
      if(MotorRotate == true){
        // This is a complex move requiring a whole-cube tilt.
        ServosCube_Movex();
        ServosFace_BackCW();
        ServosCube_MoveX(); 
      }   
      Serial.print(F("F")); 
      break;
    // An 'r' move is a counter-clockwise turn of the Right face.
    case 'r':  
      RotateRightCCW();
      if(MotorRotate == true){ 
        // This is a direct move; the Right rotator can perform it.
        ServosFace_RightCCW();  
      }  
      Serial.print(F("R'"));
      break;
    // An 'R' move is a clockwise turn of the Right face.
    case 'R':  
      RotateRightCW(); 
      if(MotorRotate == true){
        // Direct move: Logical Right -> Physical Right Rotator.
        ServosFace_RightCW();  
      }  
      Serial.print(F("R"));  
      break;
    // An 'u' move is a counter-clockwise turn of the Up face.
    case 'u':  
      RotateUpCCW();
      if(MotorRotate == true){     
        // Direct move: Logical Up -> Physical Front Rotator.
        ServosFace_FrontCW();
      }
      Serial.print(F("U'"));
      break;
    // An 'U' move is a clockwise turn of the Up face.
    case 'U': 
      RotateUpCW(); 
      if(MotorRotate == true){     
        // Direct move: Logical Up -> Physical Front Rotator.
        ServosFace_FrontCCW(); 
      }   
      Serial.print(F("U")); 
      break;
    case 'l':  
      RotateLeftCCW(); 
      if(MotorRotate == true){ 
        // Direct move: Logical Left -> Physical Left Rotator.
        ServosFace_LeftCW();  
      }   
      Serial.print(F("L'"));
      break;
    case 'L':  
      RotateLeftCW();
      if(MotorRotate == true){
        // Direct move: Logical Left -> Physical Left Rotator.
        ServosFace_LeftCCW();     
      }   
      Serial.print(F("L"));
      break;
    case 'd':  
      RotateDownCCW(); 
      if(MotorRotate == true){
        // Direct move: Logical Down -> Physical Back Rotator.
        ServosFace_BackCCW();  
      }  
      Serial.print(F("D'")); 
      break;
    case 'D':  
      RotateDownCW(); 
      if(MotorRotate == true){ 
        // Direct move: Logical Down -> Physical Back Rotator.
        ServosFace_BackCW();    
      }  
      Serial.print(F("D"));
      break;
    case 'b':  
      RotateBackCCW();  
      if(MotorRotate == true){
        // Complex move: Logical Back face requires a tilt to be turned.
        ServosCube_Movex();
        ServosFace_FrontCW();
        ServosCube_MoveX();
      }  
      Serial.print(F("B'")); 
      break;
    case 'B':  
      RotateBackCW();  
      if(MotorRotate == true){  
        // Complex move: Logical Back face requires a tilt to be turned.
        ServosCube_Movex();
        ServosFace_FrontCCW();
        ServosCube_MoveX();
      }   
      Serial.print(F("B")); 
      break;
    // A 'Y' move is a whole-cube rotation.
    case 'Y':  
      RotateY();
      if(MotorRotate == true){  
        ServosCube_MoveZ();
      }  
      if((HTM > 0) && (QTM > 0)) {
        HTM--;  QTM--;
      }  
      break;
  }
  // If a physical move was made, print the new state of the virtual cube for verification.
  if(MotorRotate == true){  
    Print();
  }
}


  // Executes a sequence of moves from a standard String object, updating the virtual cube only.
  // It also appends the sequence to the main resolution string.
  void RotateStr(String sequence) {
  
    for (int i = 0; i < sequence.length(); i++) {
      // Call the main rotator with 'motorsMove' set to false for virtual-only execution.
      Rubik_Rotate(sequence.charAt(i), false);
      // Add this move to the complete solution sequence.
      resolutionStr += sequence.charAt(i);
    }
  }

  // Executes a sequence of moves stored in PROGMEM (Flash memory) to save RAM.
  // This is used for the large algorithm look-up tables (F2L, OLL, PLL).
  void RotateStr_Progmem(char *sequence) {
    static char buff[30]; // A temporary buffer in RAM to hold the algorithm.
    
    // Copy the string from program memory (PROGMEM) to the RAM buffer.
    strcpy_P(buff, sequence);
    int len = strlen(buff);
  
    // Process the algorithm from the buffer.
    for (int i = 0; i < len; i++) {
      Rubik_Rotate(buff[i], false);
      resolutionStr += buff[i];
    }
  }

    // Optimizes the global 'resolutionStr' by simplifying and canceling moves.
    void ProcessStr(int caze = 0) {
      bool flag = true;
      // Loop repeatedly until no more optimizations can be made in a full pass.
      while(flag == true) {
        flag = false;
        for(int i = 0; i < resolutionStr.length(); i++) {
          switch(caze) {
            case 0:
              // Case 1: A move and its inverse cancel each other out (e.g., R then R').
              if(resolutionStr[i] == 'r' && resolutionStr[i + 1] == 'R' || resolutionStr[i] == 'R' && resolutionStr[i + 1] == 'r' ||
                resolutionStr[i] == 'l' && resolutionStr[i + 1] == 'L'  || resolutionStr[i] == 'L' && resolutionStr[i + 1] == 'l' ||
                resolutionStr[i] == 'f' && resolutionStr[i + 1] == 'F'  || resolutionStr[i] == 'F' && resolutionStr[i + 1] == 'f' ||
                resolutionStr[i] == 'b' && resolutionStr[i + 1] == 'B'  || resolutionStr[i] == 'B' && resolutionStr[i + 1] == 'b' ||
                resolutionStr[i] == 'u' && resolutionStr[i + 1] == 'U'  || resolutionStr[i] == 'U' && resolutionStr[i + 1] == 'u' ||
                resolutionStr[i] == 'd' && resolutionStr[i + 1] == 'D'  || resolutionStr[i] == 'D' && resolutionStr[i + 1] == 'd') {
                  resolutionStr.remove(i, 2); // Remove the pair.
                  i++;
                  flag = true; // Mark that a change was made.
                  //Serial.print("  C1  "); 
              // Case 2: Three identical moves (e.g., R R R) are equivalent to one inverse move (r).
              } else if((resolutionStr[i] == resolutionStr[i + 1]) && (resolutionStr[i] == resolutionStr[i + 2])) {
                  if(isUpperCase(resolutionStr[i])) {
                    resolutionStr[i] = toLowerCase(resolutionStr[i]);
                  } else {
                    resolutionStr[i] = toUpperCase(resolutionStr[i]);
                  }
                  resolutionStr.remove(i + 1, 2); // Remove the extra two moves.
                  i++;
                  flag = true;
                  //Serial.print("  C2  "); 
              // Case 3: Four identical moves result in no change.
              } else if((resolutionStr[i] == resolutionStr[i + 1]) &&  (resolutionStr[i] == resolutionStr[i + 2]) && (resolutionStr[i] == resolutionStr[i + 3])) {
                resolutionStr.remove(i, 4); // Remove all four.
                i++;
                flag = true;
                //Serial.print("  C3  "); 
              }
              break;
            case 1:
              // This case would convert double moves (e.g., R R) to "R2" notation. It is currently unused.
              if(resolutionStr[i] == 'r' && resolutionStr[i + 1] == 'r' || resolutionStr[i] == 'R' && resolutionStr[i + 1] == 'R' ||
                resolutionStr[i] == 'l' && resolutionStr[i + 1] == 'l'  || resolutionStr[i] == 'L' && resolutionStr[i + 1] == 'L' ||
                resolutionStr[i] == 'f' && resolutionStr[i + 1] == 'f'  || resolutionStr[i] == 'F' && resolutionStr[i + 1] == 'F' ||
                resolutionStr[i] == 'b' && resolutionStr[i + 1] == 'b'  || resolutionStr[i] == 'B' && resolutionStr[i + 1] == 'B' ||
                resolutionStr[i] == 'u' && resolutionStr[i + 1] == 'u'  || resolutionStr[i] == 'U' && resolutionStr[i + 1] == 'U' ||
                resolutionStr[i] == 'd' && resolutionStr[i + 1] == 'd'  || resolutionStr[i] == 'D' && resolutionStr[i + 1] == 'D') {
                  resolutionStr[i + 1] = '2';
                  i++;
                  flag = true;
                  //Serial.print("  C4  "); 
              } 
              break;
            case 2:
              return;
              break;
            }
          }
          //Serial.print("\nResolução = ");  Serial.print(resolutionStr);  Serial.print(F("\n")); 
       }
       
       // The recursive call is commented out; the function relies on the while loop instead.
       //Rubik_ProcessStr(caze + 1);
    }
  
    // The main function that orchestrates the entire solving process from start to finish.
    void SolveSequence() {
      // 1. Create a backup of the scrambled cube to solve on, preserving the original.
      CopyCube(Rubik, Rubik_ToSolve);
      Print();
      delay(1000);
      // 2. Call the solving stages in order (CFOP method).
      Solve_WhiteCross_1 ();
      Solve_WhiteCross_2 ();
      Solve_WhiteCross_3 ();
      Solve_WhiteCross_4 ();
      Solve_WhiteCross_5 ();
      Solve_WhiteCross_6 ();
      SolveF2L();
      Solve_OLL();
      Solve_PLL();
      // 3. Optimize the generated solution string.
      ProcessStr();
      // 4. Copy the solved state back to the main virtual cube for display.
      CopyCube(Rubik_ToSolve, Rubik);
      // 5. Execute the final, optimized solution on the physical robot.
      for(int i = 0; i < resolutionStr.length(); i++) {
        Rubik_Rotate(resolutionStr.charAt(i), true); // 'true' enables motor movement.
      }
      // 6. Print the final results.
      Serial.print("\nResolução = ");  Serial.print(resolutionStr);  Serial.print(F("\n")); 
      Serial.print("        (HTM = ");  Serial.print(HTM);  Serial.print(F(")")); 
      Serial.print("  (QTM = ");  Serial.print(QTM);  Serial.print(F(")"));  Serial.print(F("\n"));
      Print();
    }
  
    // Scrambles the virtual cube with a given number of random moves.
    void Sramble(int rotations_count) {
      // An array of all possible single moves.
      static const char rotations[] = {'f', 'F', 'r', 'R', 'u', 'U', 'l', 'L', 'd', 'D', 'b', 'B'};
    
      randomSeed(millis()); // Initialize the pseudo-random generator.
      
      for (int i = 0; i < rotations_count; i++) {
        char i_rot = random(sizeof(rotations)); // Pick a random move from the array.
        Rubik_Rotate(rotations[i_rot], false); // Apply the move virtually.
      }
      Serial.println(F("\n"));
    }

    // A utility function to copy the sticker data from one cube array to another.
    void CopyCube(unsigned char de[][9], unsigned char para[][9]) { // from, to

      Serial.print(F("A")); 
      for (int face = 0; face < RF_TOTAL_FACES; face ++) {
        for (int i = 0; i < 9; i++) {
            para[face][i] = de[face][i];
        }
      }
    }
    
  private:
    // A temporary backup of the cube state, used during virtual rotations.
    unsigned char Rubik_Backup[RF_TOTAL_FACES][9];
    // A separate copy of the scrambled cube that the solving algorithms will modify.
    unsigned char Rubik_ToSolve[RF_TOTAL_FACES][9];

    // A string to store the final sequence of moves for the solution.
    String resolutionStr = "";
    
    // Counters for Quarter Turn Metric and Half Turn Metric.
    int QTM;
    int HTM;

  public:
    void RotateRightCW();
    void RotateRightCCW();
    void RotateLeftCW();
    void RotateLeftCCW();
    void RotateUpCW();
    void RotateUpCCW();
    void RotateDownCW();
    void RotateDownCCW();
    void RotateFrontCW();
    void RotateFrontCCW();
    void RotateBackCW();
    void RotateBackCCW();
    void RotateY();

  private:
    // This private helper function copies the main Rubik array to the backup array.
    // This must be done before every virtual turn.
    void MakeBackup() {
      int face , i;
      for (face = 0; face < RF_TOTAL_FACES; face++) {
        for (i = 0; i < 9; i++) {
            Rubik_Backup [face][i] = Rubik [face][i];
        }
      }
    }
};

Cubo rubik;

//__________________________________________________________________________________
// Rotações: -> Rotations:
// These functions are the core of the virtual cube simulation. Each one modifies the
// Rubik[][] array to simulate a physical turn. They all follow the same pattern:
// 1. Call MakeBackup() to save the current state of the cube.
// 2. Re-assign the 8 stickers on the face being turned (4 corners, 4 edges).
// 3. Re-assign the 12 adjacent stickers on the four surrounding faces.
//__________________________________________________________________________________
//
// Rotação da face da frente: -> Rotation of the front face:
//____________________________________________________________

// Simulates a counter-clockwise turn of the Front face (F').
void Cubo::RotateFrontCCW () {
  MakeBackup();
  // --- Rotate stickers on the Front face itself ---
  //Corners of the face
  Rubik[RF_FRONT][0] = Rubik_Backup[RF_FRONT][6];
  Rubik[RF_FRONT][2] = Rubik_Backup[RF_FRONT][0];
  Rubik[RF_FRONT][8] = Rubik_Backup[RF_FRONT][2];
  Rubik[RF_FRONT][6] = Rubik_Backup[RF_FRONT][8];
  //Edges of the face
  Rubik[RF_FRONT][1] = Rubik_Backup[RF_FRONT][3];
  Rubik[RF_FRONT][5] = Rubik_Backup[RF_FRONT][1];
  Rubik[RF_FRONT][7] = Rubik_Backup[RF_FRONT][5];
  Rubik[RF_FRONT][3] = Rubik_Backup[RF_FRONT][7];
  // --- Move adjacent stickers on surrounding faces ---
  //3 top pieces move to the left
  Rubik[RF_UP][0] = Rubik_Backup[RF_RIGHT][6];
  Rubik[RF_UP][1] = Rubik_Backup[RF_RIGHT][3];
  Rubik[RF_UP][2] = Rubik_Backup[RF_RIGHT][0];
  //3 right pieces move up
  Rubik[RF_RIGHT][6] = Rubik_Backup[RF_DOWN][8];
  Rubik[RF_RIGHT][3] = Rubik_Backup[RF_DOWN][7];
  Rubik[RF_RIGHT][0] = Rubik_Backup[RF_DOWN][6];
  //3 bottom pieces move to the right
  Rubik[RF_DOWN][8] = Rubik_Backup[RF_LEFT][2];
  Rubik[RF_DOWN][7] = Rubik_Backup[RF_LEFT][5];
  Rubik[RF_DOWN][6] = Rubik_Backup[RF_LEFT][8];
  //3 left pieces move down
  Rubik[RF_LEFT][2] = Rubik_Backup[RF_UP][0];
  Rubik[RF_LEFT][5] = Rubik_Backup[RF_UP][1];
  Rubik[RF_LEFT][8] = Rubik_Backup[RF_UP][2];
}

// Simulates a clockwise turn of the Front face (F).
void Cubo::RotateFrontCW () {
  MakeBackup();
  // --- Rotate stickers on the Front face itself ---
  //Corners of the face
  Rubik[RF_FRONT][0] = Rubik_Backup[RF_FRONT][2];
  Rubik[RF_FRONT][2] = Rubik_Backup[RF_FRONT][8];
  Rubik[RF_FRONT][8] = Rubik_Backup[RF_FRONT][6];
  Rubik[RF_FRONT][6] = Rubik_Backup[RF_FRONT][0];
  //Edges of the face
  Rubik[RF_FRONT][1] = Rubik_Backup[RF_FRONT][5];
  Rubik[RF_FRONT][5] = Rubik_Backup[RF_FRONT][7];
  Rubik[RF_FRONT][7] = Rubik_Backup[RF_FRONT][3];
  Rubik[RF_FRONT][3] = Rubik_Backup[RF_FRONT][1];
  // --- Move adjacent stickers on surrounding faces ---
  //3 top pieces move to the right
  Rubik[RF_UP][0] = Rubik_Backup[RF_LEFT][2];
  Rubik[RF_UP][1] = Rubik_Backup[RF_LEFT][5];
  Rubik[RF_UP][2] = Rubik_Backup[RF_LEFT][8];
  //3 right pieces move down
  Rubik[RF_RIGHT][6] = Rubik_Backup[RF_UP][0];
  Rubik[RF_RIGHT][3] = Rubik_Backup[RF_UP][1];
  Rubik[RF_RIGHT][0] = Rubik_Backup[RF_UP][2];
  //3 bottom pieces move to the left
  Rubik[RF_DOWN][8] = Rubik_Backup[RF_RIGHT][6];
  Rubik[RF_DOWN][7] = Rubik_Backup[RF_RIGHT][3];
  Rubik[RF_DOWN][6] = Rubik_Backup[RF_RIGHT][0];
  //3 left pieces move up
  Rubik[RF_LEFT][2] = Rubik_Backup[RF_DOWN][8];
  Rubik[RF_LEFT][5] = Rubik_Backup[RF_DOWN][7];
  Rubik[RF_LEFT][8] = Rubik_Backup[RF_DOWN][6];
}


// Rotação da face da direita: -> Rotation of the right face:
//________________________________________________________________
// Simulates a clockwise turn of the Right face (R).
void Cubo::RotateRightCW () {
  MakeBackup();
  //Corners of the right
  Rubik[RF_RIGHT][0] = Rubik_Backup[RF_RIGHT][2];
  Rubik[RF_RIGHT][2] = Rubik_Backup[RF_RIGHT][8];
  Rubik[RF_RIGHT][8] = Rubik_Backup[RF_RIGHT][6];
  Rubik[RF_RIGHT][6] = Rubik_Backup[RF_RIGHT][0];
  //Edges of the right
  Rubik[RF_RIGHT][1] = Rubik_Backup[RF_RIGHT][5];
  Rubik[RF_RIGHT][5] = Rubik_Backup[RF_RIGHT][7];
  Rubik[RF_RIGHT][7] = Rubik_Backup[RF_RIGHT][3];
  Rubik[RF_RIGHT][3] = Rubik_Backup[RF_RIGHT][1];
  //3 top pieces move backwards.
  Rubik[RF_UP][2] = Rubik_Backup[RF_FRONT][2];
  Rubik[RF_UP][5] = Rubik_Backup[RF_FRONT][5];
  Rubik[RF_UP][8] = Rubik_Backup[RF_FRONT][8];
  //3 back pieces move down
  Rubik[RF_BACK][6] = Rubik_Backup[RF_UP][2];
  Rubik[RF_BACK][3] = Rubik_Backup[RF_UP][5];
  Rubik[RF_BACK][0] = Rubik_Backup[RF_UP][8];
  //3 bottom pieces move to the front
  Rubik[RF_DOWN][2] = Rubik_Backup[RF_BACK][6];
  Rubik[RF_DOWN][5] = Rubik_Backup[RF_BACK][3];
  Rubik[RF_DOWN][8] = Rubik_Backup[RF_BACK][0];
  //3 front pieces move up
  Rubik[RF_FRONT][2] = Rubik_Backup[RF_DOWN][2];
  Rubik[RF_FRONT][5] = Rubik_Backup[RF_DOWN][5];
  Rubik[RF_FRONT][8] = Rubik_Backup[RF_DOWN][8];
}

// Simulates a counter-clockwise turn of the Right face (R').
void Cubo::RotateRightCCW () {
  MakeBackup();
  //Corners of the right
  Rubik[RF_RIGHT][0] = Rubik_Backup[RF_RIGHT][6];
  Rubik[RF_RIGHT][2] = Rubik_Backup[RF_RIGHT][0];
  Rubik[RF_RIGHT][8] = Rubik_Backup[RF_RIGHT][2];
  Rubik[RF_RIGHT][6] = Rubik_Backup[RF_RIGHT][8];
  //Edges of the right
  Rubik[RF_RIGHT][1] = Rubik_Backup[RF_RIGHT][3];
  Rubik[RF_RIGHT][5] = Rubik_Backup[RF_RIGHT][1];
  Rubik[RF_RIGHT][7] = Rubik_Backup[RF_RIGHT][5];
  Rubik[RF_RIGHT][3] = Rubik_Backup[RF_RIGHT][7];
  //3 top pieces move to the front
  Rubik[RF_UP][8] = Rubik_Backup[RF_BACK][0];
  Rubik[RF_UP][5] = Rubik_Backup[RF_BACK][3];
  Rubik[RF_UP][2] = Rubik_Backup[RF_BACK][6];
  //3 front pieces move down
  Rubik[RF_FRONT][8] = Rubik_Backup[RF_UP][8];
  Rubik[RF_FRONT][5] = Rubik_Backup[RF_UP][5];
  Rubik[RF_FRONT][2] = Rubik_Backup[RF_UP][2];
  //3 bottom pieces move to the back
  Rubik[RF_DOWN][8] = Rubik_Backup[RF_FRONT][8];
  Rubik[RF_DOWN][5] = Rubik_Backup[RF_FRONT][5];
  Rubik[RF_DOWN][2] = Rubik_Backup[RF_FRONT][2];
  //3 back pieces move up
  Rubik[RF_BACK][0] = Rubik_Backup[RF_DOWN][8];
  Rubik[RF_BACK][3] = Rubik_Backup[RF_DOWN][5];
  Rubik[RF_BACK][6] = Rubik_Backup[RF_DOWN][2];
}


// Rotação da face de cima -> Rotation of the top face
//________________________________________________________________
// Simulates a clockwise turn of the Up face (U).
void Cubo::RotateUpCW () {
  MakeBackup();
  //Top corners
  Rubik[RF_UP][0] = Rubik_Backup[RF_UP][2];
  Rubik[RF_UP][2] = Rubik_Backup[RF_UP][8];
  Rubik[RF_UP][8] = Rubik_Backup[RF_UP][6];
  Rubik[RF_UP][6] = Rubik_Backup[RF_UP][0];
  //Top edges
  Rubik[RF_UP][1] = Rubik_Backup[RF_UP][5];
  Rubik[RF_UP][5] = Rubik_Backup[RF_UP][7];
  Rubik[RF_UP][7] = Rubik_Backup[RF_UP][3];
  Rubik[RF_UP][3] = Rubik_Backup[RF_UP][1];
  //3 back pieces move to the right
  Rubik[RF_BACK][8] = Rubik_Backup[RF_LEFT][8];
  Rubik[RF_BACK][7] = Rubik_Backup[RF_LEFT][7];
  Rubik[RF_BACK][6] = Rubik_Backup[RF_LEFT][6];
  //3 right pieces move to the front
  Rubik[RF_RIGHT][8] = Rubik_Backup[RF_BACK][8];
  Rubik[RF_RIGHT][7] = Rubik_Backup[RF_BACK][7];
  Rubik[RF_RIGHT][6] = Rubik_Backup[RF_BACK][6];
  //3 front pieces move to the left
  Rubik[RF_FRONT][8] = Rubik_Backup[RF_RIGHT][8];
  Rubik[RF_FRONT][7] = Rubik_Backup[RF_RIGHT][7];
  Rubik[RF_FRONT][6] = Rubik_Backup[RF_RIGHT][6];
  //3 left pieces move to the back
  Rubik[RF_LEFT][8] = Rubik_Backup[RF_FRONT][8];
  Rubik[RF_LEFT][7] = Rubik_Backup[RF_FRONT][7];
  Rubik[RF_LEFT][6] = Rubik_Backup[RF_FRONT][6];
}

// Simulates a counter-clockwise turn of the Up face (U').
void Cubo::RotateUpCCW () {
  MakeBackup();
  //Top corners
  Rubik[RF_UP][0] = Rubik_Backup[RF_UP][6];
  Rubik[RF_UP][2] = Rubik_Backup[RF_UP][0];
  Rubik[RF_UP][8] = Rubik_Backup[RF_UP][2];
  Rubik[RF_UP][6] = Rubik_Backup[RF_UP][8];
  //Top edges
  Rubik[RF_UP][1] = Rubik_Backup[RF_UP][3];
  Rubik[RF_UP][5] = Rubik_Backup[RF_UP][1];
  Rubik[RF_UP][7] = Rubik_Backup[RF_UP][5];
  Rubik[RF_UP][3] = Rubik_Backup[RF_UP][7];
  //3 back pieces move to the left
  Rubik[RF_BACK][6] = Rubik_Backup[RF_RIGHT][6];
  Rubik[RF_BACK][7] = Rubik_Backup[RF_RIGHT][7];
  Rubik[RF_BACK][8] = Rubik_Backup[RF_RIGHT][8];
  //3 left pieces move to the front
  Rubik[RF_LEFT][6] = Rubik_Backup[RF_BACK][6];
  Rubik[RF_LEFT][7] = Rubik_Backup[RF_BACK][7];
  Rubik[RF_LEFT][8] = Rubik_Backup[RF_BACK][8];
  //3 front pieces move to the right
  Rubik[RF_FRONT][6] = Rubik_Backup[RF_LEFT][6];
  Rubik[RF_FRONT][7] = Rubik_Backup[RF_LEFT][7];
  Rubik[RF_FRONT][8] = Rubik_Backup[RF_LEFT][8];
  //3 right pieces move to the back
  Rubik[RF_RIGHT][6] = Rubik_Backup[RF_FRONT][6];
  Rubik[RF_RIGHT][7] = Rubik_Backup[RF_FRONT][7];
  Rubik[RF_RIGHT][8] = Rubik_Backup[RF_FRONT][8];
}


// Rotação da face da esquerda -> Rotation of the left face
//________________________________________________________________
// Simulates a clockwise turn of the Left face (L).
void Cubo::RotateLeftCW () {
  MakeBackup();
  //Corners of the left
  Rubik[RF_LEFT][0] = Rubik_Backup[RF_LEFT][2];
  Rubik[RF_LEFT][2] = Rubik_Backup[RF_LEFT][8];
  Rubik[RF_LEFT][8] = Rubik_Backup[RF_LEFT][6];
  Rubik[RF_LEFT][6] = Rubik_Backup[RF_LEFT][0];
  //Edges of the left
  Rubik[RF_LEFT][3] = Rubik_Backup[RF_LEFT][1];
  Rubik[RF_LEFT][1] = Rubik_Backup[RF_LEFT][5];
  Rubik[RF_LEFT][5] = Rubik_Backup[RF_LEFT][7];
  Rubik[RF_LEFT][7] = Rubik_Backup[RF_LEFT][3];
  //3 top pieces move to the front
  Rubik[RF_UP][6] = Rubik_Backup[RF_BACK][2];
  Rubik[RF_UP][3] = Rubik_Backup[RF_BACK][5];
  Rubik[RF_UP][0] = Rubik_Backup[RF_BACK][8];
  //3 front pieces move down
  Rubik[RF_FRONT][6] = Rubik_Backup[RF_UP][6];
  Rubik[RF_FRONT][3] = Rubik_Backup[RF_UP][3];
  Rubik[RF_FRONT][0] = Rubik_Backup[RF_UP][0];
  //3 bottom pieces move to the back
  Rubik[RF_DOWN][6] = Rubik_Backup[RF_FRONT][6];
  Rubik[RF_DOWN][3] = Rubik_Backup[RF_FRONT][3];
  Rubik[RF_DOWN][0] = Rubik_Backup[RF_FRONT][0];
  //3 back pieces move up
  Rubik[RF_BACK][2] = Rubik_Backup[RF_DOWN][6];
  Rubik[RF_BACK][5] = Rubik_Backup[RF_DOWN][3];
  Rubik[RF_BACK][8] = Rubik_Backup[RF_DOWN][0];
}

// Simulates a counter-clockwise turn of the Left face (L').
void Cubo::RotateLeftCCW () {
  MakeBackup();
  //Corners of the left
  Rubik[RF_LEFT][0] = Rubik_Backup[RF_LEFT][6];
  Rubik[RF_LEFT][2] = Rubik_Backup[RF_LEFT][0];
  Rubik[RF_LEFT][8] = Rubik_Backup[RF_LEFT][2];
  Rubik[RF_LEFT][6] = Rubik_Backup[RF_LEFT][8];
  //Edges of the left
  Rubik[RF_LEFT][1] = Rubik_Backup[RF_LEFT][3];
  Rubik[RF_LEFT][5] = Rubik_Backup[RF_LEFT][1];
  Rubik[RF_LEFT][7] = Rubik_Backup[RF_LEFT][5];
  Rubik[RF_LEFT][3] = Rubik_Backup[RF_LEFT][7];
  //3 top pieces move to the back
  Rubik[RF_UP][0] = Rubik_Backup[RF_FRONT][0];
  Rubik[RF_UP][3] = Rubik_Backup[RF_FRONT][3];
  Rubik[RF_UP][6] = Rubik_Backup[RF_FRONT][6];
  //3 back pieces move down
  Rubik[RF_BACK][8] = Rubik_Backup[RF_UP][0];
  Rubik[RF_BACK][5] = Rubik_Backup[RF_UP][3];
  Rubik[RF_BACK][2] = Rubik_Backup[RF_UP][6];
  //3 bottom pieces move to the front
  Rubik[RF_DOWN][0] = Rubik_Backup[RF_BACK][8];
  Rubik[RF_DOWN][3] = Rubik_Backup[RF_BACK][5];
  Rubik[RF_DOWN][6] = Rubik_Backup[RF_BACK][2];
  //3 front pieces move up
  Rubik[RF_FRONT][0] = Rubik_Backup[RF_DOWN][0];
  Rubik[RF_FRONT][3] = Rubik_Backup[RF_DOWN][3];
  Rubik[RF_FRONT][6] = Rubik_Backup[RF_DOWN][6];
}


// Rotação da face de baixo -> Rotation of the bottom face
//________________________________________________________________
// Simulates a clockwise turn of the Down face (D).
void Cubo::RotateDownCW () {
  MakeBackup();
  //Bottom corners
  Rubik[RF_DOWN][6] = Rubik_Backup[RF_DOWN][0];
  Rubik[RF_DOWN][8] = Rubik_Backup[RF_DOWN][6];
  Rubik[RF_DOWN][2] = Rubik_Backup[RF_DOWN][8];
  Rubik[RF_DOWN][0] = Rubik_Backup[RF_DOWN][2];
  //Bottom edges
  Rubik[RF_DOWN][7] = Rubik_Backup[RF_DOWN][3];
  Rubik[RF_DOWN][5] = Rubik_Backup[RF_DOWN][7];
  Rubik[RF_DOWN][1] = Rubik_Backup[RF_DOWN][5];
  Rubik[RF_DOWN][3] = Rubik_Backup[RF_DOWN][1];
  //3 front pieces move to the right
  Rubik[RF_FRONT][0] = Rubik_Backup[RF_LEFT][0];
  Rubik[RF_FRONT][1] = Rubik_Backup[RF_LEFT][1];
  Rubik[RF_FRONT][2] = Rubik_Backup[RF_LEFT][2];
  //3 right pieces move to the back
  Rubik[RF_RIGHT][0] = Rubik_Backup[RF_FRONT][0];
  Rubik[RF_RIGHT][1] = Rubik_Backup[RF_FRONT][1];
  Rubik[RF_RIGHT][2] = Rubik_Backup[RF_FRONT][2];
  //3 back pieces move to the left
  Rubik[RF_BACK][0] = Rubik_Backup[RF_RIGHT][0];
  Rubik[RF_BACK][1] = Rubik_Backup[RF_RIGHT][1];
  Rubik[RF_BACK][2] = Rubik_Backup[RF_RIGHT][2];
  //3 left pieces move to the front
  Rubik[RF_LEFT][0] = Rubik_Backup[RF_BACK][0];
  Rubik[RF_LEFT][1] = Rubik_Backup[RF_BACK][1];
  Rubik[RF_LEFT][2] = Rubik_Backup[RF_BACK][2];
}

// Simulates a counter-clockwise turn of the Down face (D').
void Cubo::RotateDownCCW () {
  MakeBackup();
  //Bottom corners
  Rubik[RF_DOWN][6] = Rubik_Backup[RF_DOWN][8];
  Rubik[RF_DOWN][0] = Rubik_Backup[RF_DOWN][6];
  Rubik[RF_DOWN][2] = Rubik_Backup[RF_DOWN][0];
  Rubik[RF_DOWN][8] = Rubik_Backup[RF_DOWN][2];
  //Bottom edges
  Rubik[RF_DOWN][7] = Rubik_Backup[RF_DOWN][5];
  Rubik[RF_DOWN][3] = Rubik_Backup[RF_DOWN][7];
  Rubik[RF_DOWN][1] = Rubik_Backup[RF_DOWN][3];
  Rubik[RF_DOWN][5] = Rubik_Backup[RF_DOWN][1];
  //3 front pieces move to the left
  Rubik[RF_FRONT][2] = Rubik_Backup[RF_RIGHT][2];
  Rubik[RF_FRONT][1] = Rubik_Backup[RF_RIGHT][1];
  Rubik[RF_FRONT][0] = Rubik_Backup[RF_RIGHT][0];
  //3 left pieces move to the back
  Rubik[RF_LEFT][2] = Rubik_Backup[RF_FRONT][2];
  Rubik[RF_LEFT][1] = Rubik_Backup[RF_FRONT][1];
  Rubik[RF_LEFT][0] = Rubik_Backup[RF_FRONT][0];
  //3 back pieces move to the right
  Rubik[RF_BACK][2] = Rubik_Backup[RF_LEFT][2];
  Rubik[RF_BACK][1] = Rubik_Backup[RF_LEFT][1];
  Rubik[RF_BACK][0] = Rubik_Backup[RF_LEFT][0];
  //3 right pieces move to the front
  Rubik[RF_RIGHT][2] = Rubik_Backup[RF_BACK][2];
  Rubik[RF_RIGHT][1] = Rubik_Backup[RF_BACK][1];
  Rubik[RF_RIGHT][0] = Rubik_Backup[RF_BACK][0];
}

// Rotação da face de trás -> Rotation of the back face
//________________________________________________________________
// Simulates a clockwise turn of the Back face (B).
void Cubo::RotateBackCW () {
  MakeBackup();
  //Back corners
  Rubik[RF_BACK][0] = Rubik_Backup[RF_BACK][2];
  Rubik[RF_BACK][6] = Rubik_Backup[RF_BACK][0];
  Rubik[RF_BACK][8] = Rubik_Backup[RF_BACK][6];
  Rubik[RF_BACK][2] = Rubik_Backup[RF_BACK][8];
  //Back edges
  Rubik[RF_BACK][1] = Rubik_Backup[RF_BACK][5];
  Rubik[RF_BACK][3] = Rubik_Backup[RF_BACK][1];
  Rubik[RF_BACK][7] = Rubik_Backup[RF_BACK][3];
  Rubik[RF_BACK][5] = Rubik_Backup[RF_BACK][7];
  //3 top pieces move to the right
  Rubik[RF_UP][8] = Rubik_Backup[RF_RIGHT][2];
  Rubik[RF_UP][7] = Rubik_Backup[RF_RIGHT][5];
  Rubik[RF_UP][6] = Rubik_Backup[RF_RIGHT][8];
  //3 right pieces move down
  Rubik[RF_LEFT][6] = Rubik_Backup[RF_UP][8];
  Rubik[RF_LEFT][3] = Rubik_Backup[RF_UP][7];
  Rubik[RF_LEFT][0] = Rubik_Backup[RF_UP][6];
  //3 bottom pieces move to the left
  Rubik[RF_DOWN][0] = Rubik_Backup[RF_LEFT][6];
  Rubik[RF_DOWN][1] = Rubik_Backup[RF_LEFT][3];
  Rubik[RF_DOWN][2] = Rubik_Backup[RF_LEFT][0];
  //3 left pieces move up
  Rubik[RF_RIGHT][2] = Rubik_Backup[RF_DOWN][0];
  Rubik[RF_RIGHT][5] = Rubik_Backup[RF_DOWN][1];
  Rubik[RF_RIGHT][8] = Rubik_Backup[RF_DOWN][2];
}

// Simulates a counter-clockwise turn of the Back face (B').
void Cubo::RotateBackCCW () {
  MakeBackup();
  //Back corners
  Rubik[RF_BACK][0] = Rubik_Backup[RF_BACK][6];
  Rubik[RF_BACK][2] = Rubik_Backup[RF_BACK][0];
  Rubik[RF_BACK][8] = Rubik_Backup[RF_BACK][2];
  Rubik[RF_BACK][6] = Rubik_Backup[RF_BACK][8];
  //Back edges
  Rubik[RF_BACK][1] = Rubik_Backup[RF_BACK][3];
  Rubik[RF_BACK][5] = Rubik_Backup[RF_BACK][1];
  Rubik[RF_BACK][7] = Rubik_Backup[RF_BACK][5];
  Rubik[RF_BACK][3] = Rubik_Backup[RF_BACK][7];
  //3 top pieces move to the left
  Rubik[RF_UP][6] = Rubik_Backup[RF_LEFT][0];
  Rubik[RF_UP][7] = Rubik_Backup[RF_LEFT][3];
  Rubik[RF_UP][8] = Rubik_Backup[RF_LEFT][6];
  //3 left pieces move down
  Rubik[RF_RIGHT][8] = Rubik_Backup[RF_UP][6];
  Rubik[RF_RIGHT][5] = Rubik_Backup[RF_UP][7];
  Rubik[RF_RIGHT][2] = Rubik_Backup[RF_UP][8];
  //3 bottom pieces move to the right
  Rubik[RF_DOWN][2] = Rubik_Backup[RF_RIGHT][8];
  Rubik[RF_DOWN][1] = Rubik_Backup[RF_RIGHT][5];
  Rubik[RF_DOWN][0] = Rubik_Backup[RF_RIGHT][2];
  //3 right pieces move up
  Rubik[RF_LEFT][0] = Rubik_Backup[RF_DOWN][2];
  Rubik[RF_LEFT][3] = Rubik_Backup[RF_DOWN][1];
  Rubik[RF_LEFT][6] = Rubik_Backup[RF_DOWN][0];
}

// Simulates a whole-cube rotation around the Y-axis (the axis through the Up and Down faces).
// This is equivalent to turning the cube in your hands so that the Front face becomes the Right face.
void Cubo::RotateY() {
  // A Y rotation can be simulated by doing a U' turn, a D turn, and then swapping the middle layer pieces.
  RotateUpCCW ();
  RotateDownCW ();
  MakeBackup();
  //Move the entire 2nd layer of the cube
  // The Front middle-layer stickers move to the Left face's middle layer.
  Rubik[RF_FRONT][5] = Rubik_Backup[RF_LEFT][5];
  Rubik[RF_FRONT][4] = Rubik_Backup[RF_LEFT][4];
  Rubik[RF_FRONT][3] = Rubik_Backup[RF_LEFT][3];
  // The Left middle-layer stickers move to the Back face's middle layer.
  Rubik[RF_LEFT][5] = Rubik_Backup[RF_BACK][5];
  Rubik[RF_LEFT][4] = Rubik_Backup[RF_BACK][4];
  Rubik[RF_LEFT][3] = Rubik_Backup[RF_BACK][3];
  // The Back middle-layer stickers move to the Right face's middle layer.
  Rubik[RF_BACK][5] = Rubik_Backup[RF_RIGHT][5];
  Rubik[RF_BACK][4] = Rubik_Backup[RF_RIGHT][4];
  Rubik[RF_BACK][3] = Rubik_Backup[RF_RIGHT][3];
  // The Right middle-layer stickers move to the Front face's middle layer.
  Rubik[RF_RIGHT][5] = Rubik_Backup[RF_FRONT][5];
  Rubik[RF_RIGHT][4] = Rubik_Backup[RF_FRONT][4];
  Rubik[RF_RIGHT][3] = Rubik_Backup[RF_FRONT][3];
}


