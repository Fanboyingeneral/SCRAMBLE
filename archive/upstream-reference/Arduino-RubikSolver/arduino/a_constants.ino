// This enumeration provides human-readable names for the faces of the cube.
// The compiler assigns integer values starting from 0 (RF_FRONT = 0, RF_UP = 1, etc.).
enum Rubik_Faces {
    RF_FRONT,   //0 - The face initially facing the user.
    RF_UP,      //1 - The top face.
    RF_LEFT,    //2 - The left face.
    RF_RIGHT,   //3 - The right face.
    RF_DOWN,    //4 - The bottom face.
    RF_BACK,    //5 - The back face.
    RF_TOTAL_FACES, //6 - A convenient constant representing the total number of faces.
};

//const char* Rubik_FaceName[] = {
//    "RF_FRONT ",
//    "RF_UP    ",
//    "RF_LEFT  ",
//    "RF_RIGHT ",
//    "RF_DOWN  ",
//    "RF_BACK  ",
//    "RF_TOTAL_FACES",6
//};

// This enumeration provides human-readable names for the colors of the stickers.
// (RC_RED = 0, RC_YELLOW = 1, etc.)
enum Rubik_Colors {
    RC_RED,
    RC_YELLOW,
    RC_BLUE,
    RC_GREEN,
    RC_WHITE,
    RC_ORANGE,
    RC_TOTAL_COLORS,
};

//const char* Rubik_ColorName[] = {
//    "RED",
//    "YELLOW",
//    "BLUE",
//    "GREEN",
//    "WHITE",
//    "ORANGE",
//    "???"
//};

// This is a Look-Up Table (LUT) that maps a face to its center color on a solved cube.
// For example, the Front face (index 0) is Red, the Up face (index 1) is Yellow, etc.
int Rubik_FaceToColorLUT[] = {
    RC_RED,    // Corresponds to RF_FRONT
    RC_YELLOW, // Corresponds to RF_UP
    RC_BLUE,   // Corresponds to RF_LEFT
    RC_GREEN,  // Corresponds to RF_RIGHT
    RC_WHITE,  // Corresponds to RF_DOWN
    RC_ORANGE, // Corresponds to RF_BACK
};


// This is the main data structure for the virtual cube.
// It's a 2D array where Rubik[face][sticker] stores the color of that sticker.
// 'unsigned char' is used as it's a small, efficient data type for storing the color values (0-5).
unsigned char Rubik[RF_TOTAL_FACES][9];
