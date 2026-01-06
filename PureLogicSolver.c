#include "raylib.h"
#include <stdbool.h>

#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define TEXTURE_COUNT 22
#define GATE_COUNT 28
#define FONT_SIZE 16

const int ScreenWidth  = 800;
const int ScreenHeight = 450;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
    char* label;
} Button;

typedef struct
{
    char level; // level in terms of how big the gate tree is.
    char place; // place within the level
    int x;
    int y;
    int w;
    int h;
    bool locked;
    Texture2D* line;
    Texture2D* body;
} Gate;

char* ImageFilenames[] = 
{
    "resources/gates/AND.png",
    "resources/gates/NAND.png",
    "resources/gates/NOR.png",
    "resources/gates/OR.png",
    "resources/gates/XNOR.png",
    "resources/gates/XOR.png",
    "resources/lines/up_white.png",
    "resources/lines/up_black.png",
    "resources/lines/up_gray.png",
    "resources/lines/down_white.png",
    "resources/lines/down_black.png",
    "resources/lines/down_gray.png",
    "resources/lines/fork_white.png",
    "resources/lines/fork_black.png",
    "resources/lines/fork_gray.png",
    "resources/lines/fork_gray.png",
    "resources/lines/dual_white.png",
    "resources/lines/dual_black.png",
    "resources/lines/dual_gray.png",
    "resources/lines/straight_white.png",
    "resources/lines/straight_black.png",
    "resources/lines/straight_gray.png",
};

Texture2D Textures[TEXTURE_COUNT];
Gate Gates[GATE_COUNT]; // TODO: These aren't just gates, give them a better name.

static int clampInclusive(int value, int min, int max)
{
    return value <= min ? min : value >= max ? max : value;
}

static void DrawButton(Button *button)
{
    DrawRectangle(button->x, button->y, button->w, button->h, LIGHTGRAY);
    DrawRectangleLines(button->x, button->y, button->w, button->h, DARKGRAY);
    DrawText(button->label, button->x + (button->w/3), button->y + (button->h/4), FONT_SIZE, BLACK);
}

static bool CheckLeftClick(Button *button)
{
    Rectangle r = {(float)button->x,(float)button->y,(float)button->w,(float)button->h};
    return CheckCollisionPointRec(GetMousePosition(), r) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(ScreenWidth, ScreenHeight, "Pure Logic Solver");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)

    for (int i = 0; i < TEXTURE_COUNT; i++) 
    {
        Image img = LoadImage(ImageFilenames[i]);
        Textures[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    char level = 1;
    char place = 1;
    for (int i = 0; i < GATE_COUNT; i++) 
    {
        Gate gate = {level, place, 0, 0, 100, 50, false, &Textures[15], &Textures[0]};
        Gates[i] = gate;
        /* There are the same number of places as the level number 
           ----------------------------------
           7 | 6 | 5 | 4 | 3 | 2 | 1 | levels 
           ----------------------------------
           1 | 1 | 1 | 1 | 1 | 1 | 1 | places
           2 | 2 | 2 | 2 | 2 | 2 | 
           3 | 3 | 3 | 3 | 3 | 
           4 | 4 | 4 | 4 | 
           5 | 5 | 5 | 
           6 | 6 | 
           7 |
           */
        if(level != place)
        {
            place++;
        }
        else if(level == place)
        {
            level++;
            place = 1;
        }
    }

    Button saveEdit = {ScreenWidth/2-50, 30, 100, 30, "Save"};
    Button decrementLevel = {saveEdit.x + 110, 30, 50, 30, "-"};
    Button incrementLevel = {decrementLevel.x + 110, 30, 50, 30, "+"};
    Rectangle gateMenu = {((float)ScreenWidth/2) - 300.0f, ((float)ScreenHeight) - 80.0f, 600.0f, 70.0f};

    bool isEditMode = true;
    char levels = 1;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if(CheckLeftClick(&saveEdit))
        {
            isEditMode = isEditMode == true ? false : true;
            saveEdit.label = isEditMode ? "Save" : "Edit";
        }

        if(CheckLeftClick(&decrementLevel))
        {
            levels = clampInclusive(--levels, 1, 6);
        }

        if(CheckLeftClick(&incrementLevel))
        {
            levels = clampInclusive(++levels, 1, 6);
        }

        if(isEditMode)
        {
            // TODO: Check for gate clicks, show gate menu when one is clicked.
        }
        else
        {
            // TODO: Check for line clicks and cycle through colour when one is found.
            // TODO: Toggle success/failure based on inputs vs. output.
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BACKGROUNDBLUE);

        DrawButton(&saveEdit );

        if(isEditMode)
        {
            DrawButton(&decrementLevel);
            char levelsText[2] = {(char)(levels+48),'\0'}; // Converts number to ascii char*
            DrawText(levelsText, decrementLevel.x + 75, 35, FONT_SIZE, BLACK);
            DrawButton(&incrementLevel);

            // TODO: Only show this menu when a gate is selected. 
            // It is intended to be where you select which gate should be displayed.
            DrawRectangle((int)gateMenu.x, (int)gateMenu.y, (int)gateMenu.width, (int)gateMenu.height, LIGHTGRAY);
            DrawRectangleLines((int)gateMenu.x, (int)gateMenu.y, (int)gateMenu.width, (int)gateMenu.height, DARKGRAY);
            for (int i = 0; i < 6; i++) 
            {
                DrawTexture(Textures[i], (int)gateMenu.x + (i * 100), (int)gateMenu.y + 10, WHITE);
            }
        }

        int startX = ((ScreenWidth/2) - 100) + ((levels * 100)/2); 
        int startY = ((ScreenHeight/2) - 25);
        for (int i = 0; i < levels; i++) 
        {
            int adjustAllY = i * 25;
            for (int j = 0; j <= i; j++)
            {
                int newX = startX - (i * 100);
                int newY = startY - adjustAllY + (j * 50);
                int index = i + j;
                Gates[index].x = newX;
                Gates[index].y = newY;
                DrawTexture(*Gates[index].body, Gates[index].x, Gates[index].y, WHITE);
                //DrawTexture(*Gates[index].line, Gates[index].x + 75, Gates[index].y, WHITE);
            }
        }

        EndDrawing();
        //---------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    for (int i = 0; i<GATE_COUNT; i++) 
    {
        UnloadTexture(Textures[i]);
    }

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
