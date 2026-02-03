#include "Decisions.h"
#include "Capital.h"

//--------------------------------------------------------------
// Commander decisions
//--------------------------------------------------------------

DecisionTreeNode* CommanderDecisions::HasResources::makeDecision()
{
	// Check missing
	Capital::ECapitalType missing = Capital::ECapitalType::None;

	for (size_t i = 0; i < Capital::ECapitalType::ECapitalTypeCount; i++)
	{
		if (compareAmounts->amounts[i] > targetAmounts->amounts[i])
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
	case Capital::ECapitalType::Tree: break;
	case Capital::ECapitalType::Coal: break;
	case Capital::ECapitalType::IronOre: break;
	case Capital::ECapitalType::IronBar: break;
	case Capital::ECapitalType::Sword: break;
	}

	return successAction;
}