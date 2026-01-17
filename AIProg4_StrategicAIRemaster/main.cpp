#include <raylib.h>
#include <string>
#include "Constants.h"
#include "World.h"
#include "Database.h"
#include "PathFinding.h"
#include <map>
#include <iostream>

extern float TIME_SCALE = 1;

/*
Debug
*/
void DrawPath(std::vector<Node>* path)
{
    int halfSize = GlobalVars::TILE_SIZE / 2;

    for (int i = 0; i < path->size(); i++)
    {
        DrawCircle((*path)[i].x * GlobalVars::TILE_SIZE, (*path)[i].y * GlobalVars::TILE_SIZE, GlobalVars::TILE_HALF_SIZE, BLUE);
    }
}

void AdjustTimeScale()
{
    if (IsKeyDown(KEY_RIGHT))
        TIME_SCALE += 0.1f;
    else  if (IsKeyDown(KEY_LEFT) && TIME_SCALE > 0.1)
        TIME_SCALE -= 0.1f;

    // Show time
    std::string strTime = "Time scale: " + std::to_string(TIME_SCALE);
    char const* cTime = strTime.c_str();
    DrawText(cTime, 50, 10, 16, YELLOW);

    // Show FPS
    std::string strFPS = "FPS: " + std::to_string(1 / GetFrameTime());
    char const* cFPS = strFPS.c_str();
    DrawText(cFPS, 50, 30, 16, YELLOW);
}

// Game functionality
void RunGame()
{
    // Init
    //World world = World("resources/testMap.txt");
    World world = World("resources/WorldMap.txt");
    PathFinding pathfinding = PathFinding(world);
    //std::vector<Node>* path = pathfinding.AStar({ 64, 64 }, { 640, 640 });
    //return;

    std::map<Node*, NodeRecordAs> searchResult;
    std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare> open;
    std::vector<Node>* path = pathfinding.AStarDivided({ 64, 64 }, { 640, 640 }, searchResult, open);
    return;
    //std::vector<Node>* path = {};
    int frameCount = 0;

    // Gameloop
    while (!WindowShouldClose())
    {
        // Update entities
        float dt = GetFrameTime() * TIME_SCALE;
        world.Update(dt);
        //std::vector<Node>* path = pathfinding.AStar({ 64, 64 }, { 640, 640 });

        if (!path || path->empty())
        {
            //std::cout << "Path calculation FPS: " << 1 / GetFrameTime() << "\n";
            //path = pathfinding.AStarDivided({ 64, 64 }, { 640, 640 }, searchResult, open);
            //path = pathfinding.AStarDivided({ 1, 1 }, { 16, 16 }, searchResult, open);
            //path = pathfinding.AStar({ 0, 0 }, { 24, 24 });
            //std::cout << "Size: " << searchResult.size() << "\n";
            //std::cout << "Path calculation FPS: " << 1 / GetFrameTime() << "\n";
            frameCount++;
        }
        else
        {
            std::cout << "Frame count: " << frameCount << "\n";
            frameCount = 0;
            path->clear();
        }

        // Rendering
        BeginDrawing();
        ClearBackground(BLACK);
        world.Draw();

        //pathfinding.DrawGraph();
        if (path)
            DrawPath(path);

        AdjustTimeScale();
        std::cout << "Path calculation FPS: " << 1 / GetFrameTime() << "\n";

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