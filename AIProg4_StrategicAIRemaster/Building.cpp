#include "Building.h"

Building::Building(EBuildingType type, Vector2 startPos)
{
	position = startPos;
	standing = false;

	switch (type)
	{
	case EBuildingType::CoalMile: color = PURPLE; break;
	case EBuildingType::ArsmithsForge: color = RED; break;
	case EBuildingType::Smelter: color = BLUE; break;
	case EBuildingType::TrainingCamp: color = ORANGE; break;
	}
}