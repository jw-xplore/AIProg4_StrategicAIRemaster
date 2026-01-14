#include "World.h"
#include <raymath.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "Constants.h"
#include "Database.h"

//-----------------------------------------------
// World class definition
//-----------------------------------------------

World::World(const char* path)
{
    //entityManager = entManager;

    // World setup
    LoadMap(path);

    // Discovered parts
    undiscoveredCount = worldSize;
}

World::~World()
{
    // Clear map
    for (size_t x = 0; x < width; x++)
    {
        delete[] mapTerrain[x];
        delete[] discovered[x];
    }

    delete[] mapTerrain;
    delete[] discovered;

    // Clear trees
    for (size_t i = 0; i < treeTiles.size(); i++)
    {
        delete[] treeTiles[i].treePositions;
    }

    treeTiles.clear();
}

void World::Update(float dTime)
{
    /*
    // Discover world
    Vector2 pos;

    for (int i = 0; i < entityManager->workers.size(); i++)
    {
        pos = entityManager->workers[i]->position;
        pos.x = pos.x / GlobalVars::TILE_SIZE;
        pos.y = pos.y / GlobalVars::TILE_SIZE;

        i = (int)pos.y * width + (int)pos.x;

        // Discover
        if (!discovered[i])
            undiscoveredCount--;

        discovered[i] = true;
    }
    */
}

/*
Load map data and create world setup
*/
bool World::LoadMap(const char* path)
{
    // Load file
	std::ifstream file;
	file.open(path);

	// Define array
	int lines = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
	height = lines + 1;

    const char** map = new const char*[height];

	// Restart line reading
	int currentLine = 0;
	file.clear();
	file.seekg(0);

	// Fail to read?
    if (!file.is_open())
        return false;

    // Read file
    std::string line;

    // Read all the lines
    while (std::getline(file, line))
    {
        char* cstr = new char[line.size()];
        for (size_t i = 0; i < line.size(); ++i)
        {
            cstr[i] = line[i];
        }

        map[currentLine] = cstr;

        currentLine++;
        if (this->width == 0)
            this->width = line.size();
    }

    // Define resources
    worldSize = width * height;
    mapTerrain = new ETerrainType*[width];
    discovered = new bool* [width];

    GameDB::Database* db = GameDB::Database::Instance();

    for (size_t x = 0; x < height; x++)
    {
        mapTerrain[x] = new ETerrainType[height];
        discovered[x] = new bool[height];

        for (size_t y = 0; y < height; y++)
        {
            // NOTE: map[y][x]
            // Temporary map data are stored as Y first, X second, due to file reading getting the height first.
            // This is fixed for actual map data, which can be naturaly accessed as [x][y]

            // Terrain types
            if (map[y][x] == db->terrains[ETerrainType::Grass].charIdentifier)
                mapTerrain[x][y] = ETerrainType::Grass;

            if (map[y][x] == db->terrains[ETerrainType::Swamp].charIdentifier)
                mapTerrain[x][y] = ETerrainType::Swamp;

            if (map[y][x] == db->terrains[ETerrainType::Water].charIdentifier)
                mapTerrain[x][y] = ETerrainType::Water;

            if (map[y][x] == db->terrains[ETerrainType::Rock].charIdentifier)
                mapTerrain[x][y] = ETerrainType::Rock;

            // Trees
            if (map[y][x] == db->terrains[ETerrainType::Trees].charIdentifier)
            {
                mapTerrain[x][y] = ETerrainType::Grass;
                treeTiles.push_back(TreesTile(x, y, 5, GlobalVars::TILE_HALF_SIZE));
            }
        }
    }

    // Clearup
    for (int y = 0; y < height; y++)
    {
        delete[] map[y];
    }

    delete[] map;

    // Stop reading
	file.close();
    return true;
}

void World::Draw()
{
    for (size_t x = 0; x < height; x++)
    {
        for (size_t y = 0; y < height; y++)
        {
            // Show fog 
            /*
            if (!discovered[i])
            {
                DrawTexture(*fogTexture, x * GlobalVars::TILE_SIZE, y * GlobalVars::TILE_SIZE, WHITE);
                continue;
            }
            */

            // Show terrain
            Color col = cGrass;

            switch (mapTerrain[x][y])
            {
            case ETerrainType::Grass: col = cGrass; break;
            case ETerrainType::Swamp: col = cSwamp; break;
            case ETerrainType::Water: col = cWater; break;
            case ETerrainType::Rock: col = cRock; break;
            }

            DrawRectangle(x * GlobalVars::TILE_SIZE, y * GlobalVars::TILE_SIZE, GlobalVars::TILE_SIZE, GlobalVars::TILE_SIZE, col);
        }
    }

    // Show trees
    for (size_t i = 0; i < treeTiles.size(); i++)
    {
        for (size_t t = 0; t < treeTiles[i].amount; t++)
        {
            Vector2 pos = { 
                treeTiles[i].x * GlobalVars::TILE_SIZE + treeTiles[i].treePositions[t].x,
                treeTiles[i].y * GlobalVars::TILE_SIZE + treeTiles[i].treePositions[t].y 
            };

            DrawCircle(pos.x, pos.y, 2, BROWN);
        }
    }
}