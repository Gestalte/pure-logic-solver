#include "raylib.h"
#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Pure Logic Solver");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    char gateNames[6][19] = {"resources/AND.png", "resources/NAND.png", "resources/NOR.png", "resources/OR.png", "resources/XNOR.png", "resources/XOR.png"};

    Texture2D gateTextures[6];

    for (int i = 0; i < 6; ++i)
    {
        Image img = LoadImage(gateNames[i]);
        gateTextures[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        // RAYWHITE
        ClearBackground(BACKGROUNDBLUE);

        for (int i = 0; i < 6; ++i)
        {
            int x = 0;
            int y = i * 50;
            DrawTexture(gateTextures[i], x, y, WHITE);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    for (int i = 0; i < 6; ++i)
    {
        UnloadTexture(gateTextures[i]);
    }

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
