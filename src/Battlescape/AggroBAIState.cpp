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
#define _USE_MATH_DEFINES
#include <cmath>
#include "AggroBAIState.h"
#include "ProjectileFlyBState.h"
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
	int aggression = _unit->getAggression();

	_aggroTarget = 0;
	for (std::vector<BattleUnit*>::iterator j = _unit->getVisibleUnits()->begin(); j != _unit->getVisibleUnits()->end(); ++j)
	{
		//pick closest living unit
		if (!_aggroTarget || _game->getTileEngine()->distance(_unit->getPosition(), (*j)->getPosition()) < _game->getTileEngine()->distance(_unit->getPosition(), _aggroTarget->getPosition()))
		{
			if(!(*j)->isOut())
			_aggroTarget = (*j);
		}
	}
	// if we currently see no target, we either can move to it's last seen position or lose aggro
	if (_aggroTarget == 0)
	{
		_timesNotSeen++;
		if (_timesNotSeen > _unit->getIntelligence() || aggression == 0)
		{
			// we lost aggro - going back to patrol state
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
		if (_unit->getHealth() < _unit->getStats()->health)
			number += 10;

		// aggrotarget has no weapon - chances of take cover get smaller
		if (!_aggroTarget->getMainHandWeapon())
			number -= 50;
		if (aggression == 0 && number < 10)
			takeCover = false;
		if (aggression == 1 && number < 50)
			takeCover = false;
		if (aggression == 2 && number < 90)
			takeCover = false;
		
		// we're using melee, so CHAAAAAAAARGE!!!!!
		if (_unit->getMainHandWeapon() && _unit->getMainHandWeapon()->getRules()->getBattleType() == BT_MELEE)
			if (_game->getTileEngine()->distance(_unit->getPosition(), _aggroTarget->getPosition()) > 1)
				takeCover = true;
			else
				takeCover = false;
		//if distance ==1 attack instead
		if (!takeCover)
		{
			_timesNotSeen = 0;
			_lastKnownPosition = _aggroTarget->getPosition();
			action->target = _aggroTarget->getPosition();
			action->type = BA_NONE;

			// lets' evaluate if we could throw a grenade
			int tu = 4; // 4TUs for picking up the grenade

			// distance must be more than 6 tiles, otherwise it's too dangerous to play with explosives
			if (_game->getTileEngine()->distance(_unit->getPosition(), _aggroTarget->getPosition()) > 6)
			{
				if((_unit->getFaction() == FACTION_NEUTRAL && _aggroTarget->getFaction() == FACTION_HOSTILE) || _unit->getFaction() == FACTION_HOSTILE)
				{
				// do we have a grenade on our belt?
				BattleItem *grenade = _unit->getGrenadeFromBelt();
				// do we have enough TUs to prime and throw the grenade?
				if (grenade && RNG::generate(0,2) == 0)
				{
					action->weapon = grenade;
					tu += _unit->getActionTUs(BA_PRIME, grenade);
					tu += _unit->getActionTUs(BA_THROW, grenade);
					if (tu <= _unit->getStats()->tu)
					{
						// are we within range?
						if (ProjectileFlyBState::validThrowRange(action))
						{
							grenade->setExplodeTurn(_game->getTurn());
							_unit->spendTimeUnits(_unit->getActionTUs(BA_PRIME, grenade), false);
							action->type = BA_THROW;
						}
					}
				}
				}
			}

			if (action->type == BA_NONE)
			{
				action->weapon = action->actor->getMainHandWeapon();
				// out of ammo or no weapon or ammo at all, we have to take cover
				// no need for an ammo check here, getmainweapon does that already
				if (!action->weapon)
				{
					takeCover = true;
				}
				else
				{
					if(((_unit->getFaction() == FACTION_NEUTRAL && _aggroTarget->getFaction() == FACTION_HOSTILE) || _unit->getFaction() == FACTION_HOSTILE))
					{
						if (action->weapon->getRules()->getBattleType() == BT_MELEE)
							action->type = BA_HIT;
						else if (RNG::generate(1,10) < 5)
							action->type = BA_SNAPSHOT;
						else
							action->type = BA_AUTOSHOT;
						tu = action->actor->getActionTUs(action->type, action->weapon);
						// enough time units to shoot?
						if (tu > _unit->getTimeUnits())
						{
							takeCover = true;
						}
					}
					else
						takeCover = true;
				}
			}
		}

		if (takeCover)
		{
			// the idea is to check within a 5 tile radius for a tile which is not seen by our aggroTarget
			// if there is no such tile, we run away from the target.
			// unless we use melee, in which case, try to get within one tile of our target asap.
			action->type = BA_WALK;
			int tries = 0;
			int i = -1;
			int j = -1;
			bool coverFound = false;
			if(action->actor->getMainHandWeapon() && action->actor->getMainHandWeapon()->getRules()->getBattleType() == BT_MELEE )
			{
				for (int i = -1; i < 2; i++)
					for (int j = -1; j < 2; j++)
					{
						Position checkPath = Position (_aggroTarget->getPosition().x+i, _aggroTarget->getPosition().y+j, _aggroTarget->getPosition().z);
						_game->getPathfinding()->calculate(_unit, checkPath);
						if (_game->getPathfinding()->getStartDirection() != -1)
							if (_game->getTileEngine()->distance(_unit->getPosition(), checkPath) < _game->getTileEngine()->distance(_unit->getPosition(), action->target))
								action->target = checkPath;
						_game->getPathfinding()->abortPath();
					}
			}
			else
			while (tries < 30 && !coverFound)
			{
				tries++;
				action->target = _unit->getPosition();
				action->target.x += RNG::generate(-5,5);
				action->target.y += RNG::generate(-5,5);
				if (tries < 20)

					coverFound = !_game->getTileEngine()->visible(_aggroTarget, _game->getTile(action->target));
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
	}

	action->TU = action->actor->getActionTUs(action->type, action->weapon);
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
