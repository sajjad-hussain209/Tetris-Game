/* PROGRAMMING FUNDAMENTAL'S PROJECT FOR FALL 2022 BS(CS)
 * Shape of each piece is represented by rows in the array.
 * TIP: Name the array what is already been coded to avoid any unwanted errors.
 */

 // The 7 Tetris blocks: I, Z, S, T, L, J, O
int BLOCKS[7][4] = {
    {1, 3, 5, 7}, // I-Shape (Vertical Line)
    {2, 4, 5, 7}, // Z-Shape
    {3, 5, 4, 6}, // S-Shape
    {3, 5, 4, 7}, // T-Shape
    {2, 3, 5, 7}, // L-Shape
    {3, 5, 7, 6}, // J-Shape
    {2, 3, 4, 5}  // O-Shape (Square)
};