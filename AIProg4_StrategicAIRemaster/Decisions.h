#pragma once
#include "DecisionTree.h"

class Commander;
enum EWorkerRole;

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
	// Decisions
	class HasResources : DecisionTreeNode
	{
	public:
		Action* successAction;
		Capital::CapitalAmounts* compareAmounts;
		Capital::CapitalAmounts* targetAmounts;

		DecisionTreeNode* makeDecision() override;
	};

	class BuildingStand : Decision
	{
	public:
		DecisionTreeNode* makeDecision() override;
	};

	class HasWorkersOfRole : Decision
	{
	public:
		EWorkerRole role;
		int amount;

		DecisionTreeNode* makeDecision() override;
	};

	// Actions
	//class 
}