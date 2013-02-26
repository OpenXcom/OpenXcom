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
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "UnitTurnBState.h"
#include "TileEngine.h"
#include "BattlescapeState.h"
#include "Map.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"
#include "../Engine/Language.h"
#include "../Engine/Options.h"
#include "../Ruleset/Armor.h"

namespace OpenXcom
{

/**
 * Sets up an UnitTurnBState.
 */
UnitTurnBState::UnitTurnBState(BattlescapeGame *parent, BattleAction action) : BattleState(parent, action), _unit(0), _turret(false)
{

}

/**
 * Deletes the UnitTurnBState.
 */
UnitTurnBState::~UnitTurnBState()
{

}

void UnitTurnBState::init()
{
	_unit = _action.actor;
	if (_unit->getFaction() == FACTION_PLAYER)
		_parent->setStateInterval(Options::getInt("battleXcomSpeed"));
	else
		_parent->setStateInterval(Options::getInt("battleAlienSpeed"));

	// if the unit has a turret and we are turning during targeting, then only the turret turns
	_turret = _unit->getTurretType() != -1 && _action.targeting || _action.strafe;

	_unit->lookAt(_action.target, _turret);

	if (_unit->getStatus() != STATUS_TURNING)
	{
		// try to open a door
		int door = _parent->getTileEngine()->unitOpensDoor(_unit, true);
		if (door == 0)
		{
			_parent->getResourcePack()->getSound("BATTLE.CAT", 3)->play(); // normal door
		}
		if (door == 1)
		{
			_parent->getResourcePack()->getSound("BATTLE.CAT", RNG::generate(20,21))->play(); // ufo door
		}
		if (door == 4)
		{
			_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
		}
		_parent->popState();
	}
}

void UnitTurnBState::think()
{
	const int tu = 1; // one turn is 1 tu

	if (_parent->checkReservedTU(_unit, tu) == false)
	{
		_unit->abortTurn();
		_parent->popState();
		return;
	}

	if (_unit->spendTimeUnits(tu))
	{
		_unit->turn(_turret);
		_parent->getTileEngine()->calculateFOV(_unit);
		_parent->getMap()->cacheUnit(_unit);
		if (_unit->getStatus() == STATUS_STANDING)
		{
			_parent->popState();
		}
	}
	else
	{
		_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
		_unit->abortTurn();
		_parent->popState();
	}
}

/*
 * Unit turning cannot be cancelled.
 */
void UnitTurnBState::cancel()
{
}

}
