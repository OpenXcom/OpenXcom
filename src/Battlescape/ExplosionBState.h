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
#ifndef OPENXCOM_EXPLOSIONBSTATE_H
#define OPENXCOM_EXPLOSIONBSTATE_H

#include "BattleState.h"
#include "Position.h"
#include <string>

namespace OpenXcom
{

class BattlescapeGame;
class BattleUnit;
class BattleItem;
class Tile;

/**
 * Explosion state not only handles explosions, but also bullet impacts!
 * Refactoring tip : ImpactBState.
 */
class ExplosionBState : public BattleState
{
private:
	BattleUnit *_unit;
	Position _center;
	BattleItem *_item;
	Tile *_tile;
	int _power;
	double _varPower;
	bool _areaOfEffect, _lowerWeapon;
	BattleAction *_action;
	/// Calculates the effects of the explosion.
	void explode();
public:
	/// Creates a new ExplosionBState class.
	ExplosionBState(BattlescapeGame *parent, Position center, BattleItem *item, BattleUnit *unit, Tile *tile = 0, bool lowerWeapon = false);
	ExplosionBState(BattlescapeGame *parent, Position center, BattleAction *action, BattleItem *item, BattleUnit *unit, Tile *tile = 0, bool lowerWeapon = false);
	/// Cleans up the ExplosionBState.
	~ExplosionBState();
	/// Initializes the state.
	void init();
	/// Handles a cancel request.
	void cancel();
	/// Runs state functionality every cycle.
	void think();
	/// Gets the result of the state.
	std::string getResult() const;

};

}

#endif
