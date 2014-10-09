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
#ifndef OPENXCOM_SOLDIERDIARYPERFORMANCESTATE_H
#define OPENXCOM_SOLDIERDIARYPERFORMANCESTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;
class SoldierDiaryOverviewState;
class Surface;
class SurfaceSet;
class Soldier;

/**
 * Diary screen that lists totals.
 */
class SoldierDiaryPerformanceState : public State
{
private:
	Base *_base;
	size_t _soldierId;
	SoldierDiaryOverviewState *_soldierDiaryOverviewState;
	Soldier *_soldier;
	std::vector<Soldier*> *_list;

	TextButton *_btnOk, *_btnPrev, *_btnNext, *_btnKills, *_btnMissions, *_btnCommendations;
	Window *_window;
	Text *_txtTitle, *_txtRank, *_txtRace, *_txtWeapon;
	Text *_txtLocation, *_txtType, *_txtUFO;
	Text *_txtMedalName, *_txtMedalLevel, *_txtMedalInfo;
	TextList *_lstRank, *_lstRace, *_lstWeapon, *_lstKillTotals;
	TextList *_lstLocation, *_lstType, *_lstUFO, *_lstMissionTotals;
	TextList *_lstCommendations;
	std::vector<std::wstring> _commendationsListEntry;
	std::vector<Surface*> _commendations, _commendationDecorations;
	SurfaceSet *_commendationSprite, *_commendationDecoration;

	int _display, _lastScrollPos; // 0 displays kills, 1 displays missions, 2 displays commendations
	bool _displayKills, _displayMissions, _displayCommendations;

public:
	/// Creates the Soldiers state.
	SoldierDiaryPerformanceState(Base *base, size_t soldierId, SoldierDiaryOverviewState *soldierDiaryState, int display);
	/// Cleans up the Soldiers state.
	~SoldierDiaryPerformanceState();
	/// Updates the soldier info.
	void init();
	/// Draw sprites.
	void drawSprites();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
	/// Handler for clicking the Kills button.
	void btnKillsToggle(Action *action);
	/// Handler for clicking the Missions button.
	void btnMissionsToggle(Action *action);
	/// Handler for clicking the Missions button.
	void btnCommendationsToggle(Action *action);
	/// Handler for moving the mouse over a medal.
    void lstInfoMouseOver(Action *action);
    /// Handler for moving the mouse outside the medals list.
    void lstInfoMouseOut(Action *action);
	/// Runs state functionality every cycle.
	void think();
};

}

#endif