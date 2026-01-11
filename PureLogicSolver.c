#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>

#define BACKGROUNDBLUE CLITERAL(Color){0, 158, 255, 255}
#define TEXTURE_COUNT 21
#define GATE_COUNT 28
#define FONT_SIZE 16
#define LINE_TYPES 15

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

enum LineColor
{
    black,
    white,
    gray
};

enum LineType
{
    up,
    down,
    fork,
    dual,
    straight 
};

typedef struct
{
    enum LineColor color;
    enum LineType type;
    Texture2D* texture;
} LineDefinition;

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
    LineDefinition* definition;
    bool locked;
    MyRect rect; // TODO: Separate rects for line and ends?
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
    "resources/lines/up_black.png",
    "resources/lines/down_black.png",
    "resources/lines/fork_black.png",
    "resources/lines/dual_black.png",
    "resources/lines/straight_black.png",
    "resources/lines/up_white.png",
    "resources/lines/down_white.png",
    "resources/lines/fork_white.png",
    "resources/lines/dual_white.png",
    "resources/lines/straight_white.png",
    "resources/lines/up_gray.png",
    "resources/lines/down_gray.png",
    "resources/lines/fork_gray.png",
    "resources/lines/dual_gray.png",
    "resources/lines/straight_gray.png",
};

Texture2D Textures[TEXTURE_COUNT];
LineDefinition Lines[LINE_TYPES];
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

        if(A->line.definition->color == gray 
            || B->line.definition->color == gray 
            || Gates[i].line.definition->color == gray)
        {
            Gates[i].gate.works = false;
            return;
        }

        inA = A->line.definition->color == white;
        inB = B->line.definition->color == white;
        out = Gates[i].line.definition->color == white;

        switch (Gates[i].gate.type) {
            case AND:
                if(inA && inB)          result = true;
                else if(inA | inB)      result = false;
                else if(!inA && !inB)   result = false;
                break;
            case NAND:
                if(inA && inB)          result = false;
                else if(inA | inB)      result = true;
                else if(!inA && !inB)   result = true;
                break; 
            case OR:
                if(inA && inB)          result = true;
                else if(inA | inB)      result = true;
                else if(!inA && !inB)   result = false;
                break; 
            case NOR:
                if(inA && inB)          result = false;
                else if(inA | inB)      result = false;
                else if(!inA && !inB)   result = true;
                break; 
            case XOR:
                if(inA && inB)          result = false;
                else if(inA | inB)      result = true;
                else if(!inA && !inB)   result = false;
                break; 
            case XNOR:
                if(inA && inB)          result = true;
                else if(inA | inB)      result = false;
                else if(!inA && !inB)   result = true;
                break;
        }
        Gates[i].gate.works = out == result;
    }
}

static LineDefinition* FindLineDefinition(enum LineColor color, enum LineType type)
{
    for (int i = 0; i<LINE_TYPES; i++) 
    {
        if(Lines[i].color == color && Lines[i].type == type)
        {
            return &Lines[i];
        }
    }
    return 0;
}

static LineDefinition* SwapLineDefinition(enum LineColor color, Line* line)
{
    enum LineType type = line->definition->type;
    return FindLineDefinition(color, type);
}

static LineDefinition* NextLineColor(Line* line)
{
    enum LineColor newColor = black;
    switch (line->definition->color) {
        case black:
            newColor = white;
            break;
        case white:
            newColor = gray;
            break;
        case gray:
            newColor = black;
            break;
    }
    return SwapLineDefinition(newColor, line);
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

    int index = 0;
    for(enum LineColor color = black; color <= gray; color++)
    {
        for(enum LineType type = up; type <= straight; type++)
        { 
            LineDefinition def = {color, type, &Textures[(5 * (color + 1)) + (type + 1)]};
            Lines[index] = def;
            index++;
        }
    }
       
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
        Line line = {FindLineDefinition(gray, fork), false, {0,0,50,50}};

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
            line.definition = FindLineDefinition(gray,down);
        }

        if(level == place)
        {
            line.definition = FindLineDefinition(gray, up);
        }

        if(level == 1 && place == 1)
        {
            line.definition = FindLineDefinition(gray, straight);
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
        int drawCount = NumberOfGates(levels + 1);
        for (int i = 0; i < drawCount; i++) 
        {
            int totalLevelHeight = (Gates[i].level * 50);

            enum LineType type = Gates[i].line.definition->type;
            if(type == dual || type == straight && i < gateCount && Gates[i].level != 1)
            {
                enum LineType newType = down;
                if(type == dual)
                {
                    newType = fork;
                }
                else if (Gates[i].place == Gates[i].level)
                {
                    newType = up;
                }
                Gates[i].line.definition = FindLineDefinition(Gates[i].line.definition->color, newType);
            }

            Gates[i].gate.rect.x = centerX + (totalGateWidth/2) - (100 * Gates[i].level);
            Gates[i].gate.rect.y = centerY - (totalLevelHeight/2) + (50 * Gates[i].place) - 50;

            Gates[i].line.rect.x = centerX + (totalGateWidth/2) - (100 * Gates[i].level) + 75;
            Gates[i].line.rect.y = centerY - (totalLevelHeight/2) + (50 * Gates[i].place) - 50;

            if(i >= gateCount)
            {
                enum LineType newType = dual;
                if(Gates[i].place == 1 || Gates[i].place == Gates[i].level)
                {
                    newType = straight;
                }
                Gates[i].line.definition = FindLineDefinition(Gates[i].line.definition->color, newType);
            }
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
                    MyRect rect = {gateMenu.x + (i * 100), gateMenu.y + 10, 100, 50};
                    if(CheckLeftClick(&rect))
                    {
                        selectedGate->gate.texture = &Textures[i];
                        selectedGate->gate.type = i;
                        selectedGate = 0;
                        break;
                    }
                }
            }

            // Check for line changes
            for (int i = 0; i < drawCount; i++)
            {
                if(CheckLeftClick(&Gates[i].line.rect))
                {
                    Gates[i].line.definition = NextLineColor(&Gates[i].line);
                    Gates[i].line.locked = Gates[i].line.definition->color == gray ? false : true;
                }
            }
        }
        else
        {
            // Check for line changes
            for (int i = 0; i < drawCount; i++)
            {
                if(CheckLeftClick(&Gates[i].line.rect) && Gates[i].line.locked == false)
                {
                    Gates[i].line.definition = NextLineColor(&Gates[i].line);
                }
            }
            
            ValidateGates(gateCount);

            // Reset button click
            if(CheckLeftClick(&resetLines.rect))
            {
                for (int i = 0; i < GATE_COUNT; i++) 
                {
                    if(Gates[i].line.locked == false)
                    {
                        Gates[i].line.definition = SwapLineDefinition(gray, &Gates[i].line);
                    }

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
                DrawRectangleLines(selectedGate->gate.rect.x, selectedGate->gate.rect.y, selectedGate->gate.rect.w, selectedGate->gate.rect.h, GREEN);

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

        // Draw gates
        for (int i = 0; i < drawCount; i++) 
        {
            if(Gates[i].gate.works && !isEditMode)
            {
                DrawRectangle(Gates[i].gate.rect.x, Gates[i].gate.rect.y, Gates[i].gate.rect.w, Gates[i].gate.rect.h, GREEN);
            }
           
            DrawTexture(*Gates[i].line.definition->texture, Gates[i].line.rect.x, Gates[i].line.rect.y, WHITE);

            if(i < gateCount)
            {
                DrawTexture(*Gates[i].gate.texture, Gates[i].gate.rect.x, Gates[i].gate.rect.y, WHITE);
            }
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
