#include "Decisions.h"
#include "Capital.h"
#include "SystemsHolder.h"
#include "Commander.h"
#include "Task.h"

//--------------------------------------------------------------
// Commander decisions
//--------------------------------------------------------------

/// <summary>
/// Check through all resources and there is enough of target resources
/// </summary>
/// <returns></returns>
DecisionTreeNode* CommanderDecisions::HasResources::makeDecision()
{
	// Check missing
	Capital::ECapitalType missing = Capital::ECapitalType::None;

	for (size_t i = 0; i < Capital::ECapitalType::ECapitalTypeCount; i++)
	{
		// Ignore 0 cost
		if (targetAmounts->amounts[i] <= 0)
			continue;

		int val = currentAmounts->amounts[i] + potentialAmounts->amounts[i];

		if (val < targetAmounts->amounts[i])
		{
			missing = (Capital::ECapitalType)i;
			break;
		}
	}
	
	// Nothing missing 
	if (missing == Capital::ECapitalType::None)
		return successAction;

	// Apply behavior
	switch (missing)
	{
	case Capital::ECapitalType::None: return successAction;
	case Capital::ECapitalType::Tree: return treeAction;
	case Capital::ECapitalType::Coal: return coalAction;
	case Capital::ECapitalType::IronOre: return ironOreAction;
	case Capital::ECapitalType::IronBar: return ironBarAction;
	case Capital::ECapitalType::Sword: return swordAction;
	}

	return successAction;
}

//--------------------------------------------------------------
// Commander actions
//--------------------------------------------------------------

void CommanderDecisions::AssignTaskAction::execute()
{
	//taskFunc();
	Commander* commander = SystemsHolder::GetInstance()->commander;
	Worker* worker = commander->FindFreeWorker(roleConstrain);

	if (worker)
	{
		Task* task = taskFunc(worker);
		commander->AssignTask(worker, nullptr, task);
	}
}