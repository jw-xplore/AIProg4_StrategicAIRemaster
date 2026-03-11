#pragma once
#include "Capital.h"
#include <vector>
#include "raylib.h"

class PathFinding;
class Pickup;
class Building;
struct TreesTile;

enum EWorkerRole
{
	General,
	Scout,
	CoalMiner,
	ArmsSmith,
	SmelterOperator,
	Builder,
	Soldier,
	EWorkerRoleCount
};

enum EWorkerActions
{
	Idle = -1,
	MoveTo,
	PickupItem,
	Deliver,
	Build,
	FellTree,
	Train,
	BuildingAction, //? Like this
	EWorkerActionsCount
};

struct WorkerActionData
{
	float x, y;
	Pickup* targetItem;
	Building* targetBuilding;
	TreesTile* targetTreeTile;
	Capital::ActionCost cost;
	EWorkerRole targetRole;
};

class Worker
{
public:
	const int WORKER_SIZE = 2;
	const Color WORKER_COLOR = RED;

	EWorkerRole role = EWorkerRole::General;
	Capital::ECapitalType carriedMaterial = Capital::ECapitalType::None;
	EWorkerActions currentAction = EWorkerActions::Idle;
	WorkerActionData actionData;
	float actionTimer = 0;

	// Transform
	Vector2 position;
	Vector2 target;
	float speed;

	// Path
	std::vector<Vector2> path = {};
	int currentPathNode = 0;
	float pathNodeDistance = 10;

	Worker(){}
	Worker(Vector2 starPos);
	~Worker();

	void Update(float dTime);
	void Render();

	void SetNewPath(std::vector<Vector2> newPath);
	bool FollowPath();

	// Actions
	void PickupItem();
	void DeliverItemTo();
	void Build(float dt);
	void FellTree(float dt);
	void TrainInto(float dt);

	bool TileReached(Vector2 targetPos);
};

