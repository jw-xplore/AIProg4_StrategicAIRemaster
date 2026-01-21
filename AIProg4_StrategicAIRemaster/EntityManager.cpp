#include "EntityManager.h"
#include "Database.h"
#include "Worker.h"
#include "Constants.h"
#include "SteeringBehavior.h"
#include "Pickup.h"
#include "Capital.h"
#include "Building.h"

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

	// Setup pickups
	int ironAmount = db->ironOreAmount;
	//ironAmount = 100000;

	pickups = new std::vector<Pickup>();
	pickups->reserve(ironAmount * 2);

	for (size_t i = 0; i < ironAmount; i++)
	{
		Vector2 randPos = { GetRandomValue(24, 100 * GlobalVars::TILE_SIZE - 24), GetRandomValue(24, 100 * GlobalVars::TILE_SIZE - 24) };
		pickups->push_back(Pickup(Capital::ECapitalType::IronOre, randPos));
	}

	// Setup buildings
	buildings = new std::vector<Building>();
	buildings->reserve(10);
}

EntityManager::~EntityManager()
{
	workers->clear();
	delete workers;

	pickups->clear();
	delete pickups;
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

	// Render pickups
	for (auto& pickup : *pickups)
	{
		DrawRectangle(pickup.position.x, pickup.position.y, pickup.size.x, pickup.size.y, pickup.color);
	}

	// Render buildings
	for (auto& building : *buildings)
	{
		DrawRectangle(building.position.x, building.position.y, GlobalVars::TILE_HALF_SIZE, GlobalVars::TILE_HALF_SIZE, building.color);
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