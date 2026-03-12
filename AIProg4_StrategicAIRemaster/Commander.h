#pragma once
/*
Takes care of high-level decision making and assigning tasks
*/

#include <vector>
#include <queue>
#include <map>
#include <algorithm>

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

		CommanderGoal(Commander* commander) : commander(commander) {}
		~CommanderGoal() {}

		virtual void Setup() {}
		virtual bool Complete() { return true; }
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
	class CreateBuidlingGoal : public CommanderGoal
	{
	public:
		EBuildingType buildingType;

		void Setup() override;
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
	std::vector<CommanderGoals::CommanderGoal> goals;
	int currentGoal = 0;

	std::vector<Task*> activeTasks;
	int activeTasksCount = 0;
	std::queue<EBuildingType> neededBuildings;

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

	void RegisterFreeWorker(Worker* worker);
};