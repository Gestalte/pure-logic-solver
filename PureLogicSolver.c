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
        toggleRecs[i] = (Rectangle){((float)rectX + (i * 100.0f)), ((float)rectY + 10.0f), 100.0f, 50.0f};
    }

    // index of the gate selected in the edit gate menu thing.
    int gateIndex = 0;
    int IsEditMode = 1;
    char levels = 1;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    Rectangle saveEditRect = {(float)screenWidth / 2 - 50, (float)screenHeight - (screenHeight - 20), 100.0f, 30.0f};
    Rectangle decrementLevelsRect = {saveEditRect.x + 100 + 10, (float)screenHeight - (screenHeight - 20), 50.0f, 30.0f};
    Rectangle incrementLevelsRect = {decrementLevelsRect.x + 110, (float)screenHeight - (screenHeight - 20), 50.0f, 30.0f};

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
            // Handle click for Increment and Decrement levels
            if (CheckCollisionPointRec(GetMousePosition(), incrementLevelsRect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                levels++;
            }
            if (CheckCollisionPointRec(GetMousePosition(), decrementLevelsRect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                levels--;
            }

            // TODO: increase and decrease the number of gates based on the number of levels.
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
            // TODO: Lines can be gray, black or white.
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

            // Select gate box
            DrawRectangle(rectX, rectY, rectWidth, rectHeight, RAYWHITE);

            for (int i = 0; i < GATECOUNT; ++i)
            {
                DrawTexture(gateTextures[i], rectX + (i * 100), rectY + 10, WHITE);
            }

            // Decrement levels button
            DrawRectangle((int)decrementLevelsRect.x, (int)decrementLevelsRect.y, (int)decrementLevelsRect.width, (int)decrementLevelsRect.height, LIGHTGRAY);
            DrawRectangleLines((int)decrementLevelsRect.x, (int)decrementLevelsRect.y, (int)decrementLevelsRect.width, (int)decrementLevelsRect.height, DARKGRAY);
            DrawText("-", (int)(decrementLevelsRect.x + (decrementLevelsRect.width / 2) - 4), (int)(decrementLevelsRect.y + (decrementLevelsRect.height / 4)), 16, BLACK);

            // Number of levels text
            // clamp betwen 1 and 6
            if (levels < 1)
            {
                levels = 1;
            }
            if (levels > 6)
            {
                levels = 6;
            }
            char levelText[2] = {(char)(levels + 48), '\0'}; // convert int to string
            DrawText(levelText, (int)(decrementLevelsRect.x + 50 + (decrementLevelsRect.width / 2)), (int)(decrementLevelsRect.y + (decrementLevelsRect.height / 4)), 16, BLACK);

            // Increment levels button
            DrawRectangle((int)incrementLevelsRect.x, (int)incrementLevelsRect.y, (int)incrementLevelsRect.width, (int)incrementLevelsRect.height, LIGHTGRAY);
            DrawRectangleLines((int)incrementLevelsRect.x, (int)incrementLevelsRect.y, (int)incrementLevelsRect.width, (int)incrementLevelsRect.height, DARKGRAY);
            DrawText("+", (int)(incrementLevelsRect.x + (incrementLevelsRect.width / 2) - 4), (int)(incrementLevelsRect.y + (incrementLevelsRect.height / 4)), 16, BLACK);
        }
        else
        {
            SaveEditButtonText = "Edit";
        }

        DrawTexture(gateTextures[gateIndex], (screenWidth / 2) - 50, (screenHeight / 2) - 25, WHITE);

        DrawRectangle((int)saveEditRect.x, (int)saveEditRect.y, (int)saveEditRect.width, (int)saveEditRect.height, LIGHTGRAY);
        DrawRectangleLines((int)saveEditRect.x, (int)saveEditRect.y, (int)saveEditRect.width, (int)saveEditRect.height, DARKGRAY);
        DrawText(SaveEditButtonText, (int)(saveEditRect.x + (saveEditRect.width / 3)), (int)(saveEditRect.y + (saveEditRect.height / 4)), 16, BLACK);

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
