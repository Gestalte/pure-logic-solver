#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>

#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define TEXTURE_COUNT 21
#define GATE_COUNT 28
#define FONT_SIZE 16

const int ScreenWidth  = 800;
const int ScreenHeight = 500;

enum GateName
{
    AND,
    NAND,
    OR,
    NOR,
    XOR,
    XNOR
};

enum LineState
{
    black,
    white,
    gray
};

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
    enum LineState color;
    bool locked;
    MyRect rect;
    Texture2D* texture;
} Line;

typedef struct
{
    enum GateName type;
    bool works;
    MyRect rect;
    Texture2D* texture;
} Gate;

typedef struct
{
    char level; // level in terms of how big the gate tree is.
    char place; // place within the level
    Line line;
    Gate gate;
} GateComplex;

char* ImageFilenames[] = 
{
    "resources/gates/AND.png",
    "resources/gates/NAND.png",
    "resources/gates/OR.png",
    "resources/gates/NOR.png",
    "resources/gates/XOR.png",
    "resources/gates/XNOR.png",
    "resources/lines/up_white.png",
    "resources/lines/down_white.png",
    "resources/lines/fork_white.png",
    "resources/lines/dual_white.png",
    "resources/lines/straight_white.png",
    "resources/lines/up_black.png",
    "resources/lines/down_black.png",
    "resources/lines/fork_black.png",
    "resources/lines/dual_black.png",
    "resources/lines/straight_black.png",
    "resources/lines/up_gray.png",
    "resources/lines/down_gray.png",
    "resources/lines/fork_gray.png",
    "resources/lines/dual_gray.png",
    "resources/lines/straight_gray.png",
};

Texture2D Textures[TEXTURE_COUNT];
GateComplex Gates[GATE_COUNT]; // TODO: These aren't just gates, give them a better name.

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

// TODO: Maybe rewrite to use enums instead?
static void CheckLineClick(GateComplex* gate, Texture2D* gray_, Texture2D* white_, Texture2D* black_)
{
    if(gate->line.texture == gray_)
    {
       gate->line.texture = white_;
       gate->line.locked = true;
       gate->line.color = white;
    }
    else if(gate->line.texture == white_)
    {
        gate->line.texture = black_;
        gate->line.locked = true;
        gate->line.color = black;
    }
    else if(gate->line.texture == black_)
    {
        gate->line.texture = gray_;
        gate->line.locked = false;
        gate->line.color = gray;
    }
}

static GateComplex* GetGateComplex(int level, int place)
{
    for (int i = 0; i < GATE_COUNT; i++) 
    {
        if(Gates[i].level == level && Gates[i].place == place)
        {
            return &Gates[i];
        }
    }
    return 0;
}

