/* PROGRAMMING FUNDAMENTAL'S PROJECT FOR FALL 2022 BS(CS)
 * You don't need to change anything in the driver code/main function except the part(s) where instructed.
 * You are expected to define your functionalities in the "functionality.h" header and call them here in the instructed part(s).
 * The game is started with a small box, you need to create other possible in "pieces.h" file in form of array.
 * E.g., Syntax for 2 pieces, each having four coordinates -> int Pieces[2][4];
 * Load the images into the textures objects from img directory.
 * Don't make any changes to the "utils.h" header file except for changing the coordinate of screen or global variables of the game.
 * SUGGESTION: If you want to make changes in "utils.h", do it once you complete rest of the functionalities.
 * TIP: You can study SFML for drawing scenes, sprites and window.
 * */

#include <SFML/Graphics.hpp>
#include <time.h>
#include <string>
#include "utils.h"
#include "pieces.h"
#include "functionality.h"
using namespace sf;

int main(){
    readHighScores(); // Load scores from file

    srand(time(0));
    // SFML 3.0 FIX: Added braces {} around the size
    RenderWindow window(VideoMode({320, 480}), title);
    
    // 1. Declare ALL Textures (Including objMenu)
    Texture obj1, obj2, obj3, objMenu; 

    // 2. Load Images (Check for errors)
    if (!obj1.loadFromFile("img/tiles.png")) return -1;
    if (!obj2.loadFromFile("img/background.png")) return -1;
    if (!obj3.loadFromFile("img/frame.png")) return -1;
    
    // 3. Load Menu Image (Optional)
    bool hasMenuImage = objMenu.loadFromFile("img/menu.png"); 

    // 4. Create Sprites (FIX: We initialize menuSprite with 'obj2' immediately)
    Sprite sprite(obj1), background(obj2), frame(obj3), menuSprite(obj2);

    // 5. Configure Menu Sprite
    if (hasMenuImage) {
        menuSprite.setTexture(objMenu); // If we have a menu image, switch to it
    } else {
        // If no menu image, we keep using obj2 (background) but make it blue
        menuSprite.setColor(Color(100, 100, 255)); 
    }

    int delta_x=0;
    float timer=0, delay=0.3;
    bool rotate=0;
    bool paused = false;
    bool inMenu = true; 
    Clock clock;
    
    // --- INITIALIZE THE FIRST PIECE ---
    int n = rand() % 4;
    colorNum = n + 1; 
    for (int i=0; i<4; i++){
        point_1[i][0] = BLOCKS[n][i] % 2;
        point_1[i][1] = BLOCKS[n][i] / 2;
    }

    // INITIALIZE THE "NEXT" PIECE ---
    nextBlockIndex = rand() % 4;
    nextColorNum = nextBlockIndex + 1;

    // --- VARIABLES FOR MENUS ---
    int gamePhase = 0; // 0=Menu, 1=Game, 2=Pause, 3=Help, 4=HighScores
    Font font;
    if (!font.openFromFile("font.ttf")) {
        // If font fails, we will just output to console, but try to get a font.ttf!
    }
    Text menuText(font); 
    menuText.setCharacterSize(20);
    menuText.setFillColor(Color::White);
    menuText.setPosition({50, 50});

    while (window.isOpen()){
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer+=time;

        //---Event Listening Part---//
        while (const std::optional event = window.pollEvent()){
            
            if (event->is<Event::Closed>())
                window.close();
            
            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                
                // --- PHASE 0: MAIN MENU ---
                if (gamePhase == 0) {
                    if (keyPressed->code == Keyboard::Key::Num1) {
                        gamePhase = 1; // Start Game
                        score = 0;     // Reset Score
                        gameRunning = true;
                        // Reset Grid
                        for (int i=0; i<M; i++) for (int j=0; j<N; j++) gameGrid[i][j] = 0;
                    }
                    else if (keyPressed->code == Keyboard::Key::Num2) {
                        gamePhase = 4; // High Scores (Placeholder)
                    }
                    else if (keyPressed->code == Keyboard::Key::Num3) {
                        gamePhase = 3; // Help
                    }
                    else if (keyPressed->code == Keyboard::Key::Num4) {
                        window.close(); // Exit
                    }
                }

                // --- PHASE 1: GAMEPLAY ---
                else if (gamePhase == 1) {
                    if (gameRunning) {
                        if (keyPressed->code == Keyboard::Key::Up) rotate = true;
                        else if (keyPressed->code == Keyboard::Key::Left) delta_x = -1;
                        else if (keyPressed->code == Keyboard::Key::Right) delta_x = 1;
                        else if (keyPressed->code == Keyboard::Key::Space) {
                            hardDrop();
                            timer = delay + 10;
                        }
                        else if (keyPressed->code == Keyboard::Key::P) {
                            gamePhase = 2; // Go to Pause Menu
                        }
                    }
                    // Restart if Game Over
                    if (keyPressed->code == Keyboard::Key::R) {
                        gamePhase = 0; // Go back to Main Menu
                    }
                }

                // --- PHASE 2: PAUSE MENU ---
                else if (gamePhase == 2) {
                    if (keyPressed->code == Keyboard::Key::Num1) { // New Game
                        gamePhase = 1;
                        score = 0;
                        gameRunning = true;
                        for (int i=0; i<M; i++) for (int j=0; j<N; j++) gameGrid[i][j] = 0;
                    }
                    else if (keyPressed->code == Keyboard::Key::Num2) gamePhase = 4; // High Scores
                    else if (keyPressed->code == Keyboard::Key::Num3) gamePhase = 3; // Help
                    else if (keyPressed->code == Keyboard::Key::Num4) window.close(); // Exit
                    else if (keyPressed->code == Keyboard::Key::Num5) gamePhase = 1; // Continue
                }

                // --- PHASE 3 & 4: HELP / HIGH SCORES ---
                else if (gamePhase == 3 || gamePhase == 4) {
                    // Press any key to go back to Menu
                    gamePhase = 0;
                }
            }
        }

        // --- LOGIC UPDATES ---
        if (gamePhase == 1 && gameRunning) {
            // Level Difficulty Logic
            float currentSpeed = 0.3;
            if (score > 50) currentSpeed = 0.25;
            if (score > 300) currentSpeed = 0.15; // Level 2 speed
            
            if (Keyboard::isKeyPressed(Keyboard::Key::Down)) delay = 0.05;
            else delay = currentSpeed;
           
            // --- SHRINKING BOARD LOGIC ---
            static float shrinkTimer = 0; // Static keeps value between frames
            shrinkTimer += time;          // Add frame time
            
            // If 5 minutes (300 seconds) have passed
            if (shrinkTimer > 300.0f) { 
                shrinkBoard(); // Push everything up!
                shrinkTimer = 0; // Reset timer for the next 5 minutes
            }
            

            fallingPiece(timer, delay);
            manualMovement(delta_x);
            rotation(rotate);
            if (!gameRunning) {
                 updateHighScore(score); 
            }
            
        }

        // --- DRAWING SECTION ---  
        window.clear(Color::Black);

        // 0. MAIN MENU
        if (gamePhase == 0) {
            window.draw(menuSprite); 
            // Draw text instructions if your menu image doesn't have them
            menuText.setString("1. Start\n2. High Scores\n3. Help\n4. Exit");
            menuText.setPosition({80, 200});
            window.draw(menuText);
            window.setTitle("Main Menu");
        }

        // 1. GAME LOOP
        else if (gamePhase == 1) {
            window.draw(background);
            
            // Draw Grid
            for (int i=0; i<M; i++){
                for (int j=0; j<N; j++){
                    if (gameGrid[i][j]==0) continue;
                    sprite.setTextureRect(IntRect({gameGrid[i][j]*18, 0}, {18, 18}));
                    sprite.setPosition({(float)j*18, (float)i*18});
                    sprite.move({28, 31});
                    window.draw(sprite);
                }
            }
            
            // Draw Shadow
             if (gameRunning) {
                int shadowCoordinates[4][2];
                getShadow(shadowCoordinates); 
                Sprite shadowSprite = sprite; 
                shadowSprite.setColor(Color(255, 255, 255, 100)); 
                for (int i=0; i<4; i++){
                    shadowSprite.setTextureRect(IntRect({colorNum*18, 0}, {18, 18})); 
                    shadowSprite.setPosition({(float)shadowCoordinates[i][0]*18, (float)shadowCoordinates[i][1]*18});
                    shadowSprite.move({28, 31}); 
                    window.draw(shadowSprite);
                }
            }

            // Draw Active Piece
            for (int i=0; i<4; i++){
                sprite.setTextureRect(IntRect({colorNum*18, 0}, {18, 18}));
                sprite.setPosition({(float)point_1[i][0]*18, (float)point_1[i][1]*18});
                sprite.move({28, 31});
                window.draw(sprite);
            }

            // Draw Next Piece
            Sprite nextSprite = sprite; 
            nextSprite.setTextureRect(IntRect({nextColorNum*18, 0}, {18, 18})); 
            for (int i=0; i<4; i++){
                int localX = BLOCKS[nextBlockIndex][i] % 2;
                int localY = BLOCKS[nextBlockIndex][i] / 2;
                nextSprite.setPosition({(float)(localX * 18) + 250, (float)(localY * 18) + 120});
                window.draw(nextSprite);
            }
            // --- GAME OVER OVERLAY ---
            if (!gameRunning) {
                // Dim the background slightly (Optional visual effect)
                RectangleShape dimBox(Vector2f(320, 480));
                dimBox.setFillColor(Color(0, 0, 0, 150)); // Semi-transparent black
                window.draw(dimBox);

                // Re-use your menuText variable to show the message
                menuText.setString("GAME OVER\n\nScore: " + std::to_string(score) + "\n\nPress R for Menu");
                menuText.setCharacterSize(30);
                menuText.setFillColor(Color::Red);
                menuText.setPosition({40, 150}); // Center on screen
                window.draw(menuText);
            }
            
            
            window.draw(frame);
            window.setTitle("Score: " + std::to_string(score));
        }

        // 2. PAUSE MENU
        else if (gamePhase == 2) {
            window.clear(Color(50, 50, 150)); // Blue background for pause
            menuText.setString("PAUSED\n\n1. New Game\n2. High Scores\n3. Help\n4. Exit\n5. Continue");
            menuText.setPosition({50, 100});
            window.draw(menuText);
            window.setTitle("Paused");
        }

        // 3. HELP SCREEN
        else if (gamePhase == 3) {
            window.clear(Color(0, 50, 0)); // Dark green
            menuText.setString("RULES:\n- Arrows to Move/Rotate\n- Space to Drop\n- Fill lines to score\n\nPress Any Key to Return");
            menuText.setPosition({20, 100});
            window.draw(menuText);
            window.setTitle("Help");
        }

        // 4. HIGH SCORES
        else if (gamePhase == 4) {
             window.clear(Color(50, 0, 0)); // Dark Red
             
             std::string scoreText = "HIGH SCORES:\n\n";
             for (int i=0; i<10; i++) {
                 scoreText += std::to_string(i+1) + ". " + std::to_string(highScores[i]) + "\n";
             }
             scoreText += "\nPress Any Key to Return";

             menuText.setString(scoreText);
             menuText.setPosition({50, 50});
             window.draw(menuText);
             window.setTitle("High Scores");
        }

        window.display();
    }
    return 0;
}