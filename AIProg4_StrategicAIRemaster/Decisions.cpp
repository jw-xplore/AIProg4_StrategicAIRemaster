#include "Decisions.h"
#include "Capital.h"
#include "SystemsHolder.h"
#include "Commander.h"
#include "Task.h"
#include "EntityManager.h"
#include "Worker.h"
#include "Building.h"
#include <iostream>

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

		int val = currentAmounts->amounts[i];

		if (potentialAmounts)
			val += potentialAmounts->amounts[i];

		if (val < targetAmounts->amounts[i])
		{
			missing = (Capital::ECapitalType)i;
			break;
		}
	}
	
	// Nothing missing 
	if (missing == Capital::ECapitalType::None)
		return successAction->makeDecision();

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

/// <summary>
/// Check if there is enought workers of selected type
/// </summary>
bool CommanderDecisions::HasWorkersOfRole::pass()
{
	EntityManager* entityManager = SystemsHolder::GetInstance()->entityMananger;

	int count = 0;

	for (Worker& worker : entityManager->workers)
	{
		if (worker.role == this->role || worker.trainedRole == this->role)
			count++;

		if (count >= this->amount)
			return true;
	}

	return false;
}

/// <summary>
/// Just compare building state value
/// </summary>
/// <returns></returns>
bool CommanderDecisions::BuidingHasState::pass()
{
	// Check finish debug 
	if (state == EBuildingState::Finished && building->state == this->state)
		std::cout << "Building finished! \n";

	return building->state == this->state;
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