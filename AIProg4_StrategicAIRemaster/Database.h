#pragma once
#include "Json.hpp"
#include "Capital.h"

namespace GameDB
{
	//--------------------------------------------------
	// Key enums
	//--------------------------------------------------

	enum EActionResource
	{
		FellTree,
		MakeCoal,
		MakeIronBar,
		MakeSword,
		EActionResourceCount
	};

	enum EActionTraining
	{
		TrainScout,
		TrainBuilder,
		TrainSoldier,
		TrainCraftsman,
		EActionTrainingCount
	};

	//--------------------------------------------------
	// Database structures
	//--------------------------------------------------

	struct TerrainData
	{
		char charIdentifier;
		float cost; // cost of traversing tile, value use directly in pathfinding
	};

	//--------------------------------------------------
	// Main storage definition
	//--------------------------------------------------
	class Database
	{
	private:
		static Database* instance;

	public:
		float tileMeterSize;
		int startingPopulation;
		int ironOreAmount;

		TerrainData* terrains;
		Capital::ActionCost* actionCostsResources;
		Capital::ActionCost* actionCostsTraining;
		Capital::ActionCost* actionCostsBuilding;

		// All data loaded on contructor
		Database();
		~Database();

		static Database* Instance()
		{
			if (!instance)
				instance = new Database();

			return instance;
		}

		Capital::ActionCost* DefineActionCosts(nlohmann::ordered_json& j, const char* section, int size);
	};
}

