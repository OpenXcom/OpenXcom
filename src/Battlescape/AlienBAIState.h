/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_ALIENBAISTATE_H
#define OPENXCOM_ALIENBAISTATE_H

#include "BattleAIState.h"
#include "../Battlescape/BattlescapeGame.h"
#include "Position.h"
#include <vector>

namespace OpenXcom
{

class SavedBattleGame;
class BattleUnit;
class BattlescapeState;
class Node;

/**
 * This class is used by the BattleUnit AI.
 */
class AlienBAIState : public BattleAIState
{
protected:
	BattleUnit *_aggroTarget;
	int _knownEnemies, _visibleEnemies, _spottingEnemies;
	int _escapeTUs, _ambushTUs, _reserveTUs;
	BattleAction *_escapeAction, *_ambushAction, *_attackAction, *_patrolAction, *_psiAction;
	bool _rifle, _melee, _blaster;
	bool _traceAI, _didPsi;
	int _AIMode, _intelligence, _closestDist;
	Node *_fromNode, *_toNode;
	std::vector<int> _reachable, _reachableWithAttack, _wasHitBy;
	BattleActionType _reserve;
public:
	/// Creates a new AlienBAIState linked to the game and a certain unit.
	AlienBAIState(SavedBattleGame *save, BattleUnit *unit, Node *node);
	/// Cleans up the AlienBAIState.
	~AlienBAIState();
	/// Loads the AI state from YAML.
	void load(const YAML::Node& node);
	/// Saves the AI state to YAML.
	YAML::Node save() const;
	/// Enters the state.
	void enter();
	/// Exits the state.
	void exit();
	/// Runs state functionality every AI cycle.
	void think(BattleAction *action);
	/// Sets the "unit was hit" flag true.
	void setWasHitBy(BattleUnit *attacker);
	/// Gets whether the unit was hit.
	bool getWasHitBy(int attacker) const;
	/// setup a patrol objective.
	void setupPatrol();
	/// setup an ambush objective.
	void setupAmbush();
	/// setup a combat objective.
	void setupAttack();
	/// setup an escape objective.
	void setupEscape();
	/// count how many xcom/civilian units are known to this unit.
	int countKnownTargets() const;
	/// count how many known XCom units are able to see this unit.
	int getSpottingUnits(Position pos) const;
	/// Selects the nearest target we can see, and return the number of viable targets.
	int selectNearestTarget();
	/// Selects the closest known xcom unit for ambushing.
	bool selectClosestKnownEnemy();
	/// Selects a random known target.
	bool selectRandomTarget();
	/// Selects the nearest reachable point relative to a target.
	bool selectPointNearTarget(BattleUnit *target, int maxTUs) const;
	/// re-evaluate our situation, and make a decision from our available options.
	void evaluateAIMode();
	/// Selects a suitable position from which to attack.
	bool findFirePoint();
	/// Decides if we should throw a grenade/launch a missile to this position.
	bool explosiveEfficacy(Position targetPos, BattleUnit *attackingUnit, int radius, int diff, bool grenade = false) const;
	/// Attempts to take a melee attack/charge an enemy we can see.
	void meleeAction();
	/// Attempts to fire a waypoint projectile at an enemy we, or one of our teammates sees.
	void wayPointAction();
	/// Attempts to fire at an enemy we can see.
	void projectileAction();
	/// Selects a fire method.
	void selectFireMethod();
	/// Attempts to throw a grenade at an enemy (or group of enemies) we can see.
	void grenadeAction();
	/// Performs a psionic attack.
	bool psiAction();
	/// Performs a melee attack action.
	void meleeAttack();
	/// Checks to make sure a target is valid, given the parameters
	bool validTarget(BattleUnit* unit, bool assessDanger, bool includeCivs) const;
	/// Checks the alien's TU reservation setting.
	BattleActionType getReserveMode();
	/// Assuming we have both a ranged and a melee weapon, we have to select one.
	void selectMeleeOrRanged();
};

}

#endif
