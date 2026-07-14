#pragma once
#include "raylib.h"
#include "Capital.h"
#include <vector>
#include "types.h"

class Worker;

namespace Capital
{
	struct CapitalAmounts;
}

enum EBuildingState
{
	Preplaced,
	InProgress,
	Finished
};

class Building
{
private:

public:
	EBuildingType type;
	Capital::CapitalAmounts storedCapital;
	Capital::CapitalAmounts reservedCapital = Capital::CapitalAmounts();
	float creationTarget;
	float creationCounter = 0;
	EBuildingState state = EBuildingState::Preplaced;

	Vector2 position;
	Color color;

	Building() {}
	Building(EBuildingType type, Vector2 startPos, EBuildingState state = EBuildingState::Preplaced);
	~Building() {}

	void StartBuilding();
	void FinishBuilding();
	void ProgressBuilding(float progress);
	Capital::CapitalAmounts GetAvailableCapital();
};

