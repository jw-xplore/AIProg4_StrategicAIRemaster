#include "Building.h"

Building::Building(EBuildingType type, Vector2 startPos)
{
	switch (type)
	{
	case EBuildingType::CoalMile: color = BLACK; break;
	case EBuildingType::ArsmithsForge: color = RED; break;
	case EBuildingType::Smelter: color = BLUE; break;
	case EBuildingType::TrainingCamp: color = ORANGE; break;
	}
}