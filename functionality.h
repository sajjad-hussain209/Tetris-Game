/* PROGRAMMING FUNDAMENTAL'S PROJECT FOR FALL 2022 BS(CS)
 * You need to define the required function in the part instructed here below.
 * Avoid making any unnecessary changes, particularly the ones you don't understand.
 * The different pieces should be defined using an array - uncomment the following code once you are done creating the array.
 * TIP: Understand thoroughly before getting started with coding.
 * */

// --- 1. HELPER FUNCTIONS (MUST BE AT THE TOP) ---

#include <fstream>  // For file handling
#include <iostream> // For input/output
#include <algorithm> // For sorting

bool anamoly(){
    for (int i=0; i<4; i++)
        if (point_1[i][0] < 0 || point_1[i][0] >= N || point_1[i][1] >= M || gameGrid[point_1[i][1]][point_1[i][0]])
            return 0;
    return 1;
}

// Checks for full lines and removes them
void checkLine(){
    int k = M-1;
    int linesClearedThisTurn = 0; // Counter for simultaneous lines

    for (int i = M-1; i > 0; i--)
    {
        int count = 0;
        for (int j = 0; j < N; j++)
        {
            if (gameGrid[i][j] != 0)
                count++;
            
            gameGrid[k][j] = gameGrid[i][j];
        }
        if (count < N) 
            k--;
        else{
            
            linesClearedThisTurn++; 
            
        }
    }

    
    if (linesClearedThisTurn > 0) {
        int points = 0;
        switch (linesClearedThisTurn) {
            case 1: points = 10; break;
            case 2: points = 30; break;
            case 3: points = 60; break;
            case 4: points = 100; break;
        }
        score += points * level; 
    }
}
// Calculates where the shadow should be
void getShadow(int shadowCoordinates[4][2]){
    for(int i=0; i<4; i++){
        shadowCoordinates[i][0] = point_1[i][0];
        shadowCoordinates[i][1] = point_1[i][1];
    }
    while(true){
        bool collision = false;
        for(int i=0; i<4; i++){
            if (shadowCoordinates[i][1] + 1 >= M || gameGrid[shadowCoordinates[i][1] + 1][shadowCoordinates[i][0]])
                collision = true;
        }
        if(collision) break;
        for(int i=0; i<4; i++)
            shadowCoordinates[i][1]++;
    }
}

// --- NEW FUNCTION: INSTANT DROP ---
void hardDrop(){
    while(true){
        // Move down by 1
        for (int i=0; i<4; i++) 
            point_1[i][1] += 1;

        // If we hit something
        if (!anamoly()){ // (Use !referee() if you renamed it)
            // Move back up by 1 and stop
            for (int i=0; i<4; i++) 
                point_1[i][1] -= 1;
            break;
        }
    }
}

// --- 2. MAIN FUNCTIONS (DEPEND ON THE HELPERS ABOVE) ---

void fallingPiece(float& timer, float& delay){
    if (timer > delay){
        for (int i=0; i<4; i++){
            point_2[i][0] = point_1[i][0];
            point_2[i][1] = point_1[i][1];
            point_1[i][1] += 1; // Move down
        }

        if (!anamoly()){ 
            // 1. Undo Move
            for (int i=0; i<4; i++)
                point_1[i][1] = point_2[i][1]; 
            
            // 2. Lock Piece
            for (int i=0; i<4; i++)
                gameGrid[point_1[i][1]][point_1[i][0]] = colorNum;

            checkLine(); 

            // 3. Spawn New Piece
            
            // A. The "Next" piece becomes the "Current" piece
            // We declare 'n' here so we can use it to set coordinates
            int n = nextBlockIndex; 
            colorNum = nextColorNum;
            
            // Set coordinates for the new Current Piece
            for (int i=0; i<4; i++){
                point_1[i][0] = BLOCKS[n][i] % 2;
                point_1[i][1] = BLOCKS[n][i] / 2;
                point_1[i][0] += 4; // Center it
            }

            // B. Generate the NEW Next Piece for the future
            if (score >= 300) level = 2; // Check if we need to level up
            
            int limit = (level == 1) ? 4 : 7; // Level 1 = 4 blocks, Level 2 = 7 blocks
            
            nextBlockIndex = rand() % limit;   // Store the result in the GLOBAL variable
            nextColorNum = nextBlockIndex + 1; // Update the next color

            // --- GAME OVER CHECK ---
            if (!anamoly()) {
                gameRunning = false; 
            }
        }
        timer = 0;
    }
}

