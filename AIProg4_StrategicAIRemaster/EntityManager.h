#pragma once
#include <vector>
#include "raylib.h"

class Worker;

class EntityManager
{
public:
	std::vector<Worker>* workers;

	EntityManager();
	~EntityManager();

	void Update();
};

