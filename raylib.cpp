#include "raylib.h"

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - simple raylib example for cpp");
    SetTargetFPS(60); // Set desired framerate

    // Ball variables
    float ballPosX = (float)screenWidth / 2.0f;
    float ballPosY = (float)screenHeight / 2.0f;
    float ballSpeedX = 5.0f;
    float ballSpeedY = 4.0f;
    float ballRadius = 20.0f;

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        ballPosX += ballSpeedX;
        ballPosY += ballSpeedY;

        // Check for collisions with screen boundaries
        if ((ballPosX >= (screenWidth - ballRadius)) || (ballPosX <= ballRadius)) ballSpeedX *= -1.0f;
        if ((ballPosY >= (screenHeight - ballRadius)) || (ballPosY <= ballRadius)) ballSpeedY *= -1.0n:f;

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE); // Clear the background with a color

            DrawText("Simple raylib C++ Example", 10, 10, 20, DARKGRAY);
            DrawCircle((int)ballPosX, (int)ballPosY, ballRadius, BLUE); // Draw the bouncing ball

            DrawFPS(10, 420); // Display the current frames per second

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and unload resources
    //--------------------------------------------------------------------------------------

    return 0;
}

