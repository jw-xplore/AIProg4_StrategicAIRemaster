#pragma once

enum ESubtaskState;
enum EBuildingType;

class Worker;

namespace Capital
{
	enum ECapitalType;
}

namespace SubtaskDefinitions
{
	// General
	ESubtaskState FindMaterial(Worker& worker, Capital::ECapitalType);

	// Role task
	ESubtaskState MakeCoal(Worker& worker, float dTime); // Coal miner
	ESubtaskState MakeIronBar(Worker& worker, float dTime); // Smelter operator
	ESubtaskState MakeSword(Worker& worker, float dTime); // Arm smith
	ESubtaskState Build(Worker& worker, float dTime, EBuildingType type); // Builder
};

