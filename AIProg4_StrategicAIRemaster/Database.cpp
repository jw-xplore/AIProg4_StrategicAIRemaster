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

	terrains[ETerrainType::Grass].charIdentifier = jsonRes["terrain"][0]["char"].get<std::string>()[0];
	terrains[ETerrainType::Grass].cost = jsonRes["terrain"][0]["cost"];

	terrains[ETerrainType::Swamp].charIdentifier = jsonRes["terrain"][1]["char"].get<std::string>()[0];
	terrains[ETerrainType::Swamp].cost = jsonRes["terrain"][1]["cost"];

	terrains[ETerrainType::Rock].charIdentifier = jsonRes["terrain"][2]["char"].get<std::string>()[0];
	terrains[ETerrainType::Rock].cost = jsonRes["terrain"][2]["cost"];

	terrains[ETerrainType::Water].charIdentifier = jsonRes["terrain"][3]["char"].get<std::string>()[0];
	terrains[ETerrainType::Water].cost = jsonRes["terrain"][3]["cost"];

	terrains[ETerrainType::Trees].charIdentifier = jsonRes["terrain"][4]["char"].get<std::string>()[0];
	terrains[ETerrainType::Trees].cost = jsonRes["terrain"][4]["cost"];

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