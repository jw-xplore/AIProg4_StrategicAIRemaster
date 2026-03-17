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
#include "DecisionTree.h"
#include "Decisions.h"
#include "Database.h"
#include <iostream>

Commander::Commander()
{
	searchResult = new std::map<Node*, NodeRecordAs>();
	open = new std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare>();

	SystemsHolder* systemsHolder = SystemsHolder::GetInstance();
	systemsHolder->commander = this;
	entityManager = SystemsHolder::GetInstance()->entityMananger;

	// Test building
	//Building* coalMile = new Building(EBuildingType::CoalMile, World::TileToCenterPosition({ 15, 14 }));
	//entityManager->buildings.push_back(coalMile);

	// Test worker action
	for (Worker& worker : entityManager->workers)
	{
		workerTaskMap[&worker] = nullptr;
		workerGoalMap[&worker] = nullptr;
	}

	goals.push_back(CommanderGoals::CreateBuidingGoal(this, EBuildingType::CoalMile));
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
	for (auto& wTask : workerTaskMap)
	{
		if (!wTask.second)
			continue;

		if (wTask.second->finished)
		{
			// Finished - Remove
			*goals[currentGoal].potentialCapital -= wTask.second->rewardCapital;
			wTask.second = nullptr;
			continue;
		}

		wTask.second->Update(dTime);
	}
}

void Commander::UpdatePlan()
{
	// Count potential capital - TODO: Do this calculation only on task add/remove
	/*
	goals[currentGoal].potentialCapital->Empty();

	for (auto& wTask : workerTaskMap)
	{
		if (!wTask.second)
			continue;

		*goals[currentGoal].potentialCapital += wTask.second->rewardCapital;
	}
	*/

	//std::cout << "Wood potential: " << goals[currentGoal].potentialCapital[Capital::ECapitalType::Tree] << "\n";

	// Run current goal decision tree
	for (Worker& worker : entityManager->workers)
	{
		if (workerTaskMap[&worker] != nullptr)
			continue;

		Action* action = dynamic_cast<Action*>(goals[currentGoal].decisionTree->makeDecision());
		if (action)
			action->execute();
	}

	// TODO:
	// Add to pending tasks

	// Distribute pending tasks to active if workers are available
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

void Commander::AssignTask(Worker* worker, CommanderGoals::CommanderGoal* goal, Task* task)
{
	workerTaskMap[worker] = task;
	workerGoalMap[worker] = goal;

	task->assignee = worker;

	*goals[currentGoal].potentialCapital += task->rewardCapital;
}

//--------------------------------------------------------------
// Commander goals
//--------------------------------------------------------------

namespace CommanderGoals
{
	//--------------------------------------------------------------
	// Warmup
	//--------------------------------------------------------------

	void WarmupGoal::Setup()
	{
		// Create scouts and builder

		// Send others for wood
	}

	bool WarmupGoal::Complete()
	{
		return true;
	}

	//--------------------------------------------------------------
	// Create building
	//--------------------------------------------------------------

	void CreateBuidingGoal::Setup()
	{
		SystemsHolder* systemsHolder = SystemsHolder::GetInstance();
		EntityManager* entityManager = SystemsHolder::GetInstance()->entityMananger;

		// Preplace building
		building = new Building(buildingType, World::TileToCenterPosition({ 15, 14 }));
		entityManager->buildings.push_back(building);

		// Setup decisions

		// Resource checking and gathering actions
		CommanderDecisions::HasResources* resourceCheck = new CommanderDecisions::HasResources();
		decisionTree = resourceCheck;

		resourceCheck->targetAmounts = &GameDB::Database::Instance()->actionCostsBuilding->capital;
		resourceCheck->currentAmounts = &building->storedCapital; // Current amounts should be evaluated with potential capital
		resourceCheck->potentialAmounts = potentialCapital;

		resourceCheck->treeAction = new CommanderDecisions::AssignTaskAction(EWorkerRole::General,
			[*this](Worker* worker) {
				// Pickup wood 
				Task* deliverTask = WorkerTasks::DeliverItemTask(worker, Capital::ECapitalType::Tree, building);
				if (deliverTask)
					return deliverTask;

				return WorkerTasks::FellTreeTask(worker);
			}
			);

		resourceCheck->ironOreAction = new CommanderDecisions::AssignTaskAction(EWorkerRole::General,
			[*this](Worker* worker) { return WorkerTasks::DeliverItemTask(worker, Capital::ECapitalType::IronOre, building); }
		);

		// Building
		//resourceCheck->successAction = 

	}

	bool CreateBuidingGoal::Complete()
	{
		return true;
	}
}