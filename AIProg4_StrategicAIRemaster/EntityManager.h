#pragma once
#include <vector>
#include "raylib.h"

class Worker;
class Pickup;
class Building;

class EntityManager
{
public:
	std::vector<Worker>* workers;
	std::vector<Pickup>* pickups;
	std::vector<Building>* buildings;

	EntityManager();
	~EntityManager();

	void Update();
};

