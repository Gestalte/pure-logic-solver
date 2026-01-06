#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>

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
    MyRect rect;
    Texture2D* texture;
} Picture;

typedef struct
{
    char level; // level in terms of how big the gate tree is.
    char place; // place within the level
    bool locked;
    Picture line;
    Picture body;
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

static bool CheckLeftClick(MyRect* rect)
{
    Rectangle r = {(float)rect->x,(float)rect->y,(float)rect->w,(float)rect->h};
    return CheckCollisionPointRec(GetMousePosition(), r) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static int NumberOfGates(int level)
{
    int count = 0;
    for (int i = 0; i < level ; i++) 
    {
        count += i + 1;
    }
    return count;
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
    
    int centerX = ScreenWidth/2;
    int centerY = ScreenHeight/2;
    char level = 1;
    char place = 1;
    for (int i = 0; i < GATE_COUNT; i++) 
    {
        Gate gate = {level, place, false, {{0, 0, 50, 50}, &Textures[15]}, {{0, 0, 100, 50} ,&Textures[0]}};
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
    Gate* selectedGate = 0;
        
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        int gateCount = NumberOfGates(levels);

        if(CheckLeftClick(&decrementLevel.rect))
        {
            levels = clampInclusive(--levels, 1, 6);
        }

        if(CheckLeftClick(&incrementLevel.rect))
        {
            levels = clampInclusive(++levels, 1, 6);
        }

        int totalGateWidth = levels * 100;

        for (int i = 0; i < gateCount; i++) 
        {
            int totalLevelHeight = Gates[i].level * 50;

            Gates[i].body.rect.x = centerX + (totalGateWidth/2) - (100 * Gates[i].level);
            Gates[i].body.rect.y = centerY - (totalLevelHeight/2) + (50 * Gates[i].place) -50;

            Gates[i].line.rect.x = centerX + (totalGateWidth/2) - (100 * Gates[i].level) + 75;
            Gates[i].line.rect.y = centerY - (totalLevelHeight/2) + (50 * Gates[i].place) -50;
        }

        if(CheckLeftClick(&saveEdit.rect))
        {
            isEditMode = isEditMode == true ? false : true;
            saveEdit.label = isEditMode ? "Save" : "Edit";
        }

        if(isEditMode)
        {
            for (int i = 0; i < gateCount ; i++) 
            {
                if(CheckLeftClick(&Gates[i].body.rect))
                {
                    selectedGate = &Gates[i];
                    printf("selectedGate: %d-%d\n", Gates[i].level, Gates[i].place);
                    break;
                }
            }

            // Unselect gate if it is no longer being shown.
            if(selectedGate != 0 && gateCount < NumberOfGates(selectedGate->level))
            {
                selectedGate = 0;
            }

            // Check for gate menu selection
            if(selectedGate != 0)
            {
                for (int i = 0; i < 6; i++) 
                {
                    MyRect rect = {gateMenu.x + (i * 100), gateMenu.y + 10, 100,50};
                    if(CheckLeftClick(&rect))
                    {
                        selectedGate->body.texture = &Textures[i];
                        selectedGate = 0;
                        break;
                    }
                }
            }
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
            // Draw level setup buttons
            DrawButton(&decrementLevel);
            char levelsText[2] = {(char)(levels+48),'\0'}; // Converts number to ascii char*
            DrawText(levelsText, decrementLevel.rect.x + 75, 35, FONT_SIZE, BLACK);
            DrawButton(&incrementLevel);

            // Draw gate type selection menu
            if(selectedGate != 0)
            {
                DrawRectangle(selectedGate->body.rect.x, selectedGate->body.rect.y, selectedGate->body.rect.w, selectedGate->body.rect.h, GREEN);

                DrawRectangle(gateMenu.x, gateMenu.y, gateMenu.w, gateMenu.h, LIGHTGRAY);
                DrawRectangleLines(gateMenu.x, gateMenu.y, gateMenu.w, gateMenu.h, DARKGRAY);
                for (int i = 0; i < 6; i++) 
                {
                    DrawTexture(Textures[i], gateMenu.x + (i * 100), gateMenu.y + 10, WHITE);
                }
            }
        }

//         DrawLine(ScreenWidth/2, 0, ScreenWidth/2, ScreenHeight, DARKBLUE);
//         DrawLine(0, ScreenHeight/2, ScreenWidth, ScreenHeight/2, DARKBLUE);
        
        for (int i = 0; i < gateCount; i++) 
        {
            DrawTexture(*Gates[i].body.texture, Gates[i].body.rect.x, Gates[i].body.rect.y, WHITE);
            DrawTexture(*Gates[i].line.texture, Gates[i].line.rect.x, Gates[i].line.rect.y, WHITE);

//             DrawRectangleLines(Gates[i].line.rect.x, Gates[i].line.rect.y, Gates[i].line.rect.w, Gates[i].line.rect.h, GREEN);
//             DrawRectangleLines(Gates[i].body.rect.x, Gates[i].body.rect.y, Gates[i].body.rect.w, Gates[i].body.rect.h, RED);
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
