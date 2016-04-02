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

class TextButton;
class Window;
class Text;
class TextList;
class Soldier;

/**
 * Diary window that shows 
 * mission details for a soldier.
 */
class SoldierDiaryMissionState : public State
{
private:
	Soldier *_soldier;

	TextButton *_btnOk, *_btnPrev, *_btnNext;
	Window *_window;
	Text *_txtTitle, *_txtUFO, *_txtScore, *_txtKills, *_txtLocation, *_txtRace, *_txtDaylight, *_txtDaysWounded;
	Text *_txtNoRecord;
	TextList *_lstKills;

	int _rowEntry;
public:
	/// Creates the Soldier Diary Mission state.
	SoldierDiaryMissionState(Soldier *soldier, int rowEntry);
	/// Cleans up the Soldier Diary Mission state.
	~SoldierDiaryMissionState();
	/// Handler for clicking the Cancel button.
	void btnOkClick(Action *action);
	/// Updates the mission info.
	void init();
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
};

}
