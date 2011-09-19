#ifndef OPENXCOM_NEWPOSSIBLEPRODUCTIONSTATE_H
#define OPENXCOM_NEWPOSSIBLEPRODUCTIONSTATE_H

#include "NewPossibleResearchState.h"

namespace OpenXcom
{
class NewPossibleProductionState : public NewPossibleResearchState
{
public:
	NewPossibleProductionState(Game * game, Base * base, RuleResearchProject * projct);
	void btnOkClick(Action *action);
	void btnAllocateClick(Action *action);
};
}
#endif
