#include "raylib.h"
#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define GATECOUNT 6
#define LINECOUNT 15

struct image2D
{
    Texture2D* texture;
    Rectangle rect;
};

// TODO: Maybe make a struct for straight input lines for leftmost inputs
struct gate
{
    struct image2D gate;
    struct gate* parent;
    struct gate* leftChild;
    struct gate* rightChild;
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
        "resources/gates/AND.png",
        "resources/gates/NAND.png",
        "resources/gates/NOR.png",
        "resources/gates/OR.png",
        "resources/gates/XNOR.png",
        "resources/gates/XOR.png"};

    Texture2D gateTextures[GATECOUNT];

    for (int i = 0; i < GATECOUNT; ++i)
    {
        Image img       = LoadImage(gateNames[i]);
        gateTextures[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    char* lineNames[LINECOUNT] = {
        "resources/lines/up_white.png",
        "resources/lines/up_gray.png",
        "resources/lines/up_black.png",

        "resources/lines/straight_white.png",
        "resources/lines/straight_gray.png",
        "resources/lines/straight_black.png",

        "resources/lines/fork_white.png",
        "resources/lines/fork_gray.png",
        "resources/lines/fork_black.png",

        "resources/lines/dual_white.png",
        "resources/lines/dual_gray.png",
        "resources/lines/dual_black.png",

        "resources/lines/down_white.png",
        "resources/lines/down_gray.png",
        "resources/lines/down_black.png",
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

    int selectedGateIndex = -1;
    int IsEditMode        = 1;
    char levels           = 1;

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
                    if (selectedGateIndex != -1)
                    {
                        gates[selectedGateIndex].gate.texture = gateMenuItems[i].texture;
                        selectedGateIndex                     = -1;
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
                    selectedGateIndex = i;
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
            if (selectedGateIndex != -1) // Only show gate menu when a gate is selected.
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
