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

	// Execute actions
	switch (currentAction)
	{
	case EWorkerActions::PickupItem: PickupItem();
	case EWorkerActions::Deliver: DeliverItemTo();
	case EWorkerActions::Build: Build(dTime);
	case EWorkerActions::FellTree: FellTree(dTime);
	case EWorkerActions::Train: TrainInto(dTime);
	}
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

//-----------------------------------------------
// Worker actions
//-----------------------------------------------

void Worker::PickupItem()
{
	if (!actionData.targetItem)
		return;

	// Is at position
	if (!TileReached(actionData.targetItem->position))
		return;

	// Do action
	SystemsHolder* systems = SystemsHolder::GetInstance();
	systems->entityMananger->RemovePickup(actionData.targetItem);
}

void Worker::DeliverItemTo()
{
	if (!actionData.targetBuilding || carriedMaterial == Capital::ECapitalType::None)
		return;

	// Is at position
	if (!TileReached(actionData.targetBuilding->position))
		return;

	// Deliver
	actionData.targetBuilding->storedCapital.amounts[carriedMaterial] += 1;
	carriedMaterial = Capital::ECapitalType::None;
}

void Worker::Build(float dt)
{
	if (role != EWorkerRole::Builder)
		return;

	Building* building = actionData.targetBuilding;

	if (!building)
		return;

	if (!TileReached(building->position))
		return;

	// Progress
	if (actionData.cost.time > 0)
	{
		actionData.cost.time -= dt;
		return;
	}

	// Create building
	building->standing = true;
}

void Worker::FellTree(float dt)
{
	TreesTile* tTile = actionData.targetTreeTile;

	if (!tTile)
		return;

	// Is at position
	SystemsHolder* systems = SystemsHolder::GetInstance();
	Vector2Int workerTile = systems->world->PositionToTile(this->position);

	if (workerTile.x != tTile->x || workerTile.y != tTile->y)
		return;

	// Progress
	if (actionData.cost.time > 0)
	{
		actionData.cost.time -= dt;
		return;
	}

	// Feel tree
	Vector2Int treePos = tTile->treePositions[tTile->amount - 1];
	Vector2 dropPos = systems->world->TileToCenterPosition({ tTile->x, tTile->y});
	dropPos.x += treePos.x;
	dropPos.y += treePos.y;

	Pickup felledTree = Pickup(Capital::ECapitalType::Tree, dropPos);
	systems->entityMananger->AddPickup(&felledTree);

	tTile->FellTree();
}

void Worker::TrainInto(float dt)
{
	if (role != EWorkerRole::General)
		return;

	// Progress
	if (actionData.cost.time > 0)
	{
		actionData.cost.time -= dt;
		return;
	}
	
	// Set role
	role = actionData.targetRole;
}

bool Worker::TileReached(Vector2 targetPos)
{
	SystemsHolder* systems = SystemsHolder::GetInstance();

	Vector2Int targetTile = systems->world->PositionToTile(targetPos);
	Vector2Int workerTile = systems->world->PositionToTile(this->position);

	return targetTile.x == workerTile.x && targetTile.y == workerTile.y;
}