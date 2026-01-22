#pragma once
#include "raylib.h"
#include "Capital.h"

namespace Capital
{
	struct CapitalAmounts;
}

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
	Capital::CapitalAmounts storedCapital;

	Vector2 position;
	Color color;

	Building() {}
	Building(EBuildingType type, Vector2 startPos);
	~Building() {}
};

