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
	GoalStep trainScout = GoalStep("Train scout",
		{},
		TaskAttribute(ETaskAttributeCategory::Worker, EWorkerRole::Scout, 1, nullptr)
	);
	
}

Commander::~Commander()
{

}

void Commander::DefineAvailableTasks()
{
	// Material gathering
	availableSteps.push_back(new GoalStep(
		"Fell tree",
		{},
		TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Tree, 1, nullptr)
	));

	/*
	availableSteps.push_back(new GoalStep(
		"Deliver wood",
		{
			TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Tree, 1, nullptr)
		},
		TaskAttribute(ETaskAttributeCategory::Worker, EWorkerRole::Scout, 1, nullptr)
	));
	*/

	//--------------

	// Goals final tasks
	GoalStep* trainScout = new GoalStep(
		"Train scout",
		{},
		TaskAttribute(ETaskAttributeCategory::Worker, EWorkerRole::Scout, 1, nullptr)
	);

	availableSteps.push_back(trainScout);

	GoalStep* deliverWood = new GoalStep(
		"Deliver wood",
		{
			TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Tree, 1, nullptr)
		},
		TaskAttribute(ETaskAttributeCategory::Worker, EWorkerRole::Scout, 1, nullptr)
	);

	availableSteps.push_back(deliverWood);

	// Goals definition
	goals.push_back(new Goal(*deliverWood, availableSteps));
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

	if (goals[0]->finalStep)
		goals[0]->DebugDraw(*goals[0]->finalStep, 0, 0);
}

Worker* Commander::FindFreeWorker(EWorkerRole roleConstrain)
{
	for (Worker& worker : entityManager->workers)
	{
		// Get free worker
		if (workerTaskMap[&worker] == nullptr)
		{
			if (roleConstrain != EWorkerRole::General && worker.role != roleConstrain)
				continue;

			return &worker;
		}
	}

	return nullptr;
}

void Commander::AssignTask(Worker* worker, Task* task)
{
	workerTaskMap[worker] = task;
	task->assignee = worker;

	//*goals[currentGoal]->potentialCapital += task->rewardCapital;
}