#include "Database.h"
#include <fstream>
#include <iostream>
#include <string>

#include "World.h"

using namespace GameDB;

Database* Database::instance;

Database::Database()
{
	// Read json
	std::ifstream file("resources/db.json");
	nlohmann::ordered_json jsonRes = nlohmann::ordered_json::parse(file);
	file.close();

	// Store data--------------------------------

	// Terrains
	terrains = new Terrain[ETerrainType::ETerrainTypeCount];
	for (size_t i = 0; i < ETerrainType::ETerrainTypeCount; i++)
	{
		terrains[i].charIdentifier = jsonRes["terrain"][i]["char"].get<std::string>()[0];
		float travelSpeed = jsonRes["terrain"][i]["travelSpeed"];

		if (travelSpeed > 0)
			terrains[i].cost = 1 / travelSpeed;
		else
			terrains[i].cost = travelSpeed;
	}

	// Costs
	actionCostsResources = DefineActionCosts(jsonRes, "resourceActionCost", EActionResource::EActionResourceCount);
	actionCostsTraining = DefineActionCosts(jsonRes, "trainingActionCost", EActionTraining::EActionTrainingCount);
	actionCostsBuilding = DefineActionCosts(jsonRes, "buildingActionCost", EActionBuilding::EActionBuildingCount);
}

Database::~Database()
{
	delete[] terrains;
	delete[] actionCostsResources;
	delete[] actionCostsTraining;
	delete[] actionCostsBuilding;
}

ActionCost* Database::DefineActionCosts(nlohmann::ordered_json& j, const char* section, int size)
{
	ActionCost* costs = new ActionCost[size];
	
	for (size_t i = 0; i < size; i++)
	{
		costs[i].time = j[section][i].at("time");
		costs[i].wood = j[section][i].at("trees");
		costs[i].coal = j[section][i].at("coal");
		costs[i].ironOre = j[section][i].at("ironOre");
		costs[i].ironBar = j[section][i].at("ironBar");
		costs[i].sword = j[section][i].at("swords");
	}

	return costs;
}