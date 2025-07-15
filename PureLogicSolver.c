#include "raylib.h"
#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define GATECOUNT 6
#define LINECOUNT 3 // TODO: Update with real line count (6?)

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

struct ButtonData
{
    Rectangle rect;
    char* text;
};

static int clampInclusive(int value, int min, int max)
{
    return value <= min ? min : value >= max ? max : value;
}

void DrawButton(struct ButtonData* buttonData)
{
    DrawRectangle(
        (int)buttonData->rect.x,
        (int)buttonData->rect.y,
        (int)buttonData->rect.width,
        (int)buttonData->rect.height,
        LIGHTGRAY);
    DrawRectangleLines(
        (int)buttonData->rect.x,
        (int)buttonData->rect.y,
        (int)buttonData->rect.width,
        (int)buttonData->rect.height,
        DARKGRAY);
    DrawText(
        buttonData->text,
        (int)(buttonData->rect.x + (buttonData->rect.width / 3)),
        (int)(buttonData->rect.y + (buttonData->rect.height / 4)),
        16,
        BLACK);
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

    for (int i = 0; i < GATECOUNT; ++i)
    {
        Image img       = LoadImage(gateNames[i]);
        gateTextures[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    // TODO: Make output lines that go up or down and draw them on top of each other to get the -[ shape.
    // Images are 50 x 50 px
    char* lineNames[LINECOUNT] = {
        "resources/input_black.png",
        "resources/input_gray.png",
        "resources/input_white.png",
    };

    Texture2D lineTextures[LINECOUNT];

    for (int i = 0; i < LINECOUNT; i++)
    {
        Image line      = LoadImage(lineNames[i]);
        lineTextures[i] = LoadTextureFromImage(line);
        UnloadImage(line);
    }

    Rectangle gateMenuRect = {((float)screenWidth / 2) - 300.0f, (float)screenHeight - 80.0f, 600.0f, 70.0f};

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

    struct ButtonData saveEditButton = {
        {(float)screenWidth / 2 - 50, (float)screenHeight - (screenHeight - 20), 100.0f, 30.0f}, "Edit"};
    struct ButtonData decrementLevelsButton = {
        {saveEditButton.rect.x + 100 + 10, (float)screenHeight - (screenHeight - 20), 50.0f, 30.0f}, " -"};
    struct ButtonData incrementLevelsButton = {
        {decrementLevelsButton.rect.x + 110, (float)screenHeight - (screenHeight - 20), 50.0f, 30.0f}, " +"};

    struct gate gates[21] = {{&gateTextures[0], 0}};

    for (int i = 0; i < 21; i++)
    {
        struct image2D img = {&gateTextures[0], {0}};
        gates[i].gate      = img;
    }

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
            gates[i].gate.rect = (Rectangle){startingX + (i * 100), ((float)screenHeight / 2) - 25.0f, 100.0f, 50.0f};
        }

        if (CheckCollisionPointRec(GetMousePosition(), saveEditButton.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            IsEditMode = IsEditMode ? 0 : 1;
        }

        if (IsEditMode)
        {
            // Handle click for Increment and Decrement levels
            if (CheckCollisionPointRec(GetMousePosition(), incrementLevelsButton.rect) &&
                IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                int old = levels;
                levels  = clampInclusive(++levels, 1, 6);
                if (old != 6)
                {
                    gates[levels - 1].gate.texture = &gateTextures[0];
                }
            }
            if (CheckCollisionPointRec(GetMousePosition(), decrementLevelsButton.rect) &&
                IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                levels = clampInclusive(--levels, 1, 6);
            }

            // TODO: Add lines between gates
            // TODO: clicking on a line should toggle line state, gray, black, white.

            // handles click for gate menu items.
            for (int i = 0; i < GATECOUNT; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), gateMenuItems[i].rect) &&
                    IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    if (gateIndex != -1)
                    {
                        gates[gateIndex].gate.texture = gateMenuItems[i].texture;
                        gateIndex                     = -1;
                    }
                    break;
                }
            }

            // handle click for gates
            for (int i = 0; i < levels; ++i)
            {
                if (CheckCollisionPointRec(GetMousePosition(), gates[i].gate.rect) &&
                    IsMouseButtonDown(MOUSE_BUTTON_LEFT))
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
            saveEditButton.text = "Save";

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
            DrawButton(&decrementLevelsButton);

            // Levels count
            char levelText[2] = {(char)(levels + 48), '\0'}; // convert int to string
            DrawText(
                levelText,
                (int)(decrementLevelsButton.rect.x + 50 + (decrementLevelsButton.rect.width / 2)),
                (int)(decrementLevelsButton.rect.y + (decrementLevelsButton.rect.height / 4)),
                16,
                BLACK);

            // Increment levels button
            DrawButton(&incrementLevelsButton);
        }
        else
        {
            saveEditButton.text = "Edit";
        }

        // Gates at screen center.
        for (int i = 0; i < levels; ++i)
        {
            DrawTexture(*gates[i].gate.texture, (int)gates[i].gate.rect.x, (int)gates[i].gate.rect.y, WHITE);
        }

        // Save/Edit button
        DrawButton(&saveEditButton);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    for (int i = 0; i < GATECOUNT; ++i)
    {
        UnloadTexture(gateTextures[i]);
    }

    for (int i = 0; i < LINECOUNT; ++i)
    {
        UnloadTexture(lineTextures[i]);
    }

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
