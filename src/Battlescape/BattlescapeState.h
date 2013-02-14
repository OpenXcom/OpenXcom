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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_BATTLESCAPESTATE_H
#define OPENXCOM_BATTLESCAPESTATE_H

#include "../Engine/State.h"
#include "Position.h"

#include <vector>
#include <string>

namespace OpenXcom
{

class Surface;
class Map;
class ImageButton;
class InteractiveSurface;
class Text;
class Bar;
class NumberText;
class BattleUnit;
class SavedBattleGame;
class BattleItem;
class Window;
class BattleState;
class Timer;
class ActionMenuItem;
class WarningMessage;
class BattlescapeGame;

/**
 * Battlescape screen which shows the tactical battle
 */
class BattlescapeState : public State
{
private:
	Surface *_icons, *_rank;
	Map *_map;
	InteractiveSurface *_btnUnitUp, *_btnUnitDown, *_btnMapUp, *_btnMapDown, *_btnShowMap, *_btnKneel;
	InteractiveSurface *_btnInventory, *_btnCenter, *_btnNextSoldier, *_btnNextStop, *_btnShowLayers, *_btnHelp;
	InteractiveSurface *_btnEndTurn, *_btnAbort, *_btnStats, *_btnLaunch, *_btnPsi;
	ImageButton *_reserve;
	ImageButton *_btnReserveNone, *_btnReserveSnap, *_btnReserveAimed, *_btnReserveAuto;
	InteractiveSurface *_btnLeftHandItem, *_btnRightHandItem;
	InteractiveSurface *_btnVisibleUnit[10];
	NumberText *_numVisibleUnit[10];
	BattleUnit *_visibleUnit[10];
	WarningMessage *_warning;
	Text *_txtName;
	NumberText *_numTimeUnits, *_numEnergy, *_numHealth, *_numMorale, *_numLayers, *_numAmmoLeft, *_numAmmoRight;
	Bar *_barTimeUnits, *_barEnergy, *_barHealth, *_barMorale;
	Timer *_animTimer, *_gameTimer;
	SavedBattleGame *_save;
	Text *_txtDebug;
	std::vector<State*> _popups;
	BattlescapeGame *_battleGame;
	bool firstInit;
	bool isMouseScrolling;
	bool isMouseScrolled;
	int xBeforeMouseScrolling, yBeforeMouseScrolling;
	Position mapOffsetBeforeMouseScrolling;
	Uint32 mouseScrollingStartTime;
	int totalMouseMoveX, totalMouseMoveY;
	int lastSucTotalMouseMoveX, lastSucTotalMouseMoveY;
	bool mouseMovedOverThreshold;

	void selectNextPlayerUnit(bool checkReselect, bool setReselect);
	void selectPreviousPlayerUnit(bool checkReselect);
	void handleItemClick(BattleItem *item);
	void blinkVisibleUnitButtons();
public:
	static const int DEFAULT_ANIM_SPEED = 100;
	/// Creates the Battlescape state.
	BattlescapeState(Game *game);
	/// Cleans up the Battlescape state.
	~BattlescapeState();
	/// init
	void init();
	/// think
	void think();
	/// Handler for moving mouse over the map.
	void mapOver(Action *action);
	/// Handler for pressing the map.
	void mapPress(Action *action);
	/// Handler for clicking the map.
	void mapClick(Action *action);
	/// Handler for entering with mouse to the map surface.
	void mapIn(Action *action);
	/// Handler for clicking the Unit Up button.
	void btnUnitUpClick(Action *action);
	/// Handler for clicking the Unit Down button.
	void btnUnitDownClick(Action *action);
	/// Handler for clicking the Map Up button.
	void btnMapUpClick(Action *action);
	/// Handler for clicking the Map Down button.
	void btnMapDownClick(Action *action);
	/// Handler for clicking the Show Map button.
	void btnShowMapClick(Action *action);
	/// Handler for clicking the Kneel button.
	void btnKneelClick(Action *action);
	/// Handler for clicking the Soldier button.
	void btnInventoryClick(Action *action);
	/// Handler for clicking the Center button.
	void btnCenterClick(Action *action);
	/// Handler for clicking the Next Soldier button.
	void btnNextSoldierClick(Action *action);
	/// Handler for clicking the Next Stop button.
	void btnNextStopClick(Action *action);
	/// Handler for clicking the Show Layers button.
	void btnShowLayersClick(Action *action);
	/// Handler for clicking the Help button.
	void btnHelpClick(Action *action);
	/// Handler for clicking the End Turn button.
	void btnEndTurnClick(Action *action);
	/// Handler for clicking the Abort button.
	void btnAbortClick(Action *action);
	/// Handler for clicking the stats.
	void btnStatsClick(Action *action);
	/// Handler for clicking the left hand item button.
	void btnLeftHandItemClick(Action *action);
	/// Handler for clicking the right hand item button.
	void btnRightHandItemClick(Action *action);
	/// Handler for clicking a visible unit button.
	void btnVisibleUnitClick(Action *action);
	/// Handler for clicking the launch rocket button.
	void btnLaunchClick(Action *action);
	/// Handler for clicking the use psi button.
	void btnPsiClick(Action *action);
	/// Handler for clicking a reserved button.
	void btnReserveNoneClick(Action *action);
	/// Handler for clicking a reserved button.
	void btnReserveSnapClick(Action *action);
	/// Handler for clicking a reserved button.
	void btnReserveAimedClick(Action *action);
	/// Handler for clicking a reserved button.
	void btnReserveAutoClick(Action *action);
	// playable unit selected?
	bool playableUnitSelected();
	/// updates soldier name/rank/tu/energy/health/morale
	void updateSoldierInfo();
	/// Animate other stuff.
	void animate();
	/// handlestates timer.
	void handleState();
	/// state timer interval.
	void setStateInterval(Uint32 interval);
	/// Get game.
	Game *getGame() const;
	/// Get map.
	Map *getMap() const;
	/// Show debug message.
	void debug(const std::wstring &message);
	/// Show warning message.
	void warning(const std::string &message);
	/// Handle keypresses.
	void handle(Action *action);
	/// Displays a popup window.
	void popup(State *state);
	/// Finishes a battle.
	void finishBattle(bool abort, int inExitArea);
	/// show launch button
	void showLaunchButton(bool show);
	/// show PSI button
	void showPsiButton(bool show);
	/// Clears mouse-scrolling state
	void clearMouseScrollingState();

	void SaveVoxelMap();
	void SaveVoxelView();

};

}

#endif
