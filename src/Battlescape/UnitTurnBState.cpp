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
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "UnitTurnBState.h"
#include "TerrainModifier.h"
#include "BattlescapeState.h"
#include "Map.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Sets up an UnitTurnBState.
 */
UnitTurnBState::UnitTurnBState(BattlescapeState *parent) : BattleState(parent)
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
	_parent->setStateInterval(DEFAULT_WALK_SPEED);
	_unit = _parent->getGame()->getSavedGame()->getBattleGame()->getSelectedUnit();
	_unit->lookAt(_parent->getTarget());
	if (_unit->getStatus() != STATUS_TURNING)
	{
		// try to open a door
		int door = _parent->getGame()->getSavedGame()->getBattleGame()->getTerrainModifier()->unitOpensDoor(_unit);
		if (door == 0)
		{
			_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(3)->play(); // normal door
		}
		if (door == 1)
		{
			_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(RNG::generate(20,21))->play(); // ufo door
		}
		_parent->popState();
	}
}

void UnitTurnBState::think()
{
	if (_unit->spendTimeUnits(1))
	{
		_unit->turn();
		_parent->getGame()->getSavedGame()->getBattleGame()->getTerrainModifier()->calculateFOV(_unit);
		_parent->getMap()->cacheUnits();
		if (_unit->getStatus() == STATUS_STANDING)
		{
			_parent->popState();
		}
	}
	else
	{
		_unit->abortTurn();
		_result = "STR_NOT_ENOUGH_TIME_UNITS";
		_parent->popState();
	}
}

/*
 * Unit turning cannot be cancelled.
 */
void UnitTurnBState::cancel()
{
}

/*
 * Get the action result. Returns error messages or an empty string when everything went fine.
 * @return returnmessage Empty when everything is fine.
 */
std::string UnitTurnBState::getResult() const
{
	return _result;
}

}
