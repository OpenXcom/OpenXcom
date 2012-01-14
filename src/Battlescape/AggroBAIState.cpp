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
#include "../Battlescape/TileEngine.h"
#include "../Battlescape/Pathfinding.h"
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
 * Loads the AI state from a YAML file.
 * @param node YAML node.
 */
void AggroBAIState::load(const YAML::Node &node)
{
	int targetID;
	node["aggrotarget"] >> targetID;
	if (targetID != -1)
	{
		for (std::vector<BattleUnit*>::iterator j = _game->getUnits()->begin(); j != _game->getUnits()->end(); ++j)
		{
			if ((*j)->getId() == targetID)
				_aggroTarget = (*j);
		}
	}
	node["lastKnownPosition"][0] >> _lastKnownPosition.x;
	node["lastKnownPosition"][1] >> _lastKnownPosition.y;
	node["lastKnownPosition"][2] >> _lastKnownPosition.z;
	node["timesNotSeen"] >> _timesNotSeen;
}

/**
 * Saves the AI state to a YAML file.
 * @param out YAML emitter.
 */
void AggroBAIState::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "state" << YAML::Value << "AGGRO";
	if (_aggroTarget)
	{
		out << YAML::Key << "aggrotarget" << YAML::Value << _aggroTarget->getId();
	}
	else
	{
		out << YAML::Key << "aggrotarget" << YAML::Value << -1;
	}
	out << YAML::Key << "lastKnownPosition" << YAML::Value << YAML::Flow;
	out << YAML::BeginSeq << _lastKnownPosition.x << _lastKnownPosition.y << _lastKnownPosition.z << YAML::EndSeq;
	out << YAML::Key << "timesNotSeen" << YAML::Value << _timesNotSeen;
	out << YAML::EndMap;
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
	int aggression = _unit->getUnit()->getAggression();

	_aggroTarget = 0;
	for (std::vector<BattleUnit*>::iterator j = _unit->getVisibleUnits()->begin(); j != _unit->getVisibleUnits()->end(); ++j)
	{
		_aggroTarget = (*j);
	}

	// if we currently see no target, we either can move to it's last seen position or loose aggro
	if (_aggroTarget == 0)
	{
		_timesNotSeen++;
		if (_timesNotSeen > _unit->getUnit()->getIntelligence() || aggression == 0)
		{
			// we lost aggro
			return;
		}
		// lets go looking where we've last seen him
		action->type = BA_WALK;
		action->target = _lastKnownPosition;
	}
	else
	{
		// if we see the target, we either can shoot him, or take cover.
		bool takeCover = true;
		int number = RNG::generate(0,100);

		// lost health, chances to take cover get bigger
		if (_unit->getHealth() < _unit->getUnit()->getHealth())
			number += 10;

		// out of ammo or no weapon or ammo at all, chance to take cover get bigger
		if (!action->weapon || !action->weapon->getAmmoItem() || !action->weapon->getAmmoItem()->getAmmoQuantity())
			number += 25;

		if (aggression == 0 && number < 10)
			takeCover = false;
		if (aggression == 1 && number < 50)
			takeCover = false;
		if (aggression == 2 && number < 90)
			takeCover = false;

		if (takeCover)
		{
			// the idea is to check within a 5 tile radius for a tile which is not seen by our aggroTarget
			// if there is no such tile, we run away from the target.
			action->type = BA_WALK;
			int tries = 0;
			bool coverFound = false;
			while (tries < 30 && !coverFound)
			{
				tries++;
				action->target = _unit->getPosition();
				action->target.x += RNG::generate(-5,5);
				action->target.y += RNG::generate(-5,5);
				if (tries < 20)
					coverFound = !_game->getTileEngine()->checkIfTileVisible(_aggroTarget, _game->getTile(action->target));
				else
					coverFound = true;

				if (coverFound)
				{
					// check if we can reach this tile
					_game->getPathfinding()->calculate(_unit, action->target);
					if (_game->getPathfinding()->getStartDirection() == -1)
					{
						coverFound = false;
					}
					_game->getPathfinding()->abortPath();
				}
			}
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
				if (RNG::generate(1,10) < 5)
					action->type = BA_SNAPSHOT;
				else
					action->type = BA_AUTOSHOT;
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
 * Note that this does not mean the AI will chase the unit, it will just walk towards this position.
 * Until it forgets about it and goes back to patrolling.
 * @param unit Pointer to the unit.
 */
void AggroBAIState::setAggroTarget(BattleUnit *unit)
{
	_timesNotSeen = 0;
	_lastKnownPosition = unit->getPosition();
}

}
