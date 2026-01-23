#pragma once
/*
Singleton holding all functional componet references
*/

class World;
class EntityManager;
class PathFinding;

class ComponentsHandlers
{
private:
	static ComponentsHandlers* instance;

public:
	World* world;
	EntityManager* entityManager;
	PathFinding* pathfinding;

	static ComponentsHandlers* GetInstance()
	{
		if (!instance)
			instance = new ComponentsHandlers();

		return instance;
	}

	ComponentsHandlers() {}
	~ComponentsHandlers() {}

	void Init(World* world, EntityManager* entityMngr, PathFinding* pathfinding);
	void Shutdown();
};

