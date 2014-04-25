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
#ifndef OPENXCOM_SOLDIERDIARYMISSIONSTATE_H
#define OPENXCOM_SOLDIERDIARYMISSIONSTATE_H

#include <vector>
#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class TextButton;
class Window;
class Text;
class TextList;

/**
 * Select Mission window that shows 
 * mission details for a soldier.
 */
class SoldierDiaryMissionState : public State
{
private:
	Base *_base;
	size_t _soldier;

	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtScore, *_txtKills, *_txtMissionType, *_txtUFO, *_txtRace, *_txtDaylight, *_txtDaysWounded;
	TextList *_lstKills;

	int _rowEntry;
public:
	/// Creates the Soldier Diary Mission state.
	SoldierDiaryMissionState(Game *game, Base *base, size_t soldier, int rowEntry);
	/// Cleans up the Soldier Diary Mission state.
	~SoldierDiaryMissionState();
	/// Handler for clicking the Cancel button.
	void btnOkClick(Action *action);
};

}

#endif
