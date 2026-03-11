#include "Commander.h"
#include "Worker.h"
#include "Building.h"
#include "SystemsHolder.h"
#include "EntityManager.h"
#include "Task.h"
#include "SubtaskDefinitions.h"
#include "World.h"
#include "Constants.h"
#include "PathFinding.h"

Commander::Commander()
{
	searchResult = new std::map<Node*, NodeRecordAs>();
	open = new std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare>();

	SystemsHolder* systemsHolder = SystemsHolder::GetInstance();
	systemsHolder->commander = this;
	entityManager = SystemsHolder::GetInstance()->entityMananger;

	// Test building
	Vector2 testBuildingPosition = { 15 * GlobalVars::TILE_SIZE, 14 * GlobalVars::TILE_SIZE };
	Building* coalMile = new Building(EBuildingType::CoalMile, testBuildingPosition);
	entityManager->buildings.push_back(coalMile);

	// Test worker action

}

Commander::~Commander()
{

}

void Commander::Update(float dTime)
{
	// Assigne new tasks
	replanTimer += dTime;

	if (replanTimer >= replanDelay)
	{
		UpdatePlan();
		replanTimer = 0;
	}

	// Update tasks
	for (size_t i = 0; i < activeTasks.size(); i++)
	{
		if (activeTasks[i])
			activeTasks[i]->Update(dTime);
	}
}

void Commander::UpdatePlan()
{
	// Run current goal decision tree
	size_t workersSize = entityManager->workers->size();

	for (size_t i = 0; i < workersSize; i++)
	{
		
	}

	// Add to pending tasks

	// Distribute pending tasks to active if workers are available
}

//--------------------------------------------------------------
// Commander goals
//--------------------------------------------------------------

namespace CommanderGoals
{
	void WarmupGoal::Setup()
	{
		// Create scouts and builder
		//commander->activeTasks

		// Send others for wood
	}

	bool WarmupGoal::Complete()
	{
		return true;
	}
}