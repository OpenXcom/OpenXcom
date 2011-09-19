/*
 * Copyright 2011 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
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
