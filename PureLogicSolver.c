#include "raylib.h"
#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define GATECOUNT 6
#define LINECOUNT 15
#define MAXGATECOUNT 21

Texture2D gateTextures[GATECOUNT];
Texture2D lineTextures[LINECOUNT];

typedef struct
{
    int x;
    int y;
    int width;
    int height;
} rect;

typedef struct
{
    Texture2D *texture;
    rect rect;
} image2D;

// typedef struct
// {
//     image2D *image;
//     struct gate *gate;
// } outputLine;

// TODO: Maybe make a struct for straight input lines for leftmost inputs
typedef struct
{
    image2D image;

    struct gate *parent;
    struct gate *leftChild;
    struct gate *rightChild;

    // NOTE: To check if the current gate's output line is valid, check its children's lines.
    image2D outputLine;
} gate;

typedef struct
{
    rect rect;
    char *text;
} ButtonData;

static void UpdateGateComplex(
    gate *currentGate, int currentLevel, int maxLevel, int columnCount, int screenWidth, int screenHeight)
{
    int totalWidth  = 100 * maxLevel;
    int totalHeight = 50 * maxLevel;
    int width       = 100;
    int height      = 50;
    int x           = (screenWidth / 2) - 50;
    int y           = (screenHeight / 2) - 25;

    currentGate->image.rect = (rect){x, y, width, height};

    if (currentLevel == 1) // Root gate
    {
        rect *r                 = &currentGate->image.rect;
        rect lineRect           = {r->x + (r->width / 2), r->y, 50, 50};
        image2D img             = {&lineTextures[4], lineRect};
        currentGate->outputLine = img;
        currentGate->parent     = 0; // Root doesn't have a parent.
    }

    columnCount++;
}

static void DrawGateComplex(gate *currentGate, int currentLevel, int maxLevel, int columnCount)
{
    DrawTexture(*currentGate->image.texture, currentGate->image.rect.x, currentGate->image.rect.y, WHITE);
    DrawRectangleLines(
        currentGate->image.rect.x,
        currentGate->image.rect.y,
        currentGate->image.rect.width,
        currentGate->image.rect.height,
        RED);

    if (currentLevel == 1) // Root gate
    {
        DrawTexture(
            *currentGate->outputLine.texture, currentGate->outputLine.rect.x, currentGate->outputLine.rect.y, WHITE);
        DrawRectangleLines(
            currentGate->outputLine.rect.x,
            currentGate->outputLine.rect.y,
            currentGate->outputLine.rect.width,
            currentGate->outputLine.rect.height,
            BLUE);
    }
}

static int clampInclusive(int value, int min, int max)
{
    return value <= min ? min : value >= max ? max : value;
}

static int CheckLeftClick(rect rect)
{
    Rectangle r = {(float)rect.x, (float)rect.y, (float)rect.width, (float)rect.height};
    return (CheckCollisionPointRec(GetMousePosition(), r) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) == 1 ? 1 : 0;
}

