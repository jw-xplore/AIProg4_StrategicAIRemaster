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
#include "Task.h"
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
	DrawCircle(pos.x, pos.y, Worker::WORKER_SIZE, Worker::WORKER_COLOR);
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

		task->rewardCapital.amounts[Capital::ECapitalType::Tree] = 1;

		return task;
	}

	Task* DeliverItemTask(Worker* worker, Capital::ECapitalType itemType, Building* target)
	{
		SystemsHolder* systems = SystemsHolder::GetInstance();

		// Find closest item
		Pickup* item = systems->entityMananger->FindClosestPickup(worker->position);

		// Setup subtasks
		Task* task = new Task(worker,
			{
			new SubtaskDefinitions::MoveToSubtask(item->position),
			new SubtaskDefinitions::PickupSubtask(item),
			new SubtaskDefinitions::MoveToSubtask(target->position),
			new SubtaskDefinitions::DropItemSubtask(target)
			}
		);

		task->rewardCapital.amounts[itemType] = 1;

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
		Task* task = new Task(worker,
			{
			new SubtaskDefinitions::TrainWorker(role, time)
			}
		);

		return task;
	}

	Task* BuildTask(Worker* worker, Building* building)
	{
		// Building task
		float time = GameDB::Database::Instance()->actionCostsBuilding[building->type].time;

		// Setup task
		Task* task = new Task(worker,
			{
			new SubtaskDefinitions::MoveToSubtask(building->position),
			new SubtaskDefinitions::CreateBuilding(building, time)
			}
		);

		return task;
	}
}
