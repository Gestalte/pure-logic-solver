#include "raylib.h"
#include <stdbool.h>

#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define TEXTURE_COUNT 22
#define GATE_COUNT 28
#define FONT_SIZE 16

const int ScreenWidth  = 800;
const int ScreenHeight = 500;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} MyRect;

typedef struct
{
    MyRect rect;
    char* label;
} Button;

typedef struct
{
    char level; // level in terms of how big the gate tree is.
    char place; // place within the level
    MyRect rect;
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
    DrawRectangle(button->rect.x, button->rect.y, button->rect.w, button->rect.h, LIGHTGRAY);
    DrawRectangleLines(button->rect.x, button->rect.y, button->rect.w, button->rect.h, DARKGRAY);
    DrawText(button->label, button->rect.x + (button->rect.w/3), button->rect.y + (button->rect.h/4), FONT_SIZE, BLACK);
}

static bool CheckLeftClick(Button *button)
{
    Rectangle r = {(float)button->rect.x,(float)button->rect.y,(float)button->rect.w,(float)button->rect.h};
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
        Gate gate = {level, place, {0, 0, 100, 50}, false, &Textures[15], &Textures[0]};
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

    Button saveEdit = {{ScreenWidth/2-50, 30, 100, 30}, "Save"};
    Button decrementLevel = {{saveEdit.rect.x + 110, 30, 50, 30}, "-"};
    Button incrementLevel = {{decrementLevel.rect.x + 110, 30, 50, 30}, "+"};
    MyRect gateMenu = {(ScreenWidth/2) - 300, (ScreenHeight) - 80, 600, 70};

    bool isEditMode = true;
    char levels = 1;
   
    int centerX = ScreenWidth/2;
    int centerY = ScreenHeight/2;
    
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if(CheckLeftClick(&decrementLevel))
        {
            levels = clampInclusive(--levels, 1, 6);
        }

        if(CheckLeftClick(&incrementLevel))
        {
            levels = clampInclusive(++levels, 1, 6);
        }

        int totalGateWidth = levels * 100;

        for (int i = 0; i < GATE_COUNT; i++) 
        {
            int totalLevelHeight = Gates[i].level * 50;

            Gates[i].rect.x = centerX + (totalGateWidth/2) - (100 * Gates[i].level);
            Gates[i].rect.y = centerY - (totalLevelHeight/2) + (50 * Gates[i].place) -50;

            if(Gates[i].level == levels && Gates[i].place == Gates[i].level)
            {
                break;
            }
        }

        if(CheckLeftClick(&saveEdit))
        {
            isEditMode = isEditMode == true ? false : true;
            saveEdit.label = isEditMode ? "Save" : "Edit";
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
            DrawText(levelsText, decrementLevel.rect.x + 75, 35, FONT_SIZE, BLACK);
            DrawButton(&incrementLevel);

            // TODO: Only show this menu when a gate is selected. 
            // It is intended to be where you select which gate should be displayed.
            DrawRectangle(gateMenu.x, gateMenu.y, gateMenu.w, gateMenu.h, LIGHTGRAY);
            DrawRectangleLines(gateMenu.x, gateMenu.y, gateMenu.w, gateMenu.h, DARKGRAY);
            for (int i = 0; i < 6; i++) 
            {
                DrawTexture(Textures[i], gateMenu.x + (i * 100), gateMenu.y + 10, WHITE);
            }
        }

        DrawLine(ScreenWidth/2, 0, ScreenWidth/2, ScreenHeight, DARKBLUE);
        DrawLine(0, ScreenHeight/2, ScreenWidth, ScreenHeight/2, DARKBLUE);
        
        for (int i = 0; i < GATE_COUNT; i++) 
        {
            DrawTexture(*Gates[i].body, Gates[i].rect.x, Gates[i].rect.y, WHITE);
            DrawTexture(*Gates[i].line, Gates[i].rect.x+75, Gates[i].rect.y, WHITE);

            DrawRectangleLines(Gates[i].rect.x + 75, Gates[i].rect.y, 50, 50, GREEN);
            DrawRectangleLines(Gates[i].rect.x, Gates[i].rect.y, Gates[i].rect.w, Gates[i].rect.h, RED);
            
            if(Gates[i].level == levels && Gates[i].place == Gates[i].level)
            {
                break;
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
