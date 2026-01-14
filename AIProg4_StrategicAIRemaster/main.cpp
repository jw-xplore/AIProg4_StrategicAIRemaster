#include <raylib.h>
#include <string>
#include "Constants.h"
#include "World.h"
#include "Database.h"

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
void RunGame()
{
    // Init
    World world = World("resources/WorldMap.txt");

    // Gameloop
    while (!WindowShouldClose())
    {
        // Update entities
        float dt = GetFrameTime() * TIME_SCALE;
        world.Update(dt);

        // Rendering
        BeginDrawing();
        ClearBackground(BLACK);
        world.Draw();

        AdjustTimeScale();

        EndDrawing();
    }

    // Cleanup
    delete GameDB::Database::Instance();
}

int main()
{
    // Window setup
    InitWindow(GlobalVars::SCREEN_WIDTH, GlobalVars::SCREEN_HEIGHT, "My first RAYLIB program!");
    SetTargetFPS(60);

    RunGame();

    // End
    CloseWindow();

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
}