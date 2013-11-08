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
#ifndef OPENXCOM_SOLDIERDIARYSSTATE_H
#define OPENXCOM_SOLDIERDIARYSSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;
class SoldierInfoState;

/**
 * Medals screen that lets the player
 * see all the medals a soldier has.
 */
class SoldierDiaryState : public State
{
private:
	Base *_base;
	size_t _soldier;
	SoldierInfoState *_soldierInfoState;

	TextButton *_btnOk, *_btnPrev, *_btnNext, *_btnTotals;
	Window *_window;
	Text *_txtTitle, *_txtLocation, *_txtStatus, *_txtDate;
	TextList *_lstDiary;

public:
	/// Creates the Soldiers state.
	SoldierDiaryState(Game *game, Base *base, size_t soldier, SoldierInfoState *soldierInfoState);
	/// Cleans up the Soldiers state.
	~SoldierDiaryState();
	/// Updates the soldier info.
	void init();
	/// Set the soldier's Id.
	void setSoldierId(size_t soldier);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
	/// Handler for clicking the Totals button.
	void btnTotalsClick(Action *action);
	/// Handler for clicking on mission list.
	void lstDiaryInfoClick(Action *action);
};

}

#endif