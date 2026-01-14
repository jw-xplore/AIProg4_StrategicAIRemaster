#include <raylib.h>
#include <string>
#include "Constants.h"

extern float TIME_SCALE = 1;

/*
Debug
*/
void AdjustTimeScale()
{
    if (IsKeyDown(KEY_RIGHT))
        TIME_SCALE += 0.1f;
    else  if (IsKeyDown(KEY_LEFT) && TIME_SCALE > 0.1)
        TIME_SCALE -= 0.1f;

    // Show time
    std::string strTime = "Time scale: " + std::to_string(TIME_SCALE);
    char const* cTime = strTime.c_str();
    DrawText(cTime, 50, 20, 24, YELLOW);
}

// Game functionality
int main()
{
    constexpr int screenWidth = GlobalVars::SCREEN_WIDTH;
    constexpr int screenHeight = GlobalVars::SCREEN_HEIGHT;

    // Window setup
    InitWindow(screenWidth, screenHeight, "My first RAYLIB program!");
    SetTargetFPS(60);

    // Gameloop
    while (!WindowShouldClose())
    {
        // Update entities
        float dt = GetFrameTime() * TIME_SCALE;

        // Rendering
        BeginDrawing();
        ClearBackground(BLACK);

        AdjustTimeScale();

        EndDrawing();
    }

    // End
    CloseWindow();

    //_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    //_CrtDumpMemoryLeaks();
}