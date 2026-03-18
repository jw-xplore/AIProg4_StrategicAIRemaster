#pragma once
#include "DecisionTree.h"
#include <functional>

class Commander;
class Building;
class Task;
class Worker;

enum EWorkerRole;
enum EBuildingState;

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
		DecisionTreeNode* successAction;
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
	/// Checks current building status - Finished? In Progress? Just preplaced?
	/// </summary>
	class BuidingHasState : public Decision
	{
	public:
		Building* building;
		EBuildingState state;

		BuidingHasState(Building* building, EBuildingState state): building(building), state(state) {}
		bool pass() override;
	};

	class HasWorkersOfRole : public Decision
	{
	public:
		EWorkerRole role;
		int amount;

		HasWorkersOfRole(EWorkerRole role, int amount): role(role), amount(amount) {}
		bool pass() override;
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