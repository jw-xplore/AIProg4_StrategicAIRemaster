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
    for (size_t y = 0; y < height; y++)
    {
        delete[] mapTerrain[y];
        delete[] discovered[y];
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

	// Read file
	if (file.is_open())
	{
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
        mapTerrain = new ETerrainType*[height];
        discovered = new bool* [height];

        GameDB::Database* db = GameDB::Database::Instance();

        for (int y = 0; y < height; y++)
        {
            mapTerrain[y] = new ETerrainType[width];
            discovered[y] = new bool[width];

            for (int x = 0; x < width; x++)
            {
                // Terrain types
                if (map[y][x] == db->terrains[ETerrainType::Grass].charIdentifier)
                    mapTerrain[y][x] = ETerrainType::Grass;

                if (map[y][x] == db->terrains[ETerrainType::Swamp].charIdentifier)
                    mapTerrain[y][x] = ETerrainType::Swamp;

                if (map[y][x] == db->terrains[ETerrainType::Water].charIdentifier)
                    mapTerrain[y][x] = ETerrainType::Water;

                if (map[y][x] == db->terrains[ETerrainType::Rock].charIdentifier)
                    mapTerrain[y][x] = ETerrainType::Rock;

                // Trees
                if (map[y][x] == db->terrains[ETerrainType::Trees].charIdentifier)
                {
                    mapTerrain[y][x] = ETerrainType::Grass;
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

	// Fail to read
	return false;
}

void World::Draw()
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
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

            switch (mapTerrain[y][x])
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