void manualMovement(int& delta_x){
    if (delta_x != 0){
        for (int i=0; i<4; i++){
            point_2[i][0] = point_1[i][0];
            point_2[i][1] = point_1[i][1];
            point_1[i][0] += delta_x;
        }
        if (!anamoly()){ 
            for (int i=0; i<4; i++)
                point_1[i][0] = point_2[i][0];
        }
        delta_x = 0;
    }
}

void rotation(bool& rotate){
    if (rotate){
        int center_x = point_1[1][0];
        int center_y = point_1[1][1];
        
        // 1. Backup Current Position
        for (int i=0; i<4; i++){
             point_2[i][0] = point_1[i][0];
             point_2[i][1] = point_1[i][1];
        }

        // 2. Perform Rotation
        for (int i=0; i<4; i++){
            int x = point_1[i][1] - center_y;
            int y = point_1[i][0] - center_x;
            point_1[i][0] = center_x - x;
            point_1[i][1] = center_y + y;
        }
        
        // 3. Wall Kick Logic (The Fix)
        // If the rotation is invalid, try pushing it Left or Right to see if it fits.
        if (!anamoly()){ // Collision detected!
            
            // Try pushing RIGHT (Fixes Left Wall Stuck)
            bool fixed = false;
            for(int i=0; i<4; i++) point_1[i][0] += 1;
            if(anamoly()) fixed = true;
            
            // If pushing Right didn't work, Undo and try pushing LEFT
            if(!fixed){
                for(int i=0; i<4; i++) point_1[i][0] -= 1; // Undo Right shift
                
                // Try pushing LEFT (Fixes Right Wall Stuck)
                for(int i=0; i<4; i++) point_1[i][0] -= 1;
                if(anamoly()) fixed = true;
                else {
                     // If Left didn't work either, undo Left shift.
                     for(int i=0; i<4; i++) point_1[i][0] += 1;
                }
            }

            // If NEITHER worked (e.g., block is stuck between other blocks), Cancel Rotation
            if (!fixed){
                 for (int i=0; i<4; i++){
                     point_1[i][0] = point_2[i][0];
                     point_1[i][1] = point_2[i][1];
                 }
            }
        }
        rotate = false;
    }

}
// --- HIGH SCORE FUNCTIONS ---

void readHighScores() {
    std::ifstream file("highscores.txt");
    if (file.is_open()) {
        for (int i = 0; i < 10; i++) {
            file >> highScores[i];
        }
        file.close();
    } else {
        // If file doesn't exist, create it with zeros
        std::ofstream outfile("highscores.txt");
        for (int i = 0; i < 10; i++) outfile << 0 << "\n";
        outfile.close();
    }
}

void writeHighScores() {
    std::ofstream file("highscores.txt");
    if (file.is_open()) {
        for (int i = 0; i < 10; i++) {
            file << highScores[i] << "\n";
        }
        file.close();
    }
}

void updateHighScore(int currentScore) {
    std::cout << "CHECKING SCORE: " << currentScore << std::endl; // DEBUG PRINT

    // 1. Check if the current score is higher than the lowest high score
    if (currentScore > highScores[9]) {
        std::cout << "NEW HIGH SCORE! Updating list..." << std::endl; // DEBUG PRINT
        
        highScores[9] = currentScore; // Replace the lowest
        
        // 2. Sort the array (Descending Order)
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 9 - i; j++) {
                if (highScores[j] < highScores[j + 1]) {
                    int temp = highScores[j];
                    highScores[j] = highScores[j + 1];
                    highScores[j + 1] = temp;
                }
            }
        }
        
        // 3. Save to file
        writeHighScores();
    } else {
        std::cout << "Score too low to save." << std::endl; // DEBUG PRINT
    }
}

// --- SHRINKING BOARD MECHANIC ---
void shrinkBoard() {
    // 1. Check top row. If it has blocks, Game Over (since we are pushing up)
    for(int j=0; j<N; j++) {
        if (gameGrid[0][j] != 0) {
            gameRunning = false;
            return;
        }
    }

    // 2. Shift all rows UP by 1
    for (int i=0; i<M-1; i++) {
        for (int j=0; j<N; j++) {
            gameGrid[i][j] = gameGrid[i+1][j];
        }
    }

    // 3. Fill the bottom row with random "garbage" blocks
    for (int j=0; j<N; j++) {
        // Randomly pick a color (1-7), or use 0 for empty spots to make it easier
        if (rand() % 2 == 0) 
            gameGrid[M-1][j] = rand() % 7 + 1; 
        else 
            gameGrid[M-1][j] = 0;
    }
}