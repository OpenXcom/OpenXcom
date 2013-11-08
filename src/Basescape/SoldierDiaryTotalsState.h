/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifndef OPENXCOM_SOLDIERDIARYSTOTALSSTATE_H
#define OPENXCOM_SOLDIERDIARYSTOTALSSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;

/**
 * Medals screen that lets the player
 * see all the medals a soldier has.
 */
class SoldierDiaryTotalsState : public State
{
private:
	Base *_base;
	size_t _soldier;
	SoldierDiaryState *_soldierDiaryState;

	TextButton *_btnOk, *_btnPrev, *_btnNext;
	Window *_window;
	Text *_txtTitle, *_txtRank, *_txtRace, *_txtWeapon, *_txtUFO;
	TextList *_lstRank, *_lstRace, *_lstWeapon, *_lstUFO;

public:
	/// Creates the Soldiers state.
	SoldierDiaryTotalsState(Game *game, Base *base, size_t soldier, SoldierDiaryState *soldierDiaryState);
	/// Cleans up the Soldiers state.
	~SoldierDiaryTotalsState();
	/// Updates the soldier info.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
};

}

#endif