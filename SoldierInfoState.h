/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__SOLDIERINFOSTATE_H
#define OPENXCOM__SOLDIERINFOSTATE_H

#include <sstream>
#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "Bar.h"
#include "Button.h"
#include "Text.h"
#include "TextEdit.h"
#include "Surface.h"
#include "Base.h"
#include "Soldier.h"

/**
 * Base Info screen that shows all the 
 * stats of a base from the Basescape.
 */
class SoldierInfoState : public State
{
private:
	Base *_base;
	unsigned int _soldier;

	Surface *_bg, *_rank;
	Button *_btnOk, *_btnPrev, *_btnNext, *_btnArmour;
	Text *_txtArmour, *_txtRank, *_txtMissions, *_txtKills, *_txtCraft;
	TextEdit *_edtSoldier;

	Text *_txtTimeUnits, *_txtStamina, *_txtHealth, *_txtBravery, *_txtReactions, *_txtFiring, *_txtThrowing, *_txtStrength;
	Text *_numTimeUnits, *_numStamina, *_numHealth, *_numBravery, *_numReactions, *_numFiring, *_numThrowing, *_numStrength;
	Bar *_barTimeUnits, *_barStamina, *_barHealth, *_barBravery, *_barReactions, *_barFiring, *_barThrowing, *_barStrength;
public:
	/// Creates the Soldier Info state.
	SoldierInfoState(Game *game, Base *base, unsigned int soldier);
	/// Cleans up the Soldier Info state.
	~SoldierInfoState();
	/// Updates the soldier info.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Previous button.
	void btnPrevClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Next button.
	void btnNextClick(SDL_Event *ev, int scale);
};

#endif
