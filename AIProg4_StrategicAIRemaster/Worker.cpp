#include "Worker.h"
#include "SteeringBehavior.h"
#include "Constants.h"
#include "PathFinding.h"
#include "SystemsHolder.h"
#include "EntityManager.h"
#include "World.h"
#include "Pickup.h"
#include "Building.h"
#include "Capital.h"
#include "GoalSystem.h"
#include "SubtaskDefinitions.h"
#include "Database.h"

Worker::Worker(Vector2 starPos)
{
	speed = (1.0 / 10.0) * GlobalVars::TILE_SIZE;
	position = starPos;
	target = position;
	pathNodeDistance = pathNodeDistance * pathNodeDistance;
}

Worker::~Worker()
{

}

void Worker::Update(float dTime)
{
	// Move
	if (path.empty() || FollowPath())
		position += SteeringBehavior::Seek(position, target, speed * dTime);
}

void Worker::Render()
{
	Vector2 pos = position;
	DrawCircle(pos.x, pos.y, Worker::WORKER_SIZE, coloring);
}

void Worker::SetNewPath(std::vector<Vector2> newPath)
{
	path = newPath;
	currentPathNode = path.size() - 1;
}

bool Worker::FollowPath()
{
	// Path finished?
	if (currentPathNode < 0)
	{
		path.clear();
		currentPathNode = 0;
		return false;
	}

	// Follow next path point
	int x = path[currentPathNode].x;
	int y = path[currentPathNode].y;

	Vector2 pos = { x, y };
	Vector2 dist = pos - position;

	if (Vector2LengthSqr(dist) > pathNodeDistance)
	{
		target = pos;
		DrawCircle(pos.x, pos.y, 4, ORANGE);
	}
	else
	{
		// Progress to next
		currentPathNode--;
	}

	return true;
}


//--------------------------------------------------------------
// Worker task definitions
//--------------------------------------------------------------

namespace WorkerTasks
{
	Task* FellTreeTask(Worker* worker)
	{
		SystemsHolder* systems = SystemsHolder::GetInstance();

		// Find closest tree
		Vector2Int workerPos = World::PositionToTile(worker->position);
		TreesTile* treesTile = systems->world->ClosestTreeTile(workerPos);
		Vector2 treeTilePos = World::TileToCenterPosition({ treesTile->x, treesTile->y });
		float time = GameDB::Database::Instance()->actionCostsResources[GameDB::EActionResource::FellTree].time;

		// Setup subtasks
		Task* task = new Task(worker,
			{
			new SubtaskDefinitions::MoveToSubtask(treeTilePos),
			new SubtaskDefinitions::FellTreeSubtask(treesTile, time)
			}
		);

		task->name = "FellTree";
		treesTile->reservations++;

		// Input - Output
		//task->output = TaskAttribute(ETaskAttributeCategory::Capital, Capital::ECapitalType::Tree, 1, nullptr);

		return task;
	}

	Task* DeliverItemTask(Worker* worker, Capital::ECapitalType itemType, Building* target)
	{
		SystemsHolder* systems = SystemsHolder::GetInstance();

		// Find closest item
		Pickup* item = systems->entityMananger->FindClosestPickup(worker->position, itemType);
		if (!item)
			return nullptr;

		// Setup subtasks
		Task* task = new Task(worker,
			{
			new SubtaskDefinitions::MoveToSubtask(item->position),
			new SubtaskDefinitions::PickupSubtask(item),
			new SubtaskDefinitions::MoveToSubtask(target->position),
			new SubtaskDefinitions::DropItemSubtask(target)
			}
		);

		task->name = "W Deliver " + std::to_string(itemType);

		// Input - Output
		/*
		TaskAttribute input = TaskAttribute(ETaskAttributeCategory::Capital, itemType, 1, nullptr);
		task->input.push_back(input);

		task->output = TaskAttribute(ETaskAttributeCategory::Capital, itemType, 1, target);
		*/

		return task;
	}

	Task* DeliverFromBuildingTask(Worker* worker, Capital::ECapitalType itemType, Building* from, Building* target)
	{
		SystemsHolder* systems = SystemsHolder::GetInstance();

		// Check availability
		if (from->GetAvailableCapital()[itemType] <= 0)
			return nullptr;

		// Reserve capital
		from->reservedCapital.amounts[itemType] += 1;

		// Setup subtasks
		Task* task = new Task(worker,
			{
			new SubtaskDefinitions::MoveToSubtask(from->position),
			new SubtaskDefinitions::PickupFromBuildingSubtask(from, itemType),
			new SubtaskDefinitions::MoveToSubtask(target->position),
			new SubtaskDefinitions::DropItemSubtask(target)
			}
		);

		task->name = "Deliver " + std::to_string(itemType);

		// Input - Output
		/*
		TaskAttribute input = TaskAttribute(ETaskAttributeCategory::Capital, itemType, 1, from);
		task->input.push_back(input);

		task->output = TaskAttribute(ETaskAttributeCategory::Capital, itemType, 1, target);
		*/

		return task;
	}

	Task* TrainForRoleTask(Worker* worker, EWorkerRole role)
	{
		// Find training time
		GameDB::EActionTraining trainingType = GameDB::EActionTraining::TrainScout;
		switch (role)
		{
			case EWorkerRole::Scout: trainingType = GameDB::EActionTraining::TrainScout; break;
			case EWorkerRole::Builder: trainingType = GameDB::EActionTraining::TrainBuilder; break;
			case EWorkerRole::Soldier: trainingType = GameDB::EActionTraining::TrainSoldier; break;
			default: trainingType = GameDB::EActionTraining::TrainCraftsman; break;
		}

		float time = GameDB::Database::Instance()->actionCostsTraining[trainingType].time;

		// Setup task
		Task* task = new Task(nullptr,
			{
			new SubtaskDefinitions::TrainWorker(role, time)
			}
		);

		task->name = "Train " + std::to_string(role);

		// Input - Output
		/*
		task->output.category = ETaskAttributeCategory::Worker;
		task->output.type = role;
		task->output.amount = 1;
		*/

		return task;
	}

	Task* BuildTask(Worker* worker, Building* building)
	{
		// Building task
		float time = GameDB::Database::Instance()->actionCostsBuilding[building->type].time;
		building->state = EBuildingState::InProgress;

		// Setup task
		Task* task = new Task(worker,
			{
			new SubtaskDefinitions::MoveToSubtask(building->position),
			new SubtaskDefinitions::CreateBuilding(building, time)
			}
		);

		task->name = "Build " + std::to_string(building->type);

		// Input - Output
		/*
		TaskAttribute input = TaskAttribute(ETaskAttributeCategory::Worker, EWorkerRole::Builder, 1, nullptr);
		task->input.push_back(input);

		task->output = TaskAttribute(ETaskAttributeCategory::Building, building->type, 1, nullptr);
		*/

		return task;
	}

	Task* CreateItemTask(Worker* worker, Building* building, Capital::ActionCost cost, Capital::ECapitalType gainItem)
	{
		// Building task
		float time = GameDB::Database::Instance()->actionCostsBuilding[building->type].time;

		// Setup task
		Task* task = new Task(worker,
			{
			new SubtaskDefinitions::MoveToSubtask(building->position),
			new SubtaskDefinitions::CreateItem(building, cost.time, cost.capital, gainItem)
			//new SubtaskDefinitions::CreateBuilding(building, time)
			}
		);

		task->name = "Cr coal";

		return task;
	}
}
