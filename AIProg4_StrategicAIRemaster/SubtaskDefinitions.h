#pragma once
#include <raylib.h>
#include "Task.h"

enum ESubtaskState;
enum EBuildingType;

class Worker;
struct TreesTile;

namespace Capital
{
	enum ECapitalType;
}

namespace SubtaskDefinitions
{
	// General
	//ESubtaskState FindMaterial(Worker& worker, Capital::ECapitalType);
	/*
	ESubtaskState MoveTo(Worker& worker, float dTime);
	ESubtaskState FellTree(Worker& worker, float dTime);
	ESubtaskState Train(Worker& worker, float dTime);
	ESubtaskState Pickup(Worker& worker, float dTime);
	ESubtaskState Deliver(Worker& worker, float dTime);

	// Role task
	ESubtaskState MakeCoal(Worker& worker, float dTime); // Coal miner
	ESubtaskState MakeIronBar(Worker& worker, float dTime); // Smelter operator
	ESubtaskState MakeSword(Worker& worker, float dTime); // Arm smith
	ESubtaskState Build(Worker& worker, float dTime, EBuildingType type); // Builder
	*/

	class MoveToSubtask : public Subtask
	{
	public:
		Vector2 position;
		bool pathAssigned = false;

		MoveToSubtask(Vector2 pos) : position(pos) {}
		ESubtaskState Execute(Worker& worker, float dTime) override;
	};

	class FellTreeSubtask : public Subtask
	{
	public:
		TreesTile* treesTile;
		float timer = 0;
		float time;

		FellTreeSubtask(TreesTile* tile, float t) : treesTile(tile), time(t) {}
		ESubtaskState Execute(Worker& worker, float dTime) override;
	};
};