static void ValidateGates(int numberOfGates)
{
    for(int i = 0; i < numberOfGates; i++)
    {
        bool result = false;
        bool inA = false;
        bool inB = false;
        bool out = false;

        int level = Gates[i].level;
        int place = Gates[i].place;
        GateComplex* A = GetGateComplex(level + 1, place);
        GateComplex* B = GetGateComplex(level + 1, place + 1);

        if(A->line.color == gray || B->line.color == gray || Gates[i].line.color == gray)
        {
            Gates[i].gate.works = false;
            return;
        }

        inA = A->line.color == white;
        inB = B->line.color == white;
        out = Gates[i].line.color == white;

        switch (Gates[i].gate.type) {
            case AND:
                if(inA && inB)                        result = true;
                else if(inA | inB) result = false;
                else if(inA == false && inB == false) result = false;
                break; 
            case NAND:
                if(inA && inB)                        result = false;
                else if(inA | inB) result = true;
                else if(inA == false && inB == false) result = true;
                break; 
            case OR:
                if(inA && inB)                        result = true;
                else if(inA | inB) result = true;
                else if(inA == false && inB == false) result = false;
                break; 
            case NOR:
                if(inA && inB)                        result = false;
                else if(inA | inB) result = false;
                else if(inA == false && inB == false) result = true;
                break; 
            case XOR:
                if(inA && inB)                        result = false;
                else if(inA | inB)                    result = true;
                else if(inA == false && inB == false) result = false;
                break; 
            case XNOR:
                if(inA && inB)                        result = true;
                else if(inA | inB) result = false;
                else if(inA == false && inB == false) result = true;
                break;
        }
        Gates[i].gate.works = out == result;
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

    Texture2D* grayStraight  = &Textures[20];
    Texture2D* grayDual      = &Textures[19];
    Texture2D* grayFork      = &Textures[18];
    Texture2D* grayDown      = &Textures[17];
    Texture2D* grayUp        = &Textures[16];

    Texture2D* blackStraight = &Textures[15];
    Texture2D* blackDual     = &Textures[14];
    Texture2D* blackFork     = &Textures[13];
    Texture2D* blackDown     = &Textures[12];
    Texture2D* blackUp       = &Textures[11];
    
    Texture2D* whiteStraight = &Textures[10];
    Texture2D* whiteDual     = &Textures[9];
    Texture2D* whiteFork     = &Textures[8];
    Texture2D* whiteDown     = &Textures[7];
    Texture2D* whiteUp       = &Textures[6];
   
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
        Line line = {gray, false, {0, 0, 50, 50}, grayFork};

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
            line.texture = grayStraight;
        }
        
        Gate gate = {AND, false, {0, 0, 100, 50}, andGate}; 

        GateComplex gateComplex = {level, place, line , gate };
        Gates[i] = gateComplex;
    }

    Button saveEdit = {{ScreenWidth/2-50, 30, 100, 30}, "Save"};
    Button resetLines = {{ScreenWidth/2-160, 30, 100, 30}, "Reset"};
    Button decrementLevel = {{saveEdit.rect.x + 110, 30, 50, 30}, "-"};
    Button incrementLevel = {{decrementLevel.rect.x + 110, 30, 50, 30}, "+"};
    MyRect gateMenu = {(ScreenWidth/2) - 300, (ScreenHeight) - 80, 600, 70};

    bool isEditMode = true;
    char levels = 1;
    GateComplex* selectedGate = 0;
        
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
            int totalLevelHeight = (Gates[i].level * 50);

            Gates[i].gate.rect.x = centerX + (totalGateWidth/2) - (100 * Gates[i].level);
            Gates[i].gate.rect.y = centerY - (totalLevelHeight/2) + (50 * Gates[i].place) - 50;

            Gates[i].line.rect.x = centerX + (totalGateWidth/2) - (100 * Gates[i].level) + 75;
            Gates[i].line.rect.y = centerY - (totalLevelHeight/2) + (50 * Gates[i].place) - 50;
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
                if(CheckLeftClick(&Gates[i].gate.rect))
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
                        selectedGate->gate.texture = &Textures[i];
                        switch (i) {
                        case 0: 
                            selectedGate->gate.type = AND;
                            break;
                        case 1:
                            selectedGate->gate.type = NAND;
                            break;
                        case 2:
                            selectedGate->gate.type = OR;
                            break;
                        case 3:
                            selectedGate->gate.type = NOR;
                            break;
                        case 4:
                            selectedGate->gate.type = XOR;
                            break;
                        case 5:
                            selectedGate->gate.type = XNOR;
                            break;
                        }
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
                if(CheckLeftClick(&Gates[i].line.rect) && Gates[i].line.locked == false)
                {
                    CheckLineClick(&Gates[i], grayFork, whiteFork, blackFork);
                    CheckLineClick(&Gates[i], grayUp, whiteUp, blackUp);
                    CheckLineClick(&Gates[i], grayDown, whiteDown, blackDown);
                    CheckLineClick(&Gates[i], grayStraight, whiteStraight, blackStraight);
                    CheckLineClick(&Gates[i], grayDual, whiteDual, blackDual);                
                    Gates[i].line.locked = false; // CheckLineClick sets locked so set it back.
                }
            }
            
            ValidateGates(gateCount);

            if(CheckLeftClick(&resetLines.rect))
            {
                for (int i = 0; i < GATE_COUNT; i++) 
                {
                    if(Gates[i].line.locked == false)
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

                    Gates[i].line.color = gray;
                    Gates[i].gate.works = false;
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
                DrawRectangle(selectedGate->gate.rect.x, selectedGate->gate.rect.y, selectedGate->gate.rect.w, selectedGate->gate.rect.h, GREEN);

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
            if(Gates[i].gate.works)
            {
                DrawRectangle(Gates[i].gate.rect.x, Gates[i].gate.rect.y, Gates[i].gate.rect.w, Gates[i].gate.rect.h, GREEN);
            }
            DrawTexture(*Gates[i].line.texture, Gates[i].line.rect.x, Gates[i].line.rect.y, WHITE);
            //DrawRectangleLines(Gates[i].line.rect.x, Gates[i].line.rect.y, Gates[i].line.rect.w, Gates[i].line.rect.h, GREEN);
            DrawTexture(*Gates[i].gate.texture, Gates[i].gate.rect.x, Gates[i].gate.rect.y, WHITE);
            //DrawRectangleLines(Gates[i].body.rect.x, Gates[i].body.rect.y, Gates[i].body.rect.w, Gates[i].body.rect.h, RED);
            
            DrawRectangleLines(Gates[i].gate.rect.x + 20, Gates[i].gate.rect.y, 60, Gates[i].gate.rect.h, BLUE);
        }

        DrawLine(0, ScreenHeight/2, ScreenWidth, ScreenHeight/2, DARKBLUE);

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
