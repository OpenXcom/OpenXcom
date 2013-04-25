/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_SOLDIERINFOSTATE_H
#define OPENXCOM_SOLDIERINFOSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class Surface;
class TextButton;
class Text;
class TextEdit;
class Bar;

/**
 * Soldier Info screen that shows all the
 * info of a specific soldier.
 */
class SoldierInfoState : public State
{
private:
	Base *_base;
	size_t _soldier;

	Surface *_bg, *_rank;
	TextButton *_btnOk, *_btnPrev, *_btnNext, *_btnArmor;
	Text *_txtArmor, *_txtRank, *_txtMissions, *_txtKills, *_txtCraft, *_txtRecovery, *_txtPsionic;
	TextEdit *_edtSoldier;

	Text *_txtTimeUnits, *_txtStamina, *_txtHealth, *_txtBravery, *_txtReactions, *_txtFiring, *_txtThrowing, *_txtStrength, *_txtPsiStrength, *_txtPsiSkill;
	Text *_numTimeUnits, *_numStamina, *_numHealth, *_numBravery, *_numReactions, *_numFiring, *_numThrowing, *_numStrength, *_numPsiStrength, *_numPsiSkill;
	Bar *_barTimeUnits, *_barStamina, *_barHealth, *_barBravery, *_barReactions, *_barFiring, *_barThrowing, *_barStrength, *_barPsiStrength, *_barPsiSkill;
public:
	/// Creates the Soldier Info state.
	SoldierInfoState(Game *game, Base *base, size_t soldier);
	/// Cleans up the Soldier Info state.
	~SoldierInfoState();
	/// Updates the soldier info.
	void init();
	/// Handler for pressing a key on the Name edit.
	void edtSoldierKeyPress(Action *action);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
	/// Handler for clicking the Armor button.
	void btnArmorClick(Action *action);
};

}

#endif
