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

class Commander;
class Building;

namespace CommanderDecisions
{
	class HasResources;
}

//--------------------------------------------------------------
// Commander goals
//--------------------------------------------------------------

namespace CommanderGoals
{
	class CommanderGoal
	{
	public:
		Commander* commander;
		DecisionTreeNode* decisionTree;
		// Capital promised if all workers tasks are finnished
		Capital::CapitalAmounts* potentialCapital = new Capital::CapitalAmounts();

		CommanderGoal() {}
		CommanderGoal(Commander* commander) : commander(commander) { Setup(); }
		~CommanderGoal() {}

		virtual void Setup() {}
		virtual bool Complete() { return false; }
	};

	// Do basic setup
	class WarmupGoal : public CommanderGoal
	{
	public:
		int scouts = 3;
		int builders = 1;
		int wood = 46;

		void Setup() override;
		bool Complete() override;
	};

	// Gather resources and create building of specific type
	class CreateBuidingGoal : public CommanderGoal
	{
	public:
		EBuildingType buildingType;
		Building* building;

		CreateBuidingGoal(Commander* commander, EBuildingType buildingType)
		{
			this->commander = commander;
			this->buildingType = buildingType;
			Setup();
		}

		void Setup() override;
		bool Complete() override;

		CommanderDecisions::HasResources* DefineResourceTree();
	};

	class GatherGoal : public CommanderGoal
	{
	public:
		EBuildingType buildingType;

		void Setup() override;
	};

	class BuildArmyGoal : public CommanderGoal
	{
	public:
		EBuildingType buildingType;

		void Setup() override;
	};
}

//--------------------------------------------------------------
// Commander
//--------------------------------------------------------------

struct CommnaderStatictics
{
	// Tracks game state from commnader perspective
	int scoutsAmount;
	
};

class Commander
{
public:
	std::vector<CommanderGoals::CommanderGoal*> goals;
	int currentGoal = 0;

	std::map<Worker*, Task*> workerTaskMap;
	std::map<Worker*, CommanderGoals::CommanderGoal*> workerGoalMap;

	EntityManager* entityManager;

	// Path finding
	std::map<Node*, NodeRecordAs>* searchResult;
	std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare>* open;


	float replanTimer = 0;
	const float replanDelay = 1;

	Commander();
	~Commander();

	void Update(float dTime);
	void UpdatePlan();
	void DebugDraw();

	Worker* FindFreeWorker(EWorkerRole roleConstrain);
	void AssignTask(Worker* worker, CommanderGoals::CommanderGoal* goal, Task* task);
};