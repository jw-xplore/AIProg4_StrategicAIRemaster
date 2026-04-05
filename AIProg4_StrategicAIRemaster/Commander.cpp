#include "Commander.h"
#include "Worker.h"
#include "Building.h"
#include "SystemsHolder.h"
#include "EntityManager.h"
#include "GoalSystem.h"
#include "SubtaskDefinitions.h"
#include "World.h"
#include "Constants.h"
#include "PathFinding.h"
#include "Database.h"
#include <iostream>

Commander::Commander()
{
	searchResult = new std::map<Node*, NodeRecordAs>();
	open = new std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare>();

	SystemsHolder* systemsHolder = SystemsHolder::GetInstance();
	systemsHolder->commander = this;
	entityManager = SystemsHolder::GetInstance()->entityMananger;

	DefineAvailableTasks();
}

Commander::~Commander()
{

}

void DefineAvailableTasks()
{
	WorkerTasks::TrainForRoleTask(nullptr, EWorkerRole::Scout);
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

	DebugDraw();
}

void Commander::UpdatePlan()
{

	for (Worker& worker : entityManager->workers)
	{
	}
}

void Commander::DebugDraw()
{
	/*
	for (auto& wTask : workerTaskMap)
	{
		if (!wTask.second)
			continue;

		// Task debug
		std::string str = wTask.second->name;
		DrawText(str.c_str(), wTask.first->position.x, wTask.first->position.y, 5, WHITE);
	}
	*/

	
}

Worker* Commander::FindFreeWorker(EWorkerRole roleConstrain)
{
	for (Worker& worker : entityManager->workers)
	{
		/*
		// Get free worker
		if (workerTaskMap[&worker] == nullptr)
		{
			if (roleConstrain != EWorkerRole::General && worker.role != roleConstrain)
				continue;

			return &worker;
		}
		*/
	}

	return nullptr;
}