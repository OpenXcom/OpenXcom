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

#include "UnitPanicBState.h"
#include "../Savegame/BattleUnit.h"

namespace OpenXcom
{

/**
 * Sets up an UnitPanicBState.
 * @param parent Pointer to the Battlescape.
 * @param unit Panicking unit.
 */
UnitPanicBState::UnitPanicBState(BattlescapeGame *parent, BattleUnit *unit) : BattleState(parent), _unit(unit)
{
	// nothing to see here
}

/**
 * Deletes the UnitPanicBState.
 */
UnitPanicBState::~UnitPanicBState()
{
}

void UnitPanicBState::init()
{
}

/**
 * Runs state functionality every cycle.
 * Ends the panicking when done.
 */
void UnitPanicBState::think()
{
	// reset the unit's time units when all panicking is done
	if (_unit)
	{
		if (!_unit->isOut())
		{
			_unit->abortTurn(); // set the unit status to standing in case it wasn't otherwise changed from berserk/panicked
		}
		_unit->setTimeUnits(0);
	}
	_parent->popState();
	_parent->setupCursor();
}

/**
 * Panicking cannot be cancelled.
 */
void UnitPanicBState::cancel()
{
}

}
