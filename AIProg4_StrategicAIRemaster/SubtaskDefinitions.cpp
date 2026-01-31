#include "SubtaskDefinitions.h"
#include "Worker.h"
#include "PathFinding.h"
#include "SystemsHolder.h"
#include "Commander.h"
#include "Constants.h"
#include "World.h"
#include "EntityManager.h"
#include "Pickup.h"
#include <vector>

using namespace SubtaskDefinitions;

ESubtaskState MoveToSubtask::Execute(Worker& worker, float dTime)
{
	// DEBUG teleport
	worker.position = position;
	return ESubtaskState::Finnished;

	// Find path
	if (!pathAssigned)
	{
		// Clear previous path
		if (!worker.path.empty())
			worker.path.clear();

		// Find 
		std::map<Node*, NodeRecordAs>* searchResult = SystemsHolder::GetInstance()->commander->searchResult;
		std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare>* open = SystemsHolder::GetInstance()->commander->open;
		std::vector<Node>* path = SystemsHolder::GetInstance()->pathfinding->AStarDivided(worker.position, position, *searchResult, *open);

		// Continue with path search
		if (path->empty())
			ESubtaskState::Running;

		// Assign path
		for (size_t p = 0; p < path->size(); p++)
		{
			float x = path->at(p).x * GlobalVars::TILE_SIZE;
			float y = path->at(p).y * GlobalVars::TILE_SIZE;
			worker.path.push_back({ x, y });
		}

		pathAssigned = true;
	}

	// Move to point
	if (worker.path.empty())
		return ESubtaskState::Finnished;
	
	return ESubtaskState::Running;
}

ESubtaskState FellTreeSubtask::Execute(Worker& worker, float dTime)
{
	if (timer >= time)
	{
		Vector2 pos = { treesTile->x * GlobalVars::TILE_SIZE, treesTile->y * GlobalVars::TILE_SIZE };

		// Done
		treesTile->FellTree();
		Pickup felledTree = Pickup(Capital::ECapitalType::Tree, pos);
		SystemsHolder::GetInstance()->entityMananger->pickups->push_back(felledTree);
		return ESubtaskState::Finnished;
	}

	// Run 
	timer++;
	return ESubtaskState::Running;
}