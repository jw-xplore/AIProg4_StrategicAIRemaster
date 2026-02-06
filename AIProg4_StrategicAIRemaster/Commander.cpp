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

	// Active tasks list setup
	int workersCount = entityManager->workers->size();
	activeTasks.reserve(workersCount);
	for (size_t i = 0; i < workersCount; i++)
	{
		activeTasks.push_back(nullptr);
	}

	// Pre-assign starting buildings
	neededBuildings.push(EBuildingType::CoalMile);
	/*
	neededBuildings.push(EBuildingType::Smelter);
	neededBuildings.push(EBuildingType::ArsmithsForge);
	neededBuildings.push(EBuildingType::TrainingCamp);
	*/

	// Test assign task
	TreesTile* treesTile = &systemsHolder->world->treeTiles[0];
	Vector2 treeTilePos = { treesTile->x * GlobalVars::TILE_SIZE, treesTile->y * GlobalVars::TILE_SIZE };

	Task* task = new Task({
		new SubtaskDefinitions::MoveToSubtask(treeTilePos),
		new SubtaskDefinitions::FellTreeSubtask(treesTile, 10),
		});

	//Worker w = (*entityManager->workers)[0];
	Worker* testWorker = &(*entityManager->workers)[0];
	task->assignee = testWorker;
	activeTasks.push_back(task);

	// Test building
	Building* testBuilding = new Building(neededBuildings.front(), { 10 * GlobalVars::TILE_SIZE, 10 * GlobalVars::TILE_SIZE });
	entityManager->buildings->push_back(*testBuilding);
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