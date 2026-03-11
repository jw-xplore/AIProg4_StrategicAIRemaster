#pragma once
#include "raylib.h"
#include "Capital.h"
#include <vector>

class Worker;

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
	EBuildingTypeCount
};

/*
struct CapitalReservation
{
	Worker* owner;
	Capital::CapitalAmounts reserved;
};
*/

class Building
{
public:
	EBuildingType type;
	Capital::CapitalAmounts storedCapital;
	float creationTarget;
	float creationCounter = 0;
	bool standing = false; // Building which is not standing is yet to be build
	//std::vector<CapitalReservation> capitalReservations;

	Vector2 position;
	Color color;

	Building() {}
	Building(EBuildingType type, Vector2 startPos);
	~Building() {}

	Capital::CapitalAmounts GetAvailableCapital();
};

