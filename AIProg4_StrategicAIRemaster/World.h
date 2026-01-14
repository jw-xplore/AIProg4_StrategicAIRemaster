#include <raylib.h>
#include <vector>

class ComponentsManager;
class EntityManager;

//-----------------------------------------------
// World data
//-----------------------------------------------

enum ETerrainType
{
    Grass,
    Swamp,
    Rock,
    Water,
    Trees,
    ETerrainTypeCount
};

struct Vector2Int
{
    int x, y;
};

struct TreesTile
{
    int x, y, amount;
    Vector2Int* treePositions;

    TreesTile(int tileX, int tileY, int count, int range)
    {
        x = tileX;
        y = tileY;
        amount = count;

        // Setup tree positions
        treePositions = new Vector2Int[count];

        for (size_t i = 0; i < count; i++)
        {
            treePositions[i].x = GetRandomValue(-range, range);
            treePositions[i].y = GetRandomValue(-range, range);
        }
    }
};

//-----------------------------------------------
// World class definition
//-----------------------------------------------
class World
{
public:
    int worldSize, width, height;

    // Map data
    int undiscoveredCount;
    ETerrainType** mapTerrain;
    std::vector<TreesTile> treeTiles;
    bool** discovered; // Blocks discovered by NCPs, Undiscovered will be covered in fog

    // Colors
    const Color cGrass = { 70, 100, 52, 255 };
    const Color cSwamp = { 37, 53, 18, 255 };
    const Color cRock = { 82, 87, 89, 255 };
    const Color cWater = { 56, 134, 219, 255 };

    EntityManager* entityManager;

    World(const char* path);
    ~World();

    bool LoadMap(const char* path);
    void Update(float dTime);
    void Draw();
};