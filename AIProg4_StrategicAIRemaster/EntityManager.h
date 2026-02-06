#pragma once
#include <vector>
#include "raylib.h"

enum EWorkerRole;
class Worker;
class Pickup;
class Building;

class EntityManager
{
public:
	std::vector<Worker>* workers;
	std::vector<Worker*>* workersRoleFilter;

	std::vector<Pickup>* pickups;
	std::vector<Building>* buildings;

	EntityManager();
	~EntityManager();

	void Update(float dTime);

	void AddPickup(Pickup* pickup);
	void RemovePickup(Pickup* pickup);
};

