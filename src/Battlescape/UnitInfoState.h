/*
 * Copyright 2010 OpenXcom Developers.
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
#ifndef OPENXCOM_UNITINFOSTATE_H
#define OPENXCOM_UNITINFOSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class BattleUnit;
class Text;
class Bar;
class Surface;

/**
 * Unit Info screen that shows all the
 * info of a specific unit.
 */
class UnitInfoState : public State
{
private:
	BattleUnit *_unit;

	Surface *_bg;
	Text *_txtName;

	Text *_txtTimeUnits, *_txtEnergy, *_txtHealth, *_txtFatalWounds, *_txtBravery, *_txtMorale, *_txtReactions, *_txtFiring, *_txtThrowing, *_txtStrength;
	Text *_numTimeUnits, *_numEnergy, *_numHealth, *_numFatalWounds, *_numBravery, *_numMorale, *_numReactions, *_numFiring, *_numThrowing, *_numStrength;
	Bar *_barTimeUnits, *_barEnergy, *_barHealth, *_barFatalWounds, *_barBravery, *_barMorale, *_barReactions, *_barFiring, *_barThrowing, *_barStrength;

	Text *_txtFrontArmour, *_txtLeftArmour, *_txtRightArmour, *_txtRearArmour, *_txtUnderArmour;
	Text *_numFrontArmour, *_numLeftArmour, *_numRightArmour, *_numRearArmour, *_numUnderArmour;
	Bar *_barFrontArmour, *_barLeftArmour, *_barRightArmour, *_barRearArmour, *_barUnderArmour;
public:
	/// Creates the Unit Info state.
	UnitInfoState(Game *game, BattleUnit *unit);
	/// Cleans up the Unit Info state.
	~UnitInfoState();
	/// Updates the unit info.
	void init();
	/// Handler for clicking the button.
	void handle(Action *action);
};

}

#endif
