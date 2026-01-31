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

	// Worker type queues
	freeWorkers.reserve(EWorkerRole::EWorkerRoleCount);
	int workerCount = systemsHolder->entityMananger->workers->size();

	for (size_t i = 0; i < EWorkerRole::EWorkerRoleCount; i++)
	{
		freeWorkers.push_back({});
	}

	for (Worker& worker : *systemsHolder->entityMananger->workers)
	{
		freeWorkers[worker.role].push(&worker);
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

	Worker* testWorker = freeWorkers[EWorkerRole::General].front();
	task->assignee = testWorker;
	activeTasks.push_back(task);

}

Commander::~Commander()
{

}

void Commander::Update(float dTime)
{
	// Assigne new tasks

	// Handle pending tasks
	//pendingTasks.front();
	//pendingTasks.pop();

	// Update tasks
	for (size_t i = 0; i < activeTasks.size(); i++)
	{
		activeTasks[i]->Update(dTime);
	}
}
