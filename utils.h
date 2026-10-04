/* PROGRAMMING FUNDAMENTAL'S PROJECT FOR FALL 2022 BS(CS)
 * This header contains the global/common variables of the project.
 * You don't really need to change anything.
 * TIP: After completing your project, you can make changes for aesthetics.
 * */
  
//---Title: PF-Project, Fall-2022 for BS(CS)---//
#ifndef UTILS_H
#define UTILS_H

const char title[] = "PF-Project: Tetris";

//---Height and Width of the Actual Interactive Game---//
const int M = 20;   //Number of rows for a piece to cover on the screen (not the entire screen) = 20
const int N = 10;   //Number of columns for a piece to cover on the screen (not the entire screen) = 10

//---The Actual Interactive Game Grid - Built Over (MxN)---//
int gameGrid[M][N] = {0};

//---To Hold the Coordinates of the Piece---//
int point_1[4][2], point_2[4][2];

// for textures and randomize the game.
int colorNum = 1;    
int nextBlockIndex = 0;
int nextColorNum = 1;

// for level systems
int level = 1; 

// for calculating score
int score = 0;

// for ending the game
bool gameRunning = true;

int highScores[10] = {0}; // Stores top 10 scores

#endif