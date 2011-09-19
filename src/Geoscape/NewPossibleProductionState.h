#ifndef OPENXCOM_NEWPOSSIBLEPRODUCTIONSTATE_H
#define OPENXCOM_NEWPOSSIBLEPRODUCTIONSTATE_H

#include "NewPossibleResearchState.h"

namespace OpenXcom
{
/**
   Window which inform the player of new possible productions.
   Also allow to go to the ManufactureState to dispatch available engineers.
 */
class NewPossibleProductionState : public NewPossibleResearchState
{
public:
	/// Creates the NewPossibleProduction state.
	NewPossibleProductionState(Game * game, Base * base, RuleResearchProject * projct);
	/// Handler for clicking the Allocate Production button.
	void btnAllocateClick(Action *action);
};
}
#endif
