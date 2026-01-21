#include "EntityManager.h"
#include "Database.h"
#include "Worker.h"
#include "Constants.h"
#include "SteeringBehavior.h"

EntityManager::EntityManager()
{
	GameDB::Database* db = GameDB::Database::Instance();

	Vector2 startX = { 10 * GlobalVars::TILE_SIZE, 13 * GlobalVars::TILE_SIZE };
	Vector2 startY = { 10 * GlobalVars::TILE_SIZE, 13 * GlobalVars::TILE_SIZE };

	// Setup workers
	int population = db->startingPopulation;
	//population = 0;

	workers = new std::vector<Worker>();
	workers->reserve(population);

	for (size_t i = 0; i < population; i++)
	{
		Vector2 randPos = { GetRandomValue(startX.x, startX.y), GetRandomValue(startY.x, startY.y) };
		workers->push_back(Worker(randPos));
	}
}

EntityManager::~EntityManager()
{
	workers->clear();
	delete workers;
}

void EntityManager::Update()
{
	float dTime = GetFrameTime();

	// Workers
	for (size_t i = 0; i < workers->size(); i++)
	{
		workers->at(i).Update(dTime);
		workers->at(i).Render();

		//workers->at(i).FollowPath();
		//moveComps[i].target = workers->at(i).target;
		//moveComps[i].position += SteeringBehavior::Seek(moveComps[i].position, moveComps[i].target, workers->at(i).speed);
	}

	/*
	for (auto& worker : *workers)
	{
		Worker::Update(worker, dTime);
		Worker::Render(worker);
	}
	*/

	/*
	for (size_t i = 0; i < workers->size(); i++)
	{
		Worker::Update(workers->at(i), dTime);
		Worker::Render(workers->at(i));
	}
	*/
}