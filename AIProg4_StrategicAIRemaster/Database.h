#pragma once
#include "Json.hpp"

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
		TrainSoldier,
		TrainCraftsman,
		EActionTrainingCount
	};

	enum EActionBuilding
	{
		BuildCoalMile,
		BuildForge,
		BuildSmelter,
		BuildTrainingCamp,
		EActionBuildingCount
	};

	//--------------------------------------------------
	// Database structures
	//--------------------------------------------------

	struct ActionCost
	{
		float time = 0;
		// TODO: change this into resource struct
		int wood = 0;
		int coal = 0;
		int ironOre = 0;
		int ironBar = 0;
		int sword = 0;
	};

	struct Terrain
	{
		//ETerrainType type;
		//const char* name;
		char charIdentifier;
		float cost;
	};

	//--------------------------------------------------
	// Main storage definition
	//--------------------------------------------------
	class Database
	{
	private:
		static Database* instance;

	public:
		Terrain* terrains;
		ActionCost* actionCostsResources;
		ActionCost* actionCostsTraining;
		ActionCost* actionCostsBuilding;

		Database();
		~Database();

		static Database* Instance()
		{
			if (!instance)
				instance = new Database();

			return instance;
		}

		ActionCost* DefineActionCosts(nlohmann::ordered_json& j, const char* section, int size);
	};
}

