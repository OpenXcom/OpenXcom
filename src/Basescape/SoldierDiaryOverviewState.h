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
#include <vector>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;
class SoldierInfoState;
class Soldier;

/**
 * Diary screen that shows all the
 * missions a soldier has.
 */
class SoldierDiaryOverviewState : public State
{
private:
	Base *_base;
	size_t _soldierId;
	SoldierInfoState *_soldierInfoState;
	Soldier *_soldier;
	std::vector<Soldier*> *_list;

	TextButton *_btnOk, *_btnPrev, *_btnNext, *_btnKills, *_btnMissions, *_btnCommendations;
	Window *_window;
	Text *_txtTitle, *_txtMission, *_txtRating, *_txtDate;
	TextList *_lstDiary;

public:
	/// Creates the Soldier Diary state.
	SoldierDiaryOverviewState(Base *base, size_t soldierId, SoldierInfoState *soldierInfoState);
	/// Cleans up the Soldier Diary state.
	~SoldierDiaryOverviewState();
	/// Updates the soldier info.
	void init();
	/// Set the soldier's Id.
	void setSoldierId(size_t soldierId);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
	/// Handler for clicking the Kills button.
	void btnKillsClick(Action *action);
	/// Handler for clicking the Missions button.
	void btnMissionsClick(Action *action);
	/// Handler for clicking the Commendations button.
	void btnCommendationsClick(Action *action);
	/// Handler for clicking on mission list.
	void lstDiaryInfoClick(Action *action);
};

}
