#pragma once
#include "raylib.h"

enum EBuildingType
{
	CoalMile,
	ArsmithsForge,
	Smelter,
	TrainingCamp,
};

class Building
{
public:
	EBuildingType type;
	Vector2 position;
	Color color;

	Building() {}
	Building(EBuildingType type, Vector2 startPos);
	~Building() {}
};

