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
#define _USE_MATH_DEFINES
#include <cmath>
#include "AggroBAIState.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Sets up a BattleAIState.
 * @param game pointer to the game.
 * @param unit pointer to the unit.
 */
AggroBAIState::AggroBAIState(SavedBattleGame *game, BattleUnit *unit) : BattleAIState(game, unit), _aggroTarget(0), _timesNotSeen(0)
{

}

/**
 * Deletes the BattleAIState.
 */
AggroBAIState::~AggroBAIState()
{
	
}

/**
 * Enters the current AI state.
 */
void AggroBAIState::enter()
{
	// ROOOAARR !

}


/**
 * Exits the current AI state.
 */
void AggroBAIState::exit()
{

}

/**
 * Runs any code the state needs to keep updating every
 * AI cycle.
 * @param action (possible) AI action to execute after thinking is done.
 */
void AggroBAIState::think(BattleAction *action)
{
	action->type = BA_NONE;
	action->actor = _unit;
	/* Aggro is mainly either shooting a target or running towards it (melee).
	   If we do no action here - we assume we lost aggro and will go back to patrol state.
	*/

	_aggroTarget = 0;
	for (std::vector<BattleUnit*>::iterator j = _unit->getVisibleUnits()->begin(); j != _unit->getVisibleUnits()->end(); ++j)
	{
		_aggroTarget = (*j);
	}

	if (_aggroTarget == 0)
	{
		_timesNotSeen++;
		if (_timesNotSeen > 3) // this should be the intelligence level of the unit
		{
			// we lost aggro
			return;
		}
		// lets go looking in the neighbourhood where we've last seen him
		action->type = BA_WALK;
		action->target = _lastKnownPosition;
		action->target.x += RNG::generate(-1,1);
		action->target.y += RNG::generate(-1,1);
	}
	else
	{
		// from time to time, don't shoot him, but just move a bit around
		if (RNG::generate(1,6) == 1)
		{
			action->type = BA_WALK;
			action->target = _unit->getPosition();
			action->target.x += RNG::generate(-5,5);
			action->target.y += RNG::generate(-5,5);
		}
		else
		{
			_timesNotSeen = 0;
			_lastKnownPosition = _aggroTarget->getPosition();
			action->target = _aggroTarget->getPosition();
			action->weapon = action->actor->getMainHandWeapon();
			/*int tu = action->actor->getActionTUs(action->type, action->weapon);*/
			if (action->weapon && action->weapon->getAmmoItem() && action->weapon->getAmmoItem()->getAmmoQuantity())
			{
				action->type = BA_SNAPSHOT;
			}
			else
			{
				// now we are in trouble, flee
				return;
			}
		}
	}

}

/**
 * Sets the aggro target to be used by the AI.
 * @param unit Pointer to the unit.
 */
void AggroBAIState::setAggroTarget(BattleUnit *unit)
{
	_timesNotSeen = 0;
	_lastKnownPosition = unit->getPosition();
}

}
