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
		// Ignore 0 cost
		if (targetAmounts->amounts[i] <= 0)
			continue;

		if (currentAmounts->amounts[i] < targetAmounts->amounts[i])
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