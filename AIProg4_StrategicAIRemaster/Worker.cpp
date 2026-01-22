#include "Worker.h"
#include "SteeringBehavior.h"
#include "Constants.h"
#include "PathFinding.h"

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