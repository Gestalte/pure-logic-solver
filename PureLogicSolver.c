#include "raylib.h"
#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define GATECOUNT 6

int gateIndex = 0;

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

    // Images are 100 x 50 px
    char gateNames[GATECOUNT][19] = {"resources/AND.png", "resources/NAND.png", "resources/NOR.png", "resources/OR.png", "resources/XNOR.png", "resources/XOR.png"};

    Texture2D gateTextures[GATECOUNT];

    for (int i = 0; i < GATECOUNT; ++i)
    {
        Image img = LoadImage(gateNames[i]);
        gateTextures[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    int rectHeight = 70;
    int rectWidth = 600;
    int rectX = (screenWidth / 2) - (rectWidth / 2);
    int rectY = screenHeight - (rectHeight + 10);

    Rectangle toggleRecs[GATECOUNT] = {0};

    for (int i = 0; i < GATECOUNT; i++)
    {
        toggleRecs[i] = (Rectangle){(float)(rectX + (i * 100)), (float)(rectY + 10), 100.0f, 50.0f};
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

        for (int i = 0; i < GATECOUNT; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), toggleRecs[i]) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                gateIndex = i;
                break;
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        // RAYWHITE
        ClearBackground(BACKGROUNDBLUE);

        DrawRectangle(rectX, rectY, rectWidth, rectHeight, RAYWHITE);

        for (int i = 0; i < GATECOUNT; ++i)
        {
            DrawTexture(gateTextures[i], rectX + (i * 100), rectY + 10, WHITE);
        }

        DrawTexture(gateTextures[gateIndex], (screenWidth / 2) - 50, (screenHeight / 2) - 25, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    for (int i = 0; i < GATECOUNT; ++i)
    {
        UnloadTexture(gateTextures[i]);
    }

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
