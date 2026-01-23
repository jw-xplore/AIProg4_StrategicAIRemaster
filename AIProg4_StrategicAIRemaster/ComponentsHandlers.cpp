#include "ComponentsHandlers.h"

void ComponentsHandlers::Init(World* world, EntityManager* entityMngr, PathFinding* pathfinding)
{
	this->world = world;
	this->entityManager = entityMngr;
	this->pathfinding = pathfinding;
}

void ComponentsHandlers::Shutdown()
{

}