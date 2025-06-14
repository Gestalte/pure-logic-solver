#include "raylib.h"
#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define GATECOUNT 6

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
        toggleRecs[i] = (Rectangle){((float)rectX + (i * 100)), ((float)rectY + 10), 100.0f, 50.0f};
    }

    int gateIndex = 0;
    int IsEditMode = 1;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    Rectangle saveEditRect = {(float)screenWidth / 2 - 50, (float)screenHeight - (screenHeight - 20), 100.0f, 30.0f};
    char* SaveEditButtonText = "Edit";
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        if (CheckCollisionPointRec(GetMousePosition(), saveEditRect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            IsEditMode = IsEditMode ? 0 : 1;
        }

        if (IsEditMode)
        {
            // TODO: Add a button for switching to IsEditMode false.
            // TODO: Add two buttons and a text label that increase and decrease the number of gates.
            // TODO: clicking on a gate should open a menu where you can pick a different gate in the chosen gate's place.
            // TODO: Add lines between gates
            // TODO: clickin on a line should toggle line state, gray, black, white.

            for (int i = 0; i < GATECOUNT; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), toggleRecs[i]) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    gateIndex = i;
                    break;
                }
            }
        }
        else
        {
            // TODO: Keep showing gates, but don't check them for mouse clicks.
            // TODO: Lins can be gray, black or white.
            // TODO: Add an indicator icon for if the input and output of the gate is correct.
            // TODO: clickin on a line should toggle line state, gray, black, white lines set in edit mode should not be able to change.
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        // RAYWHITE
        ClearBackground(BACKGROUNDBLUE);

        if (IsEditMode)
        {
            SaveEditButtonText = "Save";

            DrawRectangle(rectX, rectY, rectWidth, rectHeight, RAYWHITE);

            for (int i = 0; i < GATECOUNT; ++i)
            {
                DrawTexture(gateTextures[i], rectX + (i * 100), rectY + 10, WHITE);
            }
        }
        else
        {
            SaveEditButtonText = "Edit";
        }

        DrawTexture(gateTextures[gateIndex], (screenWidth / 2) - 50, (screenHeight / 2) - 25, WHITE);

        DrawRectangle(saveEditRect.x, saveEditRect.y, saveEditRect.width, saveEditRect.height, LIGHTGRAY);
        DrawRectangleLines(saveEditRect.x, saveEditRect.y, saveEditRect.width, saveEditRect.height, DARKGRAY);
        DrawText(SaveEditButtonText, saveEditRect.x + (saveEditRect.width / 3), saveEditRect.y + (saveEditRect.height / 4), 16, BLACK);

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
