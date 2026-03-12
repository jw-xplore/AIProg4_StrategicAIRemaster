#include "Building.h"
#include "Database.h"

Building::Building(EBuildingType type, Vector2 startPos)
{
	position = startPos;
	standing = false;

	this->type = type;
	color = GREEN;
	creationTarget = GameDB::Database::Instance()->actionCostsBuilding[type].time;
}

void Building::FinishBuilding()
{
	creationCounter = creationTarget;
	ProgressBuilding(0);
}

void Building::ProgressBuilding(float progress)
{
	creationCounter += progress;
	if (creationCounter < creationTarget)
		return;

	// Create building
	switch (type)
	{
	case EBuildingType::CoalMile: color = PURPLE; break;
	case EBuildingType::ArsmithsForge: color = RED; break;
	case EBuildingType::Smelter: color = BLUE; break;
	case EBuildingType::TrainingCamp: color = ORANGE; break;
	}

	standing = true;
}