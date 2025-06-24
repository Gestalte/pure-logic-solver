#include "raylib.h"
#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define GATECOUNT 6

struct image2D
{
    Texture2D* texture;
    Rectangle rect;
};

// TODO: Maybe make a struct for straight input lines for leftmost inputs
struct gate
{
    struct image2D gate;
    struct image2D outputLine;
    struct image2D successIndicator;
    struct gate* parent;
    // Children are responsible for drawing input lines
    struct gate* childA;
    struct gate* childB;
    // inputLines should be used when the gate has no children.
    struct image2D inputLineA;
    struct image2D inputLineB;
};

static int clampInclusive(int value, int min, int max)
{
    return value <= min ? min : value >= max ? max : value;
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    const int screenWidth  = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Pure Logic Solver");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)

    // Images are 100 x 50 px
    char* gateNames[GATECOUNT] = {
        "resources/AND.png",
        "resources/NAND.png",
        "resources/NOR.png",
        "resources/OR.png",
        "resources/XNOR.png",
        "resources/XOR.png"};

    Texture2D gateTextures[GATECOUNT];

    // Images are 50 x 50 px
    char* lineNames[GATECOUNT] = {
        "resources/input_black.png",
        "resources/input_gray.png",
        "resources/input_white.png",
        "resources/output_black.png",
        "resources/output_gray.png",
        "resources/output_white.png",
    };

    Texture2D lineTextures[GATECOUNT];

    for (int i = 0; i < GATECOUNT; ++i)
    {
        Image img       = LoadImage(gateNames[i]);
        gateTextures[i] = LoadTextureFromImage(img);
        UnloadImage(img);

        Image line      = LoadImage(lineNames[i]);
        lineTextures[i] = LoadTextureFromImage(line);
        UnloadImage(line);
    }

    Rectangle gateMenuRect = {
        600.0f,
        70.0f,
        ((float)screenWidth / 2) - (gateMenuRect.width / 2),
        (float)screenHeight - (gateMenuRect.height + 10)};

    struct image2D gateMenuItems[GATECOUNT];

    for (int i = 0; i < GATECOUNT; i++)
    {
        struct image2D menuGate = {
            &gateTextures[i], {(gateMenuRect.x + (i * 100.0f)), (gateMenuRect.y + 10.0f), 100.0f, 50.0f}};

        gateMenuItems[i] = menuGate;
    }

    int gateIndex  = -1;
    int IsEditMode = 1;
    char levels    = 1;

    struct image2D gates[GATECOUNT];

    for (int i = 0; i < GATECOUNT; ++i)
    {
        gates[i].texture = &gateTextures[0];
    }

    Rectangle saveEditRect = {(float)screenWidth / 2 - 50, (float)screenHeight - (screenHeight - 20), 100.0f, 30.0f};
    Rectangle decrementLevelsRect = {
        saveEditRect.x + 100 + 10, (float)screenHeight - (screenHeight - 20), 50.0f, 30.0f};
    Rectangle incrementLevelsRect = {
        decrementLevelsRect.x + 110, (float)screenHeight - (screenHeight - 20), 50.0f, 30.0f};

    char* SaveEditButtonText = "Edit";

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        // Set positions gates should draw at.
        int startingX = (screenWidth / 2) - 50 * levels;
        for (int i = 0; i < GATECOUNT; ++i)
        {
            gates[i].rect = (Rectangle){startingX + (i * 100), ((float)screenHeight / 2) - 25.0f, 100.0f, 50.0f};
        }

        if (CheckCollisionPointRec(GetMousePosition(), saveEditRect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            IsEditMode = IsEditMode ? 0 : 1;
        }

        if (IsEditMode)
        {
            // Handle click for Increment and Decrement levels
            if (CheckCollisionPointRec(GetMousePosition(), incrementLevelsRect) &&
                IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                levels++;
                levels                    = clampInclusive(levels, 1, 6);
                gates[levels - 1].texture = &gateTextures[0];
            }
            if (CheckCollisionPointRec(GetMousePosition(), decrementLevelsRect) &&
                IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                levels--;
                levels = clampInclusive(levels, 1, 6);
            }

            // TODO: Add lines between gates
            // TODO: clickin on a line should toggle line state, gray, black, white.

            // handles click for gate menu items.
            for (int i = 0; i < GATECOUNT; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), gateMenuItems[i].rect) &&
                    IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    if (gateIndex != -1)
                    {
                        gates[gateIndex].texture = gateMenuItems[i].texture;
                        gateIndex                = -1;
                    }
                    break;
                }
            }

            // handle click for gates
            for (int i = 0; i < levels; ++i)
            {
                if (CheckCollisionPointRec(GetMousePosition(), gates[i].rect) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    gateIndex = i;
                    break;
                }
            }
        }
        else
        {
            // TODO: Lines can be gray, black or white.
            // TODO: Add an indicator icon for if the input and output of the gate is correct.
            // TODO: clickin on a line should toggle line state, gray, black, white lines set in edit mode should not be
            // able to change.
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BACKGROUNDBLUE);

        if (IsEditMode)
        {
            SaveEditButtonText = "Save";

            // Gate menu
            if (gateIndex != -1) // Only show gate menu when a gate is selected.
            {
                DrawRectangle(
                    (int)gateMenuRect.x,
                    (int)gateMenuRect.y,
                    (int)gateMenuRect.width,
                    (int)gateMenuRect.height,
                    RAYWHITE);
                for (int i = 0; i < GATECOUNT; ++i)
                {
                    DrawTexture(
                        *gateMenuItems[i].texture, (int)gateMenuRect.x + (i * 100), (int)gateMenuRect.y + 10, WHITE);
                }
            }

            // Decrement levels button
            DrawRectangle(
                (int)decrementLevelsRect.x,
                (int)decrementLevelsRect.y,
                (int)decrementLevelsRect.width,
                (int)decrementLevelsRect.height,
                LIGHTGRAY);
            DrawRectangleLines(
                (int)decrementLevelsRect.x,
                (int)decrementLevelsRect.y,
                (int)decrementLevelsRect.width,
                (int)decrementLevelsRect.height,
                DARKGRAY);
            DrawText(
                "-",
                (int)(decrementLevelsRect.x + (decrementLevelsRect.width / 2) - 4),
                (int)(decrementLevelsRect.y + (decrementLevelsRect.height / 4)),
                16,
                BLACK);

            char levelText[2] = {(char)(levels + 48), '\0'}; // convert int to string
            DrawText(
                levelText,
                (int)(decrementLevelsRect.x + 50 + (decrementLevelsRect.width / 2)),
                (int)(decrementLevelsRect.y + (decrementLevelsRect.height / 4)),
                16,
                BLACK);

            // Increment levels button
            DrawRectangle(
                (int)incrementLevelsRect.x,
                (int)incrementLevelsRect.y,
                (int)incrementLevelsRect.width,
                (int)incrementLevelsRect.height,
                LIGHTGRAY);
            DrawRectangleLines(
                (int)incrementLevelsRect.x,
                (int)incrementLevelsRect.y,
                (int)incrementLevelsRect.width,
                (int)incrementLevelsRect.height,
                DARKGRAY);
            DrawText(
                "+",
                (int)(incrementLevelsRect.x + (incrementLevelsRect.width / 2) - 4),
                (int)(incrementLevelsRect.y + (incrementLevelsRect.height / 4)),
                16,
                BLACK);
        }
        else
        {
            SaveEditButtonText = "Edit";
        }

        // Gates at screen center.
        for (int i = 0; i < levels; ++i)
        {
            DrawTexture(*gates[i].texture, (int)gates[i].rect.x, (int)gates[i].rect.y, WHITE);
        }

        // Save/Edit button
        DrawRectangle(
            (int)saveEditRect.x, (int)saveEditRect.y, (int)saveEditRect.width, (int)saveEditRect.height, LIGHTGRAY);
        DrawRectangleLines(
            (int)saveEditRect.x, (int)saveEditRect.y, (int)saveEditRect.width, (int)saveEditRect.height, DARKGRAY);
        DrawText(
            SaveEditButtonText,
            (int)(saveEditRect.x + (saveEditRect.width / 3)),
            (int)(saveEditRect.y + (saveEditRect.height / 4)),
            16,
            BLACK);

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
