#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Engine/State.h"

namespace OpenXcom
{

class BattleUnit;
class Text;
class TextButton;
class Bar;
class Surface;
class SavedBattleGame;
class BattlescapeState;

/**
 * Unit Info screen that shows all the
 * info of a specific unit.
 */
class UnitInfoState : public State
{
private:
	SavedBattleGame *_battleGame;

	BattleUnit *_unit;
	BattlescapeState *_parent;

	bool _fromInventory, _mindProbe;

	Surface *_bg;
	InteractiveSurface *_exit;
	Text *_txtName;

	Text *_txtTimeUnits, *_txtEnergy, *_txtHealth, *_txtFatalWounds, *_txtBravery, *_txtMorale, *_txtReactions, *_txtFiring, *_txtThrowing, *_txtMelee, *_txtStrength;
	Text *_txtPsiStrength,  *_txtPsiSkill;
	Text *_numTimeUnits, *_numEnergy, *_numHealth, *_numFatalWounds, *_numBravery, *_numMorale, *_numReactions, *_numFiring, *_numThrowing, *_numMelee, *_numStrength;
	Text *_numPsiStrength, *_numPsiSkill;
	Bar *_barTimeUnits, *_barEnergy, *_barHealth, *_barFatalWounds, *_barBravery, *_barMorale, *_barReactions, *_barFiring, *_barThrowing, *_barMelee, *_barStrength;
	Bar *_barPsiStrength, *_barPsiSkill;

	Text *_txtFrontArmor, *_txtLeftArmor, *_txtRightArmor, *_txtRearArmor, *_txtUnderArmor;
	Text *_numFrontArmor, *_numLeftArmor, *_numRightArmor, *_numRearArmor, *_numUnderArmor;
	Bar *_barFrontArmor, *_barLeftArmor, *_barRightArmor, *_barRearArmor, *_barUnderArmor;
	TextButton *_btnPrev, *_btnNext;
public:
	/// Creates the Unit Info state.
	UnitInfoState(BattleUnit *unit, BattlescapeState *parent, bool fromInventory, bool mindProbe);
	/// Cleans up the Unit Info state.
	~UnitInfoState();
	/// Updates the unit info.
	void init();
	/// Handler for clicking the button.
	void handle(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
	/// Handler for exiting the state.
	void exitClick(Action *action);
};

}
