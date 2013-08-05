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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_BATTLESCAPEGAME_H
#define OPENXCOM_BATTLESCAPEGAME_H

#include "Position.h"
#include <SDL.h>
#include <string>
#include <list>

namespace OpenXcom
{

class BattleUnit;
class SavedBattleGame;
class BattleItem;
class BattleState;
class BattlescapeState;
class Timer;
class ResourcePack;
class Map;
class TileEngine;
class Pathfinding;
class Ruleset;
class InfoboxOKState;

enum BattleActionType { BA_NONE, BA_TURN, BA_WALK, BA_PRIME, BA_THROW, BA_AUTOSHOT, BA_SNAPSHOT, BA_AIMEDSHOT, BA_STUN, BA_HIT, BA_USE, BA_LAUNCH, BA_MINDCONTROL, BA_PANIC, BA_RETHINK };

struct BattleAction
{
	BattleActionType type;
	BattleUnit *actor;
	BattleItem *weapon;
	Position target;
	std::list<Position> waypoints;
	int TU;
	bool targeting;
	int value;
	std::string result;
	bool strafe, run;
	int diff;
	int autoShotCounter;
	Position cameraPosition;
    bool desperate; // ignoring newly-spotted units
    bool reckless; // ignoring reaction fire!
    int number; // first action of turn, second, etc.?
	BattleAction() : type(BA_NONE), actor(0), weapon(0), TU(0), targeting(false), value(0), result(""), strafe(false), run(false), diff(0), autoShotCounter(0), cameraPosition(0, 0, -1), desperate(false), reckless(false) { }
};

/**
 * Battlescape game - the core game engine of the battlescape game
 */
class BattlescapeGame
{
private:
	SavedBattleGame *_save;
	BattlescapeState *_parentState;
	std::list<BattleState*> _states;
	BattleActionType _tuReserved, _playerTUReserved;
	bool _playerPanicHandled;
	int _AIActionCounter;
	BattleAction _currentAction;

	void selectNextPlayerUnit(bool checkReselect);
	void endTurn();
	bool handlePanickingPlayer();
	bool handlePanickingUnit(BattleUnit *unit);
	bool noActionsPending(BattleUnit *bu);
	std::vector<InfoboxOKState*> _infoboxQueue;
	void showInfoBoxQueue();
	bool _playedAggroSound, _endTurnRequested, _kneelReserved;
public:
	/// Creates the BattlescapeGame state.
	BattlescapeGame(SavedBattleGame *save, BattlescapeState *parentState);
	/// Cleans up the BattlescapeGame state.
	~BattlescapeGame();
	/// think.
	void think();
	void init();
	// playable unit selected?
	bool playableUnitSelected();
	/// handlestates timer.
	void handleState();
	/// Push a state at the front of the list.
	void statePushFront(BattleState *bs);
	/// Push a state at the second of the list.
	void statePushNext(BattleState *bs);
	/// Push a state at the back of the list.
	void statePushBack(BattleState *bs);
	/// Handles the result of non target actions, like priming a grenade.
	void handleNonTargetAction();
	/// Remove current state.
	void popState();
	/// Set state think interval.
	void setStateInterval(Uint32 interval);
	/// Checks for casualties in battle.
	void checkForCasualties(BattleItem *murderweapon, BattleUnit *murderer, bool hiddenExplosion = false, bool terrainExplosion = false);
	/// Checks if a unit panics.
	void checkForPanic(BattleUnit *unit);
	/// Check reserved tu.
	bool checkReservedTU(BattleUnit *bu, int tu, bool justChecking = false);
	/// Handles unit AI.
	void handleAI(BattleUnit *unit);
	/// Add item & affect with gravity.
	void dropItem(const Position &position, BattleItem *item, bool newItem = false, bool removeItem = false);
	/// Convert a unit into a unit of another type.
	BattleUnit *convertUnit(BattleUnit *unit, std::string newType);
	/// Handle kneeling action.
	bool kneel(BattleUnit *bu);
	/// Cancel whatever action we were going at.
	bool cancelCurrentAction(bool bForce = false);
	/// Get pointer to access action members directly.
	BattleAction *getCurrentAction();
	/// Is there currently an action going on.
	bool isBusy();
	/// Activate primary action (left click)
	void primaryAction(const Position &pos);
	/// Activate secondary action (right click)
	void secondaryAction(const Position &pos);
	/// Pressed the launch action.
	void launchAction();
	/// Pressed the psi action.
	void psiButtonAction();
	/// Move a unit up or down.
	void moveUpDown(BattleUnit *unit, int dir);
	/// Request of the end of the turn (wait for explosions etc to really end the turn)
	void requestEndTurn();
	/// Set the TU reserved type.
	void setTUReserved(BattleActionType tur);
	/// Sets up the cursor taking into account the action.
	void setupCursor();
	/// Getters:
	Map *getMap();
	SavedBattleGame *getSave();
	TileEngine *getTileEngine();
	Pathfinding *getPathfinding();
	ResourcePack *getResourcePack();
	const Ruleset *getRuleset() const;
	/// this method evaluates the threats from XCom soldiers to tiles, for later use by AI.
	void resetSituationForAI();
	static bool _debugPlay;
	/// is panic done with yet?
	bool getPanicHandled() { return _playerPanicHandled; }
	/// try to find and pick up an item.
	void findItem(BattleAction *action);
	/// check through all the items on the ground and pick one.
	BattleItem *surveyItems(BattleAction *action);
	/// evaluate if it's worth while to take this item.
	bool worthTaking(BattleItem* item, BattleAction *action);
	/// pick the item up from the ground.
	int takeItemFromGround(BattleItem* item, BattleAction *action);
	/// assign the item to a slot (stolen from battlescapeGenerator::addItem())
	bool takeItem(BattleItem* item, BattleAction *action);
	/// return the type of action that is reserved.
	BattleActionType getReservedAction();
	/// tally the living units, convert them if necessary.
	void tallyUnits(int &liveAliens, int &liveSoldiers, bool convert);
	/// set the kneel reservation setting.
	void setKneelReserved(bool reserved);
	/// check the kneel reservation setting.
	bool getKneelReserved();
	/// attempt a psionic attack on an enemy we "know of".
	bool psiAction(BattleAction *action);

};

}

#endif
