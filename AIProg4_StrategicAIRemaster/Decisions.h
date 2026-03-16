#pragma once
#include "DecisionTree.h"
#include <functional>

class Commander;
enum EWorkerRole;
class Building;
class Task;
class Worker;

//--------------------------------------------------------------
// Commander decisions
//--------------------------------------------------------------

namespace Capital
{
	enum ECapitalType;
	struct CapitalAmounts;
}

namespace CommanderDecisions
{
	//--------------------------------------------------------------
	// Decisions
	//--------------------------------------------------------------
	class HasResources : public DecisionTreeNode
	{
	public:
		Action* successAction;
		Action* treeAction;
		Action* coalAction;
		Action* ironOreAction;
		Action* ironBarAction;
		Action* swordAction;

		Capital::CapitalAmounts* potentialAmounts;
		Capital::CapitalAmounts* currentAmounts;
		Capital::CapitalAmounts* targetAmounts;

		DecisionTreeNode* makeDecision() override;
	};

	/// <summary>
	/// Checks if building is standing
	/// </summary>
	class BuildingIsPreplaced : public Decision
	{
	public:
		
		DecisionTreeNode* makeDecision() override;
	};

	class BuildingIsFinished : public Decision
	{
	public:
		Building* building;
		BuildingIsFinished(Building* building): building(building) {}
		bool pass() override;
	};

	class HasWorkersOfRole : public Decision
	{
	public:
		EWorkerRole role;
		int amount;

		HasWorkersOfRole(EWorkerRole role, int amount): role(role), amount(amount) {}
		DecisionTreeNode* makeDecision() override;
	};


	//--------------------------------------------------------------
	// Actions
	//--------------------------------------------------------------

	class AssignTaskAction : public Action
	{
	public:
		EWorkerRole roleConstrain;
		std::function<Task* (Worker*)> taskFunc;

		AssignTaskAction(EWorkerRole roleConstrain, std::function<Task* (Worker*)> fnc)
		{
			this->roleConstrain = roleConstrain;
			taskFunc = fnc;
		}

		void execute() override;
	};
}