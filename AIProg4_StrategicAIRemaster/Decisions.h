#pragma once
#include "DecisionTree.h"

class Commander;
enum EWorkerRole;
class Building;

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
	class HasResources : DecisionTreeNode
	{
	public:
		Action* successAction;
		Action* treeAction;
		Action* coalAction;
		Action* ironOreAction;
		Action* ironBarAction;
		Action* swordAction;

		Capital::CapitalAmounts* currentAmounts;
		Capital::CapitalAmounts* targetAmounts;

		DecisionTreeNode* makeDecision() override;
	};

	class BuildingIsPreplaced : Decision
	{
	public:
		
		DecisionTreeNode* makeDecision() override;
	};

	class BuildingIsFinished : Decision
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

	//--------------------------------------------------------------
	// Actions
	//--------------------------------------------------------------

}