static void DrawButton(ButtonData *buttonData)
{
    DrawRectangle(buttonData->rect.x, buttonData->rect.y, buttonData->rect.width, buttonData->rect.height, LIGHTGRAY);
    DrawRectangleLines(
        buttonData->rect.x, buttonData->rect.y, buttonData->rect.width, buttonData->rect.height, DARKGRAY);
    DrawText(
        buttonData->text,
        buttonData->rect.x + (buttonData->rect.width / 3),
        buttonData->rect.y + (buttonData->rect.height / 4),
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
    char *gateNames[GATECOUNT] = {
        "resources/gates/AND.png",
        "resources/gates/NAND.png",
        "resources/gates/NOR.png",
        "resources/gates/OR.png",
        "resources/gates/XNOR.png",
        "resources/gates/XOR.png"};

    for (int i = 0; i < GATECOUNT; ++i)
    {
        Image img       = LoadImage(gateNames[i]);
        gateTextures[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    char *lineNames[LINECOUNT] = {
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

    for (int i = 0; i < LINECOUNT; i++)
    {
        Image line      = LoadImage(lineNames[i]);
        lineTextures[i] = LoadTextureFromImage(line);
        UnloadImage(line);
    }

    Rectangle gateMenuRect = {((float)screenWidth / 2) - 300.0f, (float)screenHeight - 80.0f, 600.0f, 70.0f};

    image2D gateMenuItems[GATECOUNT];

    for (int i = 0; i < GATECOUNT; i++)
    {
        image2D menuGate = {
            &gateTextures[i], {(gateMenuRect.x + (i * 100.0f)), (gateMenuRect.y + 10.0f), 100.0f, 50.0f}};
        gateMenuItems[i] = menuGate;
    }

    int selectedGateIndex = -1;
    int IsEditMode        = 1;
    char gateTreeHeight   = 1;

    ButtonData saveEditButton = {{screenWidth / 2 - 50, screenHeight - (screenHeight - 20), 100, 30}, "Edit"};

    ButtonData decrementLevelsButton = {
        {saveEditButton.rect.x + 110, screenHeight - (screenHeight - 20), 50, 30}, " -"};

    ButtonData incrementLevelsButton = {
        {decrementLevelsButton.rect.x + 110, screenHeight - (screenHeight - 20), 50, 30}, " +"};

    gate gates[MAXGATECOUNT]; // = {{&gateTextures[0], 0}};
    image2D gateImg = {&gateTextures[0], 0};
    image2D lineImg = {&lineTextures[4], 0};

    for (int i = 0; i < MAXGATECOUNT; i++)
    {
        gate gate = {gateImg, 0, 0, 0, lineImg};
        gates[i]  = gate;
    }

    gate rootGate = gates[0];

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        // Set positions gates should draw at.
        int startingX = (screenWidth / 2) - (50 * gateTreeHeight);
        for (int i = 0; i < GATECOUNT; ++i)
        {
            gates[i].image.rect = (rect){startingX + (i * 100), (screenHeight / 2) - 25, 100, 50};
        }

        if (CheckLeftClick(saveEditButton.rect))
        {
            IsEditMode = IsEditMode ? 0 : 1;
        }

        if (IsEditMode)
        {
            // Handle click for Increment and Decrement levels
            if (CheckLeftClick(incrementLevelsButton.rect))
            {
                int old        = gateTreeHeight;
                gateTreeHeight = clampInclusive(++gateTreeHeight, 1, 6);
                if (old != 6)
                {
                    gates[gateTreeHeight - 1].image.texture = &gateTextures[0];
                }
            }

            if (CheckLeftClick(decrementLevelsButton.rect))
            {
                gateTreeHeight = clampInclusive(--gateTreeHeight, 1, 6);
            }

            // handles click for gate menu items.
            for (int i = 0; i < GATECOUNT; i++)
            {

                if (CheckLeftClick(gateMenuItems[i].rect))
                {
                    if (selectedGateIndex != -1)
                    {
                        gates[selectedGateIndex].image.texture = gateMenuItems[i].texture;
                        selectedGateIndex                      = -1;
                    }
                    break;
                }
            }

            // handle click for selecting a gate
            // for (int i = 0; i < gateTreeHeight; ++i)
            // {
            //     if (CheckLeftClick(gates[i].image.rect))
            //     {
            //         selectedGateIndex = i;
            //         break;
            //     }
            // }
        }

        UpdateGateComplex(&rootGate, 1, gateTreeHeight, 1, screenWidth, screenHeight);

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
                DrawRectangle(gateMenuRect.x, gateMenuRect.y, gateMenuRect.width, gateMenuRect.height, RAYWHITE);
                for (int i = 0; i < GATECOUNT; ++i)
                {
                    DrawTexture(*gateMenuItems[i].texture, gateMenuRect.x + (i * 100), gateMenuRect.y + 10, WHITE);
                }
            }

            // Decrement levels button
            DrawButton(&decrementLevelsButton);

            // Levels count
            char levelText[2] = {(char)(gateTreeHeight + 48), '\0'}; // convert int to string
            DrawText(
                levelText,
                (decrementLevelsButton.rect.x + 50 + (decrementLevelsButton.rect.width / 2)),
                (decrementLevelsButton.rect.y + (decrementLevelsButton.rect.height / 4)),
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
        // for (int i = 0; i < gateTreeHeight; ++i)
        // {
        //     DrawTexture(*gates[i].image.texture, gates[i].image.rect.x, gates[i].image.rect.y, WHITE);
        // }
        //
        DrawGateComplex(&rootGate, 1, gateTreeHeight, 1);

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
