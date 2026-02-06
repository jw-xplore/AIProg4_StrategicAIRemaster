#pragma once
#include <raylib.h>
#include "Task.h"

enum ESubtaskState;
enum EBuildingType;

class Worker;
enum EWorkerRole;
struct TreesTile;
class Pickup;

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
		float delay;

		FellTreeSubtask(TreesTile* tile, float t) : treesTile(tile), delay(t) {}
		ESubtaskState Execute(Worker& worker, float dTime) override;
	};

	class PickupSubtask : public Subtask
	{
	public:
		Pickup* pickup;

		PickupSubtask(Pickup* pickup) : pickup(pickup) {}
		ESubtaskState Execute(Worker& worker, float dTime) override;
	};

	class TrainWorker : public Subtask
	{
	public:
		EWorkerRole role;
		float timer = 0;
		float delay;

		TrainWorker(EWorkerRole role, float t) : role(role), timer(0), delay(t) {}
		ESubtaskState Execute(Worker& worker, float dTime) override;
	};
};

