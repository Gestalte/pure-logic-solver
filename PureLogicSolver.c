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
    bool works;
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

static void CheckLineClick(Gate* gate, Texture2D* gray, Texture2D* white, Texture2D* black)
{
    if(gate->line.texture == gray)
    {
       gate->line.texture = white;
       gate->locked = true;
    }
    else if(gate->line.texture == white)
    {
        gate->line.texture = black;
        gate->locked = true;
    }
    else if(gate->line.texture == black)
    {
        gate->line.texture = gray;
        gate->locked = false;
    }
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
  
    Texture2D* andGate = &Textures[0];

    Texture2D* grayDown = &Textures[11];
    Texture2D* grayUp = &Textures[8];
    Texture2D* grayStraight = &Textures[21];
    Texture2D* grayFork = &Textures[15];
    Texture2D* grayDual = &Textures[18];

    Texture2D* whiteDown = &Textures[9];
    Texture2D* whiteUp = &Textures[6];
    Texture2D* whiteStraight = &Textures[19];
    Texture2D* whiteFork = &Textures[12];
    Texture2D* whiteDual = &Textures[16];

    Texture2D* blackDown = &Textures[10];
    Texture2D* blackUp = &Textures[7];
    Texture2D* blackStraight = &Textures[20];
    Texture2D* blackFork = &Textures[13];
    Texture2D* blackDual = &Textures[17];

    int centerX = ScreenWidth/2;
    int centerY = ScreenHeight/2;
    char level = 0;
    char place = 0;

    for (int i = 0; i < GATE_COUNT; i++) 
    {
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
        Picture line = {{0, 0, 50, 50}, grayFork};

        if(level != place)
        {
            place++;
        }
        else if(level == place)
        {
            level++;
            place = 1;
        }

        if(place == 1)
        {
            line.texture = grayDown;
        }

        if(level == place)
        {
            line.texture = grayUp;
        }

        if(level == 1 && place == 1)
        {
            line.texture = grayStraight ;
        }

        Gate gate = 
        {
            level, 
            place, 
            false, 
            false, 
            line,
            {{0, 0, 100, 50}, andGate}  // gate
        };
        Gates[i] = gate;
    }

    Button saveEdit = {{ScreenWidth/2-50, 30, 100, 30}, "Save"};
    Button resetLines = {{ScreenWidth/2-160, 30, 100, 30}, "Reset"};
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

            // Check for line changes
            for (int i = 0; i < gateCount; i++)
            {
                if(CheckLeftClick(&Gates[i].line.rect))
                {
                     CheckLineClick(&Gates[i], grayFork, whiteFork, blackFork);
                     CheckLineClick(&Gates[i], grayUp, whiteUp, blackUp);
                     CheckLineClick(&Gates[i], grayDown, whiteDown, blackDown);
                     CheckLineClick(&Gates[i], grayStraight, whiteStraight, blackStraight);
                     CheckLineClick(&Gates[i], grayDual, whiteDual, blackDual);
                }
            }
        }
        else
        {
            // Check for line changes
            for (int i = 0; i < gateCount; i++)
            {
                if(CheckLeftClick(&Gates[i].line.rect) && Gates[i].locked == false)
                {
                    CheckLineClick(&Gates[i], grayFork, whiteFork, blackFork);
                    CheckLineClick(&Gates[i], grayUp, whiteUp, blackUp);
                    CheckLineClick(&Gates[i], grayDown, whiteDown, blackDown);
                    CheckLineClick(&Gates[i], grayStraight, whiteStraight, blackStraight);
                    CheckLineClick(&Gates[i], grayDual, whiteDual, blackDual);                
                    Gates[i].locked = false; // CheckLineClick sets locked so set it back.
                }
            }

            if(CheckLeftClick(&resetLines.rect))
            {
                for (int i = 0; i < GATE_COUNT; i++) 
                {
                    if(Gates[i].locked == false)
                    {
                        if(Gates[i].level == Gates[i].place && Gates[i].level != 1)
                        {
                            Gates[i].line.texture = grayUp;
                        }
                        else if(Gates[i].place == 1 && Gates[i].level != 1)
                        {
                            Gates[i].line.texture = grayDown;
                        }
                        else if(Gates[i].level == 1 && Gates[i].place == 1)
                        {
                            Gates[i].line.texture = grayStraight;
                        }
                        else
                        {
                            Gates[i].line.texture = grayFork;
                        }
                    }
                }
            }
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
        else
        {
            DrawButton(&resetLines);
        }

        for (int i = 0; i < gateCount; i++) 
        {
            DrawTexture(*Gates[i].line.texture, Gates[i].line.rect.x, Gates[i].line.rect.y, WHITE);
            DrawTexture(*Gates[i].body.texture, Gates[i].body.rect.x, Gates[i].body.rect.y, WHITE);
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
