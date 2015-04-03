/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_MELEEATTACKBSTATE_H
#define OPENXCOM_MELEEATTACKBSTATE_H

#include "BattleState.h"
#include "Position.h"

namespace OpenXcom
{

class BattlescapeGame;
class BattleUnit;
class BattleItem;
class Tile;

/**
 * A Melee Attack state.
 */
class MeleeAttackBState : public BattleState
{
private:
	BattleUnit *_unit, *_target;
	BattleItem *_weapon, *_ammo;
	Position _voxel;
	bool _initialized, _deathMessage;
public:
	/// Creates a new MeleeAttackBState class
	MeleeAttackBState(BattlescapeGame *parent, BattleAction action);
	/// Cleans up the MeleeAttackBState.
	~MeleeAttackBState();
	/// Initializes the state.
	void init();
	/// Runs state functionality every cycle.
	void think();
	/// Performs a melee attack
	void performMeleeAttack();
	/// Determine if the attack hit, and if so, do stuff.
	void resolveHit();

};

}

#endif
