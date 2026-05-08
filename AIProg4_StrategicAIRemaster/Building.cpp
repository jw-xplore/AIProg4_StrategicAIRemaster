#include "Building.h"
#include "Database.h"

Building::Building(EBuildingType type, Vector2 startPos)
{
	position = startPos;
	state = EBuildingState::Preplaced;

	this->type = type;
	color = GREEN;
	creationTarget = GameDB::Database::Instance()->actionCostsBuilding[type].time;
}

void Building::FinishBuilding()
{
	// Substract resources
	storedCapital -= GameDB::Database::Instance()->actionCostsBuilding[type].capital;

	// Change building
	creationCounter = creationTarget;
	ProgressBuilding(0);
}

void Building::ProgressBuilding(float progress)
{
	creationCounter += progress;
	state = EBuildingState::InProgress;
	color = BLACK;

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

	state = EBuildingState::Finished;
}

Capital::CapitalAmounts Building::GetAvailableCapital()
{
	Capital::CapitalAmounts availabe = storedCapital;
	availabe -= reservedCapital;

	return availabe;
}