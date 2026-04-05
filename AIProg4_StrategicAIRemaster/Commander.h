#pragma once
/*
Takes care of high-level decision making and assigning tasks
*/

#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include "Capital.h"

class Worker;
enum EWorkerRole;
class Task;
enum EBuildingType;

struct Node;
struct NodeRecordAs;
struct NodeRecordAsCompare;
class PathFinding;

class EntityManager;
class DecisionTreeNode;
class Decision;

class Commander;
class Building;

//--------------------------------------------------------------
// Commander
//--------------------------------------------------------------

struct CommnaderStateTracking
{
	// Tracks game state from commnader perspective
	int scoutsAmount;
	
	// Buildings
	Building* coalMile;
	Building* arsmithsForge;
	Building* smelter;
	Building* trainingCamp;
};

class Commander
{
public:
	CommnaderStateTracking stateTracking;
	EntityManager* entityManager;

	// Path finding
	std::map<Node*, NodeRecordAs>* searchResult;
	std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare>* open;

	// Plan
	std::vector<Task> availableTasks;
	std::vector<Goal> goals;

	float replanTimer = 0;
	const float replanDelay = 1;

	Commander();
	~Commander();

	void DefineAvailableTasks();
	void Update(float dTime);
	void UpdatePlan();
	void DebugDraw();

	Worker* FindFreeWorker(EWorkerRole roleConstrain);
	//void AssignTask(Worker* worker, CommanderGoals::CommanderGoal* goal, Task* task);
};