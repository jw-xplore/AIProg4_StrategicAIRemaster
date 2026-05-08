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

	// Preplace buildings 
	entityManager->AddBuilding(new Building(EBuildingType::CoalMile, {160, 160}));
	entityManager->AddBuilding(new Building(EBuildingType::Smelter, { 120, 100 }));

	DefineAvailableTasks();
}

Commander::~Commander()
{

}

void Commander::DefineAvailableTasks()
{
	GameDB::Database* db = GameDB::Database::Instance();
	//db->actionCostsResources[EActionResource::MakeCoal];

	// Buildings
	Building* coalMile = entityManager->FindBuildingOfType(EBuildingType::CoalMile);
	Building* smelter = entityManager->FindBuildingOfType(EBuildingType::Smelter);

	// Material gathering
	GoalStep* cutWood = new GoalStep(
		"Fell tree",
		{},
		TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Tree, 1, nullptr),
		[*this](Worker* worker) { return  WorkerTasks::FellTreeTask(worker); }
	);

	GoalStep* createCoal = new GoalStep(
		"Cr coal",
		{
			// TODO: Determine which building is required
			// TODO: Get unit numbers from database
			TaskAttribute(ETaskAttributeCategory::Building, EBuildingType::CoalMile, 1, nullptr, true),
			TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Tree, db->actionCostsResources[GameDB::EActionResource::MakeCoal].capital.amounts[Capital::ECapitalType::Tree], coalMile),
			TaskAttribute(ETaskAttributeCategory::Worker, EWorkerRole::CoalMiner, 1, nullptr)
		},
		TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Coal, 1, coalMile),

		// TODO: Add creating of coal by coal miler
		[*this, db](Worker* worker) { return  WorkerTasks::CreateItemTask(
			worker,
			entityManager->FindBuildingOfType(EBuildingType::CoalMile), 
			db->actionCostsResources[GameDB::EActionResource::MakeCoal],
			Capital::ECapitalType::Coal
		); 
		}
	);

	availableSteps.push_back(cutWood);
	availableSteps.push_back(createCoal);

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
	for (size_t i = 0; i < EWorkerRole::EWorkerRoleCount; i++)
	{
		// Soldier will have custon task
		if (i == EWorkerRole::Soldier)
			continue;

		std::string name = "";
		switch (i)
		{
		case EWorkerRole::Scout: name = "Scout"; break;
		case EWorkerRole::Builder: name = "Builder"; break;
		case EWorkerRole::CoalMiner: name = "CoalM"; break;
		case EWorkerRole::SmelterOperator: name = "Smelt"; break;
		case EWorkerRole::ArmsSmith: name = "Smith"; break;
		}

		GoalStep* train = new GoalStep(
			"Train " + name,
			{},
			TaskAttribute(ETaskAttributeCategory::Worker, static_cast<EWorkerRole>(i), 1, nullptr),
			[*this, i](Worker* worker) { return WorkerTasks::TrainForRoleTask(worker, static_cast<EWorkerRole>(i)); }
		);

		availableSteps.push_back(train);
	}

	// Define delivery tasks
	GoalStep* deliverItem = new GoalStep(
		"Deliver",
		{
			//TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Tree, 1, nullptr)
		},
		TaskAttribute(ETaskAttributeCategory::Capital, -1, 0, nullptr),
		nullptr
		/*
		[*this](Worker* worker) {
			return WorkerTasks::DeliverItemTask(worker, Capital::ECapitalType::Tree, nullptr);
		}
		*/
	);

	deliverItem->variableInOut = true;
	deliverItem->isDelivery = true;

	availableSteps.push_back(deliverItem);

	// Buidling tasks 
	GoalStep* buildCoalMile = new GoalStep(
		"Build coal mile",
		{
			TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Tree, 10, entityManager->FindBuildingOfType(EBuildingType::CoalMile)),
			TaskAttribute(ETaskAttributeCategory::Worker, EWorkerRole::Builder, 1, nullptr)
		},
		TaskAttribute(ETaskAttributeCategory::Building, EBuildingType::CoalMile, 1, nullptr),
		[*this](Worker* worker) { return WorkerTasks::BuildTask(worker, entityManager->FindBuildingOfType(EBuildingType::CoalMile)); }
	);

	buildCoalMile->doneEvaluateReality = true;

	GoalStep* buildSmelter = new GoalStep(
		"Build smelter",
		{
			TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Tree, 10, entityManager->FindBuildingOfType(EBuildingType::Smelter)),
			TaskAttribute(ETaskAttributeCategory::Worker, EWorkerRole::Builder, 1, nullptr)
		},
		TaskAttribute(ETaskAttributeCategory::Building, EBuildingType::Smelter, 1, nullptr),
		[*this](Worker* worker) { return WorkerTasks::BuildTask(worker, entityManager->FindBuildingOfType(EBuildingType::Smelter)); }
	);

	buildSmelter->doneEvaluateReality = true;

	availableSteps.push_back(buildCoalMile);
	availableSteps.push_back(buildSmelter);

	// Goals definition
	goals.push_back(new Goal(*buildCoalMile, availableSteps));
	goals.push_back(new Goal(*buildSmelter, availableSteps));
	goals.push_back(new Goal(*createCoal, availableSteps));
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
	for (auto& wTask : workerTaskMap)
	{
		if (!wTask.second)
			continue;

		if (wTask.second->finished)
		{
			// Finished - Remove
			//*goals[currentGoal]->potentialCapital -= wTask.second->rewardCapital;
			wTask.second->parentGoalStep->finishedTasks++;
			wTask.second = nullptr;
			continue;
		}

		wTask.second->Update(dTime);
	}

	DebugDraw();
}

void Commander::UpdatePlan()
{
	for (Worker& worker : entityManager->workers)
	{
		for (Goal*& goal : goals)
		{
			GoalStep* step = goal->NextAvailableStep();
			if (step)
				step->AssignTask();
		}
	}
}

int displayTask = 0;

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
	if (IsKeyDown(KEY_ONE))
		displayTask = 0;
	if (IsKeyDown(KEY_TWO))
		displayTask = 1;
	if (IsKeyDown(KEY_THREE))
		displayTask = 2;

	if (goals[displayTask]->finalStep)
		goals[displayTask]->DebugDraw(*goals[displayTask]->finalStep, 0, 0);
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