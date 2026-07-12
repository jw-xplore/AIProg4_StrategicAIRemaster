#include "SubtaskDefinitions.h"
#include "Worker.h"
#include "PathFinding.h"
#include "SystemsHolder.h"
#include "Commander.h"
#include "Constants.h"
#include "World.h"
#include "EntityManager.h"
#include "Pickup.h"
#include "Building.h"
#include <vector>
#include <iostream>

using namespace SubtaskDefinitions;

ESubtaskState MoveToSubtask::Execute(Worker& worker, float dTime)
{
	// Find path
	if (!pathAssigned)
	{
		// Clear previous path
		worker.ClearPath();

		// Find 
		std::map<Node*, NodeRecordAs>* searchResult = SystemsHolder::GetInstance()->commander->searchResult;
		std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare>* open = SystemsHolder::GetInstance()->commander->open;
		std::vector<Node>* path = SystemsHolder::GetInstance()->pathfinding->AStarDivided(worker.position, position, *searchResult, *open);

		// Continue with path search
		if (path->empty())
			ESubtaskState::Running;

		// Assign path
		std::vector<Vector2> pathPositions;

		for (size_t p = 0; p < path->size(); p++)
		{
			float x = path->at(p).x * GlobalVars::TILE_SIZE;
			float y = path->at(p).y * GlobalVars::TILE_SIZE;
			pathPositions.push_back({ x, y });
		}

		worker.SetNewPath(pathPositions);

		pathAssigned = true;
	}

	// Move to point
	if (worker.PathTargetReached())
		return ESubtaskState::Finnished;

	return ESubtaskState::Running;
}

FellTreeSubtask::FellTreeSubtask(TreesTile* tile, float t) :
	treesTile(tile), delay(t) 
{
	ogX = tile->x;
	ogY = tile->y;
}

ESubtaskState FellTreeSubtask::Execute(Worker& worker, float dTime)
{
	if (timer >= delay)
	{
		Vector2 pos = worker.position; // Hot fix for faulty tile (11,37)

		// Done
		treesTile->FellTree();
		Pickup* felledTree = new Pickup(Capital::ECapitalType::Tree, pos);
		SystemsHolder::GetInstance()->entityMananger->pickups.push_back(felledTree);

		// Remove empty tree tile
		if (treesTile->amount <= 0)
		{
			SystemsHolder::GetInstance()->world->RemoveTreeTile(treesTile);
			return ESubtaskState::Finnished;
		}

		treesTile->reservations--;
		if (treesTile->reservations < 0)
		{
			std::string err = "negative reservation!";
			throw std::runtime_error(err);
		}

		return ESubtaskState::Finnished;
	}

	// Run 
	timer += dTime;
	return ESubtaskState::Running;
}

ESubtaskState PickupSubtask::Execute(Worker& worker, float dTime)
{
	if (worker.carriedMaterial != Capital::ECapitalType::None)
		return ESubtaskState::Canceled;

	worker.carriedMaterial = pickup->type;
	SystemsHolder::GetInstance()->entityMananger->RemovePickup(pickup);
	return ESubtaskState::Finnished;
}

ESubtaskState PickupFromBuildingSubtask::Execute(Worker& worker, float dTime)
{
	if (worker.carriedMaterial != Capital::ECapitalType::None)
		return ESubtaskState::Canceled;

	worker.carriedMaterial = type;
	building->storedCapital.amounts[type] -= 1;
	building->reservedCapital.amounts[type] -= 1;

	return ESubtaskState::Finnished;
}

ESubtaskState DropItemSubtask::Execute(Worker& worker, float dTime)
{
	if (worker.carriedMaterial == Capital::ECapitalType::None)
		return ESubtaskState::Canceled;

	targetBuilding->storedCapital.amounts[worker.carriedMaterial]++;
	worker.carriedMaterial = Capital::ECapitalType::None;
	return ESubtaskState::Finnished;
}

ESubtaskState TrainWorker::Execute(Worker& worker, float dTime)
{
	//delay = 1;
	if (timer >= delay)
	{
		// Done
		worker.role = role;

		// Change coloring
		switch (role)
		{
			case EWorkerRole::Builder: worker.coloring = YELLOW; break;
		}

		worker.trainedRole = EWorkerRole::None;
		return ESubtaskState::Finnished;
	}

	// Run 
	timer += dTime;
	worker.coloring = BLACK;
	return ESubtaskState::Running;
}

TrainSoldier::TrainSoldier(Building* target, float t) : targetBuilding(target), timer(0), delay(t)
{
	targetBuilding->reservedCapital.amounts[Capital::ECapitalType::Sword]++;
}

ESubtaskState TrainSoldier::Execute(Worker& worker, float dTime)
{
	if (timer >= delay)
	{
		// Done
		worker.role = EWorkerRole::Soldier;

		// Change coloring
		worker.coloring = LIME;

		targetBuilding->storedCapital.amounts[Capital::ECapitalType::Sword]--;
		targetBuilding->reservedCapital.amounts[Capital::ECapitalType::Sword]--;

		worker.trainedRole = EWorkerRole::None;
		return ESubtaskState::Finnished;
	}

	// Run 
	timer += dTime;
	worker.coloring = BLACK;
	return ESubtaskState::Running;
}

ESubtaskState CreateBuilding::Execute(Worker& worker, float dTime)
{
	if (!started)
	{
		building->StartBuilding();
		started = true;
	}

	if (timer >= delay)
	{
		// Done
		building->FinishBuilding();
		return ESubtaskState::Finnished;
	}

	// Run 
	timer += dTime;
	return ESubtaskState::Running;
}

ESubtaskState CreateItem::Execute(Worker& worker, float dTime)
{
	if (!started)
	{
		building->reservedCapital += cost;
		started = true;
	}

	if (timer >= delay)
	{
		// Done
		building->reservedCapital -= cost;
		building->storedCapital -= cost;
		building->storedCapital.amounts[gainItem] += 1;
		return ESubtaskState::Finnished;
	}

	// Run 
	timer += dTime;
	return ESubtaskState::Running;
}