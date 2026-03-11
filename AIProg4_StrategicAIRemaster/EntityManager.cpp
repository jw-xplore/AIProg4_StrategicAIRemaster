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

	// Setup worker role filter
	for (size_t i = 0; i < EWorkerRole::EWorkerRoleCount; i++)
	{
		workersRoleFilter = new std::vector<Worker*>[EWorkerRole::EWorkerRoleCount];
	}

	// Setup workers
	int population = db->startingPopulation;

	workers = new std::vector<Worker>();
	workers->reserve(population);

	for (size_t i = 0; i < population; i++)
	{
		Vector2 randPos = { GetRandomValue(startX.x, startX.y), GetRandomValue(startY.x, startY.y) };
		Worker worker = Worker(randPos);
		workers->push_back(worker);

		workersRoleFilter[EWorkerRole::General].push_back(&worker);
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

void EntityManager::Update(float dTime)
{
	// Workers
	for (size_t i = 0; i < workers->size(); i++)
	{
		workers->at(i).Update(dTime);
		workers->at(i).Render();
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
}

void EntityManager::AddPickup(Pickup* pickup)
{
	pickups->push_back(*pickup);
}

void EntityManager::RemovePickup(Pickup* pickup)
{
	pickups->erase(find(pickups->begin(), pickups->end(), pickup));
}

