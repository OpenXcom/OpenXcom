/*
 * Copyright 2010 OpenXcom Developers.
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
#include <list>
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

enum BattleActionType { BA_NONE, BA_TURN, BA_WALK, BA_PRIME, BA_THROW, BA_AUTOSHOT, BA_SNAPSHOT, BA_AIMEDSHOT, BA_STUN, BA_HIT };

struct BattleAction
{
	BattleActionType type;
	BattleUnit *actor;
	BattleItem *weapon;
	Position target;
	int TU;
	bool targeting;
	int value;
};


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
	InteractiveSurface *_btnEndTurn, *_btnAbort, *_btnStats;
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
	Timer *_stateTimer, *_animTimer;
	SavedBattleGame *_battleGame;
	Text *_txtDebug;
	std::list<BattleState*> _states;
	BattleAction _action;
	BattleActionType _tuReserved;
	std::vector<State*> _popups;
	bool _debugPlay, _playerPanicHandled;

	void selectNextPlayerUnit(bool checkReselect);
	void endTurn();
	void handleItemClick(BattleItem *item);
	void blinkVisibleUnitButtons();
	void handleNonTargetAction();
	void setupCursor();
	bool handlePanickingPlayer();
	bool handlePanickingUnit(BattleUnit *unit);
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
	/// Handler for clicking the map.
	void mapClick(Action *action);
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
	/// handlestates timer.
	void handleState();
	/// Animate other stuff.
	void animate();
	/// Get game.
	Game *getGame() const;
	/// Get map.
	Map *getMap() const;
	/// Push a state at the front of the list.
	void statePushFront(BattleState *bs);
	/// Push a state at the second of the list.
	void statePushNext(BattleState *bs);
	/// Push a state at the back of the list.
	void statePushBack(BattleState *bs);
	/// Remove current state.
	void popState();
	/// Set state think interval.
	void setStateInterval(Uint32 interval);
	/// Show debug message.
	void debug(const std::wstring message);
	/// Handle keypresses.
	void handle(Action *action);
	/// Displays a popup window.
	void popup(State *state);
	/// Checks for casualties in battle.
	bool checkForCasualties(BattleItem *murderweapon, BattleUnit *murderer, bool noSound = false);
	/// Checks if a unit panics.
	void checkForPanic(BattleUnit *unit);
	/// Check reserved tu.
	bool checkReservedTU(BattleUnit *bu, int tu);
	/// Handles unit AI.
	void handleAI(BattleUnit *unit);
	/// Finishes a battle.
	void finishBattle(bool abort);
	/// Add item & affect with gravity.
	void dropItem(const Position &position, BattleItem *item, bool newItem = false);
	/// Check whether TUs should be spent.
	bool dontSpendTUs();
};

}

#endif
