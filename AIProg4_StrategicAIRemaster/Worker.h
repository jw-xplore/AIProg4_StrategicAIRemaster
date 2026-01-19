#pragma once
#include "Capital.h"
#include <vector>
#include "raylib.h"

struct Node;
class PathFinding;

namespace Worker
{
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

	struct TransformComponent
	{
		Vector2 position;
		Vector2 target;
		float speed;
	};

	struct WorkerComponent
	{
		EWorkerRole role = EWorkerRole::General;
		Capital::ECapitalType carriedMaterial = Capital::ECapitalType::None;

		// Path
		PathFinding* pathfinding;
		std::vector<Node>* path;
		int currentPathNode = 0;
		float pathNodeDistance = 10;
	};

	void Update(float dTime, WorkerComponent entity);
	void Render();
}

