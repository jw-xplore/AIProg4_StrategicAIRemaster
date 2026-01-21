#pragma once
#include "Capital.h"
#include <vector>
#include "raylib.h"

class PathFinding;

enum EWorkerRole
{
	General,
	Scout,
	CoalMiner,
	ArmsSmith,
	Smelter,
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
	PathFinding* pathfinding;
	std::vector<Vector2> path;
	int currentPathNode = 0;
	float pathNodeDistance = 10;

	Worker(){}
	Worker(Vector2 starPos);
	~Worker();

	void Update(float dTime);
	void Render();
	bool FollowPath();
};

