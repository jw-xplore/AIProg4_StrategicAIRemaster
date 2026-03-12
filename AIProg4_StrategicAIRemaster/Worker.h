#pragma once
#include "Capital.h"
#include <vector>
#include "raylib.h"

class PathFinding;
class Pickup;
class Building;
struct TreesTile;
class Task;

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

class Worker
{
public:
	const int WORKER_SIZE = 2;
	const Color WORKER_COLOR = RED;

	EWorkerRole role = EWorkerRole::General;
	Capital::ECapitalType carriedMaterial = Capital::ECapitalType::None;

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
};

namespace WorkerTasks
{
	Task* FellTreeTask(Worker* worker);
	Task* DeliverItemTask(Worker* worker, Capital::ECapitalType itemType, Building* target);

	/*
	Task* TrainForRoleTask(Worker* worker, EWorkerRole role);
	Task* BuildTask(Worker* worker, Building* building);
	*/
};