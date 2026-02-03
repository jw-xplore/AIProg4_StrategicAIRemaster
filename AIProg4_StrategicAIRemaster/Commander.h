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

class DecisionTreeNode;
class CommanderGoal;

class Commander
{
public:
	std::vector<CommanderGoal> goals;
	int currentGoal = 0;

	std::vector<std::queue<Worker*>> freeWorkers; // Queue of free workers divided by roles
	std::queue<Task*> pendingTasks;
	std::vector<Task*> activeTasks;
	std::queue<EBuildingType> neededBuildings;

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

		CommanderGoal() {}
		~CommanderGoal() {}

		virtual void Setup() {}
		virtual bool Complete() {}
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