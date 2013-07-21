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
#define _USE_MATH_DEFINES
#include <cmath>
#include <climits>
#include <algorithm>
#include "AggroBAIState.h"
#include "ProjectileFlyBState.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Battlescape/TileEngine.h"
#include "../Battlescape/BattlescapeState.h"
#include "../Savegame/Tile.h"
#include "../Battlescape/Pathfinding.h"
#include "../Engine/RNG.h"
#include "../Engine/Logger.h"
#include "../Engine/Game.h"
#include "../Ruleset/Armor.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{


std::vector<Position> AggroBAIState::_randomTileSearch;
int AggroBAIState::_randomTileSearchAge = 0xBAD; // data not good yet

/**
 * Sets up a BattleAIState.
 * @param game pointer to the game.
 * @param unit pointer to the unit.
 */
AggroBAIState::AggroBAIState(SavedBattleGame *game, BattleUnit *unit) : BattleAIState(game, unit), _aggroTarget(0), _lastKnownTarget(0), _timesNotSeen(0), _coverCharge(0), _charge(false), _wasHit(false)
{
	_traceAI = _game->getTraceSetting();

    if (_randomTileSearch.size() == 0)
    {
        _randomTileSearch.resize(11*11); // this is currently regenerating this structure once for every alien. Perhaps store it persistently instead?

        for (int i = 0; i < 121; ++i)
        {
            _randomTileSearch[i].x = ((i%11) - 5);
	        _randomTileSearch[i].y = ((i/11) - 5); 
        }
    }

    if (_randomTileSearchAge > 42) // shuffle the search pattern after an arbitrary number of uses
    {

        std::random_shuffle(_randomTileSearch.begin(), _randomTileSearch.end());
        _randomTileSearchAge = 0;
    }
	_coverAction = new BattleAction();
}

/**
 * Deletes the BattleAIState.
 */
AggroBAIState::~AggroBAIState()
{
	delete _coverAction;
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
	node["lastKnownTarget"] >> targetID;
	if (targetID != -1)
	{
		for (std::vector<BattleUnit*>::iterator j = _game->getUnits()->begin(); j != _game->getUnits()->end(); ++j)
		{
			if ((*j)->getId() == targetID)
				_lastKnownTarget = (*j);
		}
	}
	node["lastKnownPosition"][0] >> _lastKnownPosition.x;
	node["lastKnownPosition"][1] >> _lastKnownPosition.y;
	node["lastKnownPosition"][2] >> _lastKnownPosition.z;
	node["timesNotSeen"] >> _timesNotSeen;
	
	_charge = false;
	if (const YAML::Node *chargeNode = node.FindValue("charge")) *chargeNode >> _charge;
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
	if (_lastKnownTarget)
	{
		out << YAML::Key << "lastKnownTarget" << YAML::Value << _lastKnownTarget->getId();
	}
	else
	{
		out << YAML::Key << "lastKnownTarget" << YAML::Value << -1;
	}
	out << YAML::Key << "lastKnownPosition" << YAML::Value << YAML::Flow;
	out << YAML::BeginSeq << _lastKnownPosition.x << _lastKnownPosition.y << _lastKnownPosition.z << YAML::EndSeq;
	out << YAML::Key << "timesNotSeen" << YAML::Value << _timesNotSeen;
	out << YAML::Key << "charge" << YAML::Value << _charge;
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
	// design choices thus far force me to derive the difficulty this way.
	action->diff = (int)(_game->getBattleState()->getGame()->getSavedGame()->getDifficulty());
 	action->type = BA_RETHINK;
	action->actor = _unit;
	// we were hit, but we can't see who did it, try turning around...
	if (_wasHit && _unit->getVisibleUnits()->empty())
	{
		_unit->lookAt(_lastKnownPosition);
		while (_unit->getStatus() == STATUS_TURNING && _unit->getVisibleUnits()->empty())
		{
			_unit->turn();
			_game->getTileEngine()->calculateFOV(_unit);
		}
		_unit->abortTurn();
		_wasHit = false;
	}
	if (_lastKnownTarget && _lastKnownTarget->isOut())
	{
		_lastKnownTarget = 0;
		_lastKnownPosition = Position(0,0,-1);
	}
	_aggroTarget = 0;

	if (_unit->getCharging() && _unit->getCharging()->isOut())
	{
		_unit->setCharging(0);
		_charge = false;
	}
	if (_traceAI) { Log(LOG_INFO) << "AggroBAIState::think() #" << action->number << (_charge ? " [charging]": " ") << (_aggroTarget ? " [aggroTarget]:": " ") << (_aggroTarget ? _aggroTarget->getId() : -1); }
	/* Aggro is mainly either shooting a target or running towards it (melee).
	   If we do no action here - we assume we lost aggro and will go back to patrol state.
	*/
	
	action->weapon = _unit->getMainHandWeapon();
	// the living weapon rule here doubles for "being a terrorist"
	if (_coverCharge == 0 && !(_unit->getOriginalFaction() == FACTION_PLAYER || _unit->getUnitRules()->isLivingWeapon()))
	{
		_coverAction->actor = action->actor;
		_coverAction->number = action->number;
		_coverAction->weapon = action->weapon;
		takeCoverAction(_coverAction);
	}
	if (_unit->getOriginalFaction() != FACTION_PLAYER && _unit->getStats()->psiSkill && RNG::generate(0,3 - (action->diff / 2)) == 0)
	{
		psiAction(action);
	}

	if (action->weapon && action->type == BA_RETHINK)
	{
		_aggroTarget = 0;
		if (action->weapon->getRules()->getBattleType() == BT_MELEE)
		{
			meleeAction(action);
		}
		else if (action->weapon->getRules()->getBattleType() ==  BT_FIREARM)
		{
			if (action->weapon->getAmmoItem() && action->weapon->getAmmoItem()->getAmmoQuantity())
			{
				projectileAction(action);
			}
		}
	}
	// terrorists don't run or hide, they only live to kill.
	if (!(_unit->getOriginalFaction() == FACTION_PLAYER || _unit->getUnitRules()->isLivingWeapon()) && takeCoverAssessment(action))
	{
		_unit->_hidingForTurn = true;
		_aggroTarget = 0;
		if (_traceAI) { Log(LOG_INFO) << "changed my mind, TAKING COVER!"; }
		_coverCharge = 0;
		action->target = _coverAction->target;
		action->type = BA_WALK;
	}
	else if (_unit->getGrenadeFromBelt() && (action->type == BA_SNAPSHOT || action->type == BA_AUTOSHOT) && RNG::generate(0,4 - (action->diff / 2)) == 0)
	{
		grenadeAction(action);
	}
	action->TU = action->actor->getActionTUs(action->type, action->weapon);
	if (_aggroTarget != 0) { setAggroTarget(_aggroTarget); }
	else if (_lastKnownTarget) { stalkingAction(action); }
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
	_lastKnownTarget = unit;
	_lastKnownPosition = unit->getPosition();
}

/*
 * decide if it worth our while to create an explosion here.
 */
bool AggroBAIState::explosiveEfficacy(Position targetPos, BattleUnit *attackingUnit, int radius, int diff)
{
	// i hate the player and i want him dead, but i don't want to piss him off.
	if (_game->getTurn() < 3)
		return false;
	if (diff == -1)
	{
		diff = (int)(_game->getBattleState()->getGame()->getSavedGame()->getDifficulty());
	}
	int distance = _game->getTileEngine()->distance(attackingUnit->getPosition(), targetPos);
	int injurylevel = attackingUnit->getStats()->health - attackingUnit->getHealth();
	int desperation = (100 - attackingUnit->getMorale()) / 10;
	// assume there's one viable target, the one at ground zero.
	int enemiesAffected = 1;
	// if we're below 1/3 health, let's assume things are dire, and increase desperation.
	if (injurylevel > (attackingUnit->getStats()->health / 3) * 2)
		desperation += 3;

	int efficacy = desperation + enemiesAffected;
	if (distance <= radius)
		efficacy -= 3;

	// we don't want to ruin our own base, but we do want to ruin XCom's day.
	if (_game->getMissionType() == "STR_ALIEN_BASE_ASSAULT") efficacy -= 3;
	else if (_game->getMissionType() == "STR_BASE_DEFENSE" || _game->getMissionType() == "STR_TERROR_MISSION") efficacy += 3;


	BattleUnit *target = _game->getTile(targetPos)->getUnit();
	for (std::vector<BattleUnit*>::iterator i = _game->getUnits()->begin(); i != _game->getUnits()->end(); ++i)
	{
		if (!(*i)->isOut() && (*i) != attackingUnit && (*i)->getPosition().z == targetPos.z && _game->getTileEngine()->distance((*i)->getPosition(), targetPos) <= radius)
		{
			Position voxelPosA = Position ((targetPos.x * 16)+8, (targetPos.y * 16)+8, (targetPos.z * 24)+12);
			Position voxelPosB = Position (((*i)->getPosition().x * 16)+8, ((*i)->getPosition().y * 16)+8, ((*i)->getPosition().z * 24)+12);
			int collidesWith = _game->getTileEngine()->calculateLine(voxelPosA, voxelPosB, false, 0, target, true, false, *i);
			if (collidesWith == 4)
			{
				if ((*i)->getFaction() != attackingUnit->getFaction())
				{
					++enemiesAffected;
					++efficacy;
				}
				else
					efficacy -= 2; // friendlies count double
			}
		}
	}
	// spice things up a bit by adding a random number based on difficulty level
	efficacy += RNG::generate(0, diff+1) - RNG::generate(0,2);
	if (efficacy > 0 || enemiesAffected >= 10)
		return true;
	return false;
}

/*
 *	Melee targetting: we can see an enemy, we can move to it so we're charging blindly toward an enemy
 */
void AggroBAIState::meleeAction(BattleAction *action)
{
	_charge = false;
	if (_aggroTarget != 0)
	{
		if (_game->getTileEngine()->validMeleeRange(_unit, _aggroTarget, _unit->getDirection()))
		{
			meleeAttack(action);
			return;
		}
	}
	int attackCost = action->actor->getActionTUs(BA_HIT, _unit->getMainHandWeapon());
	int chargeReserve = _unit->getTimeUnits() - attackCost;
	int distance = (chargeReserve / 4) + 1;
	for (std::vector<BattleUnit*>::iterator j = _unit->getVisibleUnits()->begin(); j != _unit->getVisibleUnits()->end(); ++j)
	{
		int newDistance = _game->getTileEngine()->distance(_unit->getPosition(), (*j)->getPosition());
		//pick closest living unit that we can move to
		if ((newDistance < distance || newDistance == 1) && !(*j)->isOut())
		{
			if (newDistance == 1 || selectPointNearTarget(action, (*j), chargeReserve))
			{
				_aggroTarget = (*j);
				action->type = BA_WALK;
				_charge = true;
				_unit->setCharging(_aggroTarget);
				distance = newDistance;
			}

		}
	}
	if (_aggroTarget != 0)
	{
		if (_game->getTileEngine()->validMeleeRange(_unit, _aggroTarget, _unit->getDirectionTo(_aggroTarget->getPosition())))
		{
			meleeAttack(action);
		}
	}
	if (_traceAI && _aggroTarget) { Log(LOG_INFO) << "AggroBAIState::meleeAction:" << " [target]: " << (_aggroTarget->getId()) << " at: "  << action->target.x << "," << action->target.y << "," << _aggroTarget->getId(); }
	if (_traceAI && _aggroTarget) { Log(LOG_INFO) << "CHARGE!"; }
}

/*	psionic targetting: pick from any of the "exposed" units.
 *	exposed means they have been previously spotted, and are therefore "known" to the AI,
 *	regardless of whether we can see them or not, because we're psychic.
 */
void AggroBAIState::psiAction(BattleAction *action)
{
	int psiAttackStrength = _unit->getStats()->psiSkill * _unit->getStats()->psiStrength / 50;
	int chanceToAttack = 0;
	for (std::vector<BattleUnit*>::const_iterator i = _game->getExposedUnits()->begin(); i != _game->getExposedUnits()->end(); ++i)
	{
		// don't target tanks or other aliens or units under mind control
		if ((*i)->getArmor()->getSize() == 1 && (*i)->getOriginalFaction() == FACTION_PLAYER && (*i)->getFaction() == FACTION_PLAYER)
		{
			int chanceToAttackMe = psiAttackStrength
				+ (((*i)->getStats()->psiSkill > 0) ? (*i)->getStats()->psiSkill * -0.4 : 0)
				- _game->getTileEngine()->distance(_unit->getPosition(), (*i)->getPosition())
				- ((*i)->getStats()->psiStrength)
				+ (RNG::generate(0, 50))
				+ 55;

			if (chanceToAttackMe > chanceToAttack)
			{
				chanceToAttack = chanceToAttackMe;
				_aggroTarget = *i;
			}
		}
	}

	if (!_aggroTarget)
		chanceToAttack = 0;

	if (chanceToAttack)
	{
		if (!_unit->getVisibleUnits()->empty() && _unit->getMainHandWeapon() && _unit->getMainHandWeapon()->getAmmoItem())
		{
			if (_unit->getMainHandWeapon()->getAmmoItem()->getRules()->getPower() >= chanceToAttack)
			{
				chanceToAttack = 0;
				_aggroTarget = 0;
			}
		}
		else
		{
			if (RNG::generate(35, 155) >= chanceToAttack)
			{
				chanceToAttack = 0;
				_aggroTarget = 0;
			}
		}
		if (chanceToAttack >= 30)
		{
			int controlOrPanic = 60;
			int morale = _aggroTarget->getMorale();
			int bravery = (110 - _aggroTarget->getStats()->bravery) / 10;
			if (bravery > 6)
				controlOrPanic += 15;
			if ( bravery < 4)
				controlOrPanic -= 15;
			if (morale >= 40)
			{
				if (morale - 10 * bravery < 50)
					controlOrPanic += 15;
			}
			else
			{
				controlOrPanic -= 15;
			}
			if (!morale)
			{
				controlOrPanic = 0;
			}
			if (RNG::generate(0, 100) >= controlOrPanic)
			{
				action->type = BA_MINDCONTROL;
				action->target = _aggroTarget->getPosition();
			}
			else
			{
				action->type = BA_PANIC;
				action->target = _aggroTarget->getPosition();
			}
		}
		else if (chanceToAttack)
		{
				action->type = BA_PANIC;
				action->target = _aggroTarget->getPosition();
		}
	}
}

/*	
*	waypoint targetting: pick from any units currently spotted by our allies.
*/
void AggroBAIState::wayPointAction(BattleAction *action)
{
	for (std::vector<BattleUnit*>::const_iterator i = _game->getUnits()->begin(); i != _game->getUnits()->end() && _aggroTarget == 0; ++i)
	{
		if ((*i)->getFaction() == _unit->getFaction())
		{
			for (std::vector<BattleUnit*>::const_iterator j = (*i)->getVisibleUnits()->begin(); j != (*i)->getVisibleUnits()->end() && _aggroTarget == 0; ++j)
			{
				_game->getPathfinding()->calculate(_unit, (*j)->getPosition(), *j, -1);
				if (_game->getPathfinding()->getStartDirection() != -1 && explosiveEfficacy((*j)->getPosition(), _unit, (_unit->getMainHandWeapon()->getAmmoItem()->getRules()->getPower()/20)+1, action->diff))
				{
					_aggroTarget = *j;
				}
				_game->getPathfinding()->abortPath();
			}
		}
	}

	if (_aggroTarget != 0)
	{
		action->type = BA_LAUNCH;
		action->TU = action->actor->getActionTUs(action->type, action->weapon);
		if (action->TU > action->actor->getTimeUnits())
		{
			action->type = BA_RETHINK;
			return;
		}
		action->waypoints.clear();

		int PathDirection;
		int CollidesWith;
		Position LastWayPoint = _unit->getPosition();
		Position LastPosition = _unit->getPosition();
		Position CurrentPosition = _unit->getPosition();
		Position DirectionVector;

		_game->getPathfinding()->calculate(_unit, _aggroTarget->getPosition(), _aggroTarget);
		PathDirection = _game->getPathfinding()->dequeuePath();
		while (PathDirection != -1)
		{
			LastPosition = CurrentPosition;
			_game->getPathfinding()->directionToVector(PathDirection, &DirectionVector);
			CurrentPosition = CurrentPosition + DirectionVector;
			Position voxelPosA ((CurrentPosition.x * 16)+8, (CurrentPosition.y * 16)+8, (CurrentPosition.z * 24)+12);
			Position voxelPosb ((LastWayPoint.x * 16)+8, (LastWayPoint.y * 16)+8, (LastWayPoint.z * 24)+12);
			CollidesWith = _game->getTileEngine()->calculateLine(voxelPosA, voxelPosb, false, 0, _unit, true);
			if (CollidesWith > -1 && CollidesWith < 4)
			{
				action->waypoints.push_back(LastPosition);
				LastWayPoint = LastPosition;
			}
			else if (CollidesWith == 4)
			{
				BattleUnit* target = _game->getTile(CurrentPosition)->getUnit();
				if (target == _aggroTarget)
				{
					action->waypoints.push_back(CurrentPosition);
					LastWayPoint = CurrentPosition;
				}
			}

			PathDirection = _game->getPathfinding()->dequeuePath();
		}
		action->target = action->waypoints.front();
		if ((int) action->waypoints.size() > 6 + (action->diff * 2) || LastWayPoint != _aggroTarget->getPosition())
		{
			action->type = BA_RETHINK;
		}
	}
}

/*
 *	Regular targetting: we can see an enemy, we have a gun, let's try to shoot.
 */
void AggroBAIState::projectileAction(BattleAction *action)
{
	if(action->weapon->getRules()->isWaypoint())
	{
		if (action->actor->getType() != "SOLDIER")
			wayPointAction(action);
		return;
	}

	selectNearestTarget();

	if (_aggroTarget != 0)
	{
		_timesNotSeen = 0;
		_lastKnownPosition = _aggroTarget->getPosition();
		action->target = _aggroTarget->getPosition();
		if((action->actor->getFaction() == FACTION_PLAYER && _aggroTarget->getFaction() == FACTION_HOSTILE) || action->actor->getFaction() == FACTION_HOSTILE)
		{
			if (!action->weapon->getAmmoItem()->getRules()->getExplosionRadius() || explosiveEfficacy(_aggroTarget->getPosition(), _unit, action->weapon->getAmmoItem()->getRules()->getExplosionRadius(), action->diff))
			{
				selectFireMethod(action);
			}
		}
	}
}

/*
 * let's evaluate if we could throw a grenade
 */
void AggroBAIState::grenadeAction(BattleAction *action)
{

	// do we have a grenade on our belt?
	BattleItem *grenade = _unit->getGrenadeFromBelt();
	// distance must be more than X tiles, otherwise it's too dangerous to play with explosives
	if (explosiveEfficacy(_aggroTarget->getPosition(), _unit, grenade->getRules()->getExplosionRadius(), action->diff))
	{
		int tu = 4; // 4TUs for picking up the grenade
		if(_unit->getFaction() == FACTION_HOSTILE)
		{
			tu += _unit->getActionTUs(BA_PRIME, grenade);
			tu += _unit->getActionTUs(BA_THROW, grenade);
			// do we have enough TUs to prime and throw the grenade?
			if (tu <= _unit->getStats()->tu)
			{
				// are we within range?
				if (_game->getTileEngine()->validateThrow(action))
				{
					grenade->setExplodeTurn(_game->getTurn());
					_unit->spendTimeUnits(_unit->getActionTUs(BA_PRIME, grenade));
					action->type = BA_THROW;
					action->weapon = grenade;
				}
			}
		}
	}
}

/*
* the idea is to check within a 11x11 tile square for a tile which is not seen by our aggroTarget
* if there is no such tile, we run away from the target.
*/
void AggroBAIState::takeCoverAction(BattleAction *action)
{
	selectNearestTarget();
	if (!_aggroTarget)
	{
		action->type = BA_RETHINK;
		return;
	}
	Uint32 start = SDL_GetTicks();
	int unitsSpottingMe =_game->getSpottingUnits(action->actor);
	action->type = BA_WALK;
	int currentTilePreference = _unit->_hidingForTurn ? action->number * 5 : 0;
	int tries = -1;
	bool coverFound = false;
	int dx = _unit->getPosition().x - _aggroTarget->getPosition().x; // 2d vector in the direction away from the aggro target
	int dy = _unit->getPosition().y - _aggroTarget->getPosition().y;
	int dist = _game->getTileEngine()->distance(_unit->getPosition(), _aggroTarget->getPosition());
    dist = dist ? dist : 1; // division by zero paranoia
	Position runOffset;
	runOffset.x = (dx * 5) / dist;
	runOffset.y = (dy * 5) / dist;
	runOffset.z = 0;
				
	int bestTileScore = -100000;
	int score = -100000;
	Position bestTile(0, 0, 0);
	++_randomTileSearchAge;
	if (action->number > 1) action->desperate = true;
				
	Tile *tile = 0;
				
	const bool civ = _unit->getFaction() == FACTION_NEUTRAL;

	if (_unit->getTimeUnits() <= 5)
	{
		action->target = _unit->getPosition(); // stop wasting our time with calculations when you can't walk anywhere, reapers
		return;
	}
				
	// weights of various factors in choosing a tile to which to withdraw
	const int EXPOSURE_PENALTY = civ ? -20 : 20;
	const int WINDOW_PENALTY = 30;
	const int WALL_BONUS = 1;
	const int FIRE_PENALTY = 40;
	const int SMOKE_PENALTY = 5;
	const int BASE_SYSTEMATIC_SUCCESS = 100;
	const int BASE_DESPERATE_SUCCESS = 110;
	const int FAST_PASS_THRESHOLD = 100; // a score that's good engouh to quit the while loop early; it's subjective, hand-tuned and may need tweaking
	const int MAX_ALLY_DISTANCE = civ ? 1000 : 25; // distance^2 actually
	const int MIN_ALLY_DISTANCE = civ ? 0 : 4; // don't clump up too much and get grenaded, OK?
	const int ALLY_BONUS = civ ? -50 : 4;
	const int SOLDIER_PROXIMITY_BASE_PENALTY = civ ? 0 : 100; // this is divided by distance^2 to nearest soldier
	
	int tu = _coverCharge ? _coverCharge : _unit->getTimeUnits() / 2;

	std::vector<int> reachable = _game->getPathfinding()->findReachable(_unit, tu);

	while (tries < 150 && !coverFound)
	{
		action->target = _unit->getPosition() + runOffset; // start looking in a direction away from the enemy
					
		if (!_game->getTile(action->target))
		{
			action->target = _unit->getPosition(); // cornered at the edge of the map perhaps? 
		}
		
		if (tries == -1)
		{
			// you know, maybe we should just stay where we are and not risk reaction fire... 
			// or maybe continue to wherever we were running to and not risk looking stupid
			if (_game->getTile(_unit->lastCover) != 0)
			{
				action->target = _unit->lastCover;
			} 
		} else if (tries < 121) 
		{
			// looking for cover
			action->target.x += _randomTileSearch[tries].x;
			action->target.y += _randomTileSearch[tries].y;
			if (action->target == _unit->getPosition()) 
			{
				if (unitsSpottingMe > 0)
				{
					// maybe don't stay in the same spot? move or something if there's any point to it?
					action->target.x += RNG::generate(-20,20);
					action->target.y += RNG::generate(-20,20);
				} else
				{
					score += currentTilePreference;
				}
			}
			//score = _game->getTileEngine()->visible(_aggroTarget, _game->getTile(action->target)) ? 0 : 100;
			score = BASE_SYSTEMATIC_SUCCESS; // no need for visible here, the TileEngine code will take care of it
		}
		else
		{
			if (tries == 121) 
			{ 
				action->reckless = true; 
				if (_traceAI) 
				{
					Log(LOG_INFO) << _unit->getId() << " best score after systematic search was: " << bestTileScore; 
				}
			}
						
			score = BASE_DESPERATE_SUCCESS; // ruuuuuuun
			action->target = _unit->getPosition() + runOffset*3;
			action->target.x += RNG::generate(-10,10);
			action->target.y += RNG::generate(-10,10);
			action->target.z = _unit->getPosition().z + RNG::generate(-1,1);
			if (action->target.z < 0)
			{
				action->target.z = 0;
			}
			else if (action->target.z >= _game->getMapSizeZ()) 
			{
				action->target.z = _unit->getPosition().z;
			}
		}

		tries++;
		if (civ) tries += 9; // civilians shouldn't have any tactical sense anyway so save some CPU cycles here

		// THINK, DAMN YOU
		tile = _game->getTile(action->target);
		if (!tile) 
		{
			score = -100001; // no you can't quit the battlefield by running off the map. 
		}
		else
		{
			if (std::find(reachable.begin(), reachable.end(), _game->getTileIndex(tile->getPosition()))  == reachable.end()) continue; // just ignore unreachable tiles

			_game->getTileEngine()->surveyXComThreatToTile(tile, action->target, _unit);
						
			if (tile->soldiersVisible == Tile::NOT_CALCULATED) continue; // you can't go there.
						
			if (tile->soldiersVisible && tile->closestSoldierDSqr <= SOLDIER_PROXIMITY_BASE_PENALTY && tile->closestSoldierDSqr > 0) 
			{
				score -= (SOLDIER_PROXIMITY_BASE_PENALTY/tile->closestSoldierDSqr);
			}
						
			if (tile->soldiersVisible && tile->meanSoldierDSqr <= (SOLDIER_PROXIMITY_BASE_PENALTY/2) && tile->meanSoldierDSqr > 0) 
			{
				score -= ((SOLDIER_PROXIMITY_BASE_PENALTY/2)/tile->meanSoldierDSqr); // less important than above
			}

			//score += (dist-_game->getTileEngine()->distance(_aggroTarget->getPosition(), action->target)); // get away from aggrotarget, modest priority
						
			if (!tile->soldiersVisible)
			{
				// yay.
			} else
			{						
				// score -= tile->soldiersVisible * EXPOSURE_PENALTY;
				score -= EXPOSURE_PENALTY; // that's for giving away our position
				score -= tile->totalExposure / (100 / EXPOSURE_PENALTY); // this is for how easy it'd be to shoot at us
			}
						
			// strength in numbers but not in "grenade us!" huddles:
			if (tile->closestAlienDSqr < MAX_ALLY_DISTANCE && tile->closestAlienDSqr > MIN_ALLY_DISTANCE) score += ALLY_BONUS;
			if (tile->closestAlienDSqr <= MIN_ALLY_DISTANCE) score -= ALLY_BONUS;
										
			if (tile->getFire()) score -= FIRE_PENALTY; // maybe stop, drop, and roll?
						
			if (tile->getSmoke()) score -= SMOKE_PENALTY; // *cough* *cough*
						
			if (tile->getMapData(MapData::O_NORTHWALL) || tile->getMapData(MapData::O_WESTWALL)) score += WALL_BONUS; // hug the walls
						
			if (_game->getTileEngine()->faceWindow(action->target) != -1) score -= WINDOW_PENALTY; // a window is not cover.
                        
			if (_traceAI)
			{
				tile->setMarkerColor(score < 0 ? 3 : (score < FAST_PASS_THRESHOLD/2 ? 8 : (score < FAST_PASS_THRESHOLD ? 9 : 5)));
				tile->setPreview(10);
				tile->setTUMarker(score);
			}
		}

		if (tile && score > bestTileScore)
		{
			// calculate TUs to tile; we could be getting this from findReachable() somehow but that would break something for sure...
			_game->getPathfinding()->calculate(_unit, action->target);
			int TUBonus = (_unit->getTimeUnits() - (_game->getPathfinding()->getTotalTUCost()+4));
			TUBonus = TUBonus > (EXPOSURE_PENALTY - 1) ? (EXPOSURE_PENALTY - 1) : TUBonus;
			if (tile->soldiersVisible == 0 && action->number > 2) score += TUBonus;
			if (score > bestTileScore && _game->getPathfinding()->getStartDirection() != -1)
			{
				bestTileScore = score;
				bestTile = action->target;
				_coverCharge = _game->getPathfinding()->getTotalTUCost();
				if (_traceAI) { tile->setMarkerColor(score < 0 ? 7 : (score < FAST_PASS_THRESHOLD/2 ? 10 : (score < FAST_PASS_THRESHOLD ? 4 : 5))); }
			}
			_game->getPathfinding()->abortPath();
			if (bestTileScore > FAST_PASS_THRESHOLD) coverFound = true; // good enough, gogogo
		}
	}
	action->target = bestTile;
	_unit->lastCover = bestTile;
	if (_traceAI)
	{
		Log(LOG_INFO) << _unit->getId() << " Taking cover with score " << bestTileScore << " after " << tries << " tries, with total exposure " << ((tile=_game->getTile(bestTile)) ? tile->totalExposure : -9999) << ", " << _game->getTileEngine()->distance(_unit->getPosition(), bestTile) << " squares or so away. Time: " << (SDL_GetTicks() - start) << " Action #" << action->number;
		// Log(LOG_INFO) << "Walking " << _game->getTileEngine()->distance(_unit->getPosition(), bestTile) << " squares or so.";
		_game->getTile(action->target)->setMarkerColor(13);
	}
				
	if (bestTileScore <= -100000) 
	{
		coverFound = false;
		_unit->_hidingForTurn = false; 
		action->type = BA_RETHINK; // do something, just don't look dumbstruck :P
		action->TU = 0;
		return;
	}
}

/*
 * if we currently see no target, we either can move to it's last seen position or lose aggro
 */
void AggroBAIState::stalkingAction(BattleAction *action)
{
	if (!_game->eyesOnTarget(_unit->getFaction(), _lastKnownTarget))
	{
		_timesNotSeen++; // come on, guys! teamwork!
	}

	if (_timesNotSeen > _unit->getIntelligence())
	{
		// we lost aggro - going back to patrol state
		_charge = 0;
		_unit->setCharging(0);
		return;
	}
	// lets go looking where we've last seen him
	if (_game->getTile(_lastKnownPosition)->getUnit())
	{
		if(selectPointNearTarget(action, _game->getTile(_lastKnownPosition)->getUnit(), _unit->getTimeUnits()))
		{
			action->type = BA_WALK;
			if (action->weapon && action->weapon->getRules()->getBattleType() == BT_MELEE)
			{
				action->actor->setCharging(_game->getTile(_lastKnownPosition)->getUnit());
			}
		}
		else
		{
			action->type = BA_RETHINK;
		}			
	}
	else
	{
		action->target = _lastKnownPosition;
		action->type = BA_WALK;
	}
}

/*
 * should we take cover from our current target?
*/
bool AggroBAIState::takeCoverAssessment(BattleAction *action)
{
	action->actor->_hidingForTurn = false;

	bool takeCover = true;
	int number = RNG::generate(0,100);
	int unitsSpottingMe = _game->getSpottingUnits(_unit);
	int aggression = _unit->getAggression();

	if (_charge || !_aggroTarget)
		return false;

	// extra 5% chance per unit that sees us
	number += unitsSpottingMe * 5;

	// lost health, chances to take cover get bigger
	if (_unit->getHealth() < _unit->getStats()->health)
		number += 10;


	// aggrotarget has no weapon - chances of take cover get smaller
	if (!_unit->getVisibleUnits()->empty() && _aggroTarget && !_aggroTarget->getMainHandWeapon())
		number -= 50;
	if (aggression == 0 && number < 10)
		takeCover = false;
	if (aggression == 1 && number < 50)
		takeCover = false;
	if (aggression == 2 && number < 90)
		takeCover = false;
	if (action->type == BA_MINDCONTROL || action->type == BA_PANIC)
		takeCover = false;
			

	if (action->number >= 3 && (!_unit->getMainHandWeapon() || _unit->getMainHandWeapon()->getRules()->getBattleType() != BT_MELEE))
	{
        takeCover = true; // always seek cover as last action (unless melee... charge, stupid reapers!)
	}

	// out of ammo or no weapon or ammo at all, we have to take cover
	if (!action->weapon || !action->weapon->getAmmoItem() || !action->weapon->getAmmoItem()->getAmmoQuantity())
	{
		takeCover = true;
	}
	else
	{
		if(action->type != BA_RETHINK && action->type != BA_WALK)
		{
			int tu = action->actor->getActionTUs(action->type, action->weapon);
			// enough time units to shoot?
			if (tu > _unit->getTimeUnits())
			{
				takeCover = true;
			}
		}
	}
	return takeCover;
}

/*
 * pick closest living unit
 */
void AggroBAIState::selectNearestTarget()
{
	for (std::vector<BattleUnit*>::iterator j = _unit->getVisibleUnits()->begin(); j != _unit->getVisibleUnits()->end(); ++j)
	{
		if (!_aggroTarget ||
			_game->getTileEngine()->distance(_unit->getPosition(), (*j)->getPosition()) <
			_game->getTileEngine()->distance(_unit->getPosition(), _aggroTarget->getPosition()))
		{
			if(!(*j)->isOut())
				_aggroTarget = (*j);
		}
	}
}	

/*
 * pick a point near enough to our target to perform a melee attack
 */
bool AggroBAIState::selectPointNearTarget(BattleAction *action, BattleUnit *target, int maxTUs)
{
	int size = action->actor->getArmor()->getSize();
	int targetsize = target->getArmor()->getSize();
	bool returnValue = false;
	int distance = 1000;
	for (int x = -size; x <= targetsize; ++x)
	{
		for (int y = -size; y <= targetsize; ++y)
		{
			if (x || y) // skip the unit itself
			{
				Position checkPath = target->getPosition() + Position (x, y, 0);
				// since vectorToDirection only works with tiles that are neighbours, we'll take the direction from the target instead.
				int dir = target->getDirectionTo(checkPath) + 4;
				if (dir >= 8)
				{
					dir -= 8;
				}
				bool valid = _game->getTileEngine()->validMeleeRange(checkPath, dir, action->actor, target);
				bool fitHere = _game->setUnitPosition(action->actor, checkPath, true);
								
				if (valid && fitHere)
				{
					_game->getPathfinding()->calculate(action->actor, checkPath, 0, maxTUs);
					if (_game->getPathfinding()->getStartDirection() != -1 && _game->getTileEngine()->distance(checkPath, action->actor->getPosition()) < distance)
					{
						action->target = checkPath;
						returnValue = true;
						distance = _game->getTileEngine()->distance(checkPath, action->actor->getPosition());
					}
					_game->getPathfinding()->abortPath();
				}
			}
		}
	}
	return returnValue;
}

/*
 * Perform a melee attack action
 */
void AggroBAIState::meleeAttack(BattleAction *action)
{
	_unit->lookAt(_aggroTarget->getPosition() + Position(_unit->getArmor()->getSize()-1, _unit->getArmor()->getSize()-1, 0), false);
	while (_unit->getStatus() == STATUS_TURNING)
		_unit->turn();
	if (_traceAI) { Log(LOG_INFO) << "Attack unit: " << _aggroTarget->getId(); }
	action->target = _aggroTarget->getPosition();
	action->type = BA_HIT;
	_charge = true;
}

/*
 *	select a fire method based on range, time units, and time units reserved for cover.
 */
void AggroBAIState::selectFireMethod(BattleAction *action)
{
	int distance = _game->getTileEngine()->distance(_unit->getPosition(), action->target);
	action->type = BA_RETHINK;
	int tuAuto = action->weapon->getRules()->getTUAuto();
	int tuSnap = action->weapon->getRules()->getTUSnap();
	int tuAimed = action->weapon->getRules()->getTUAimed();
	int currentTU = action->actor->getTimeUnits() - _coverCharge;

	if (distance < 4)
	{
		if ( tuAuto && currentTU >= action->actor->getActionTUs(BA_AUTOSHOT, action->weapon) )
		{
			action->type = BA_AUTOSHOT;
			return;
		}
		if ( !tuSnap || currentTU < action->actor->getActionTUs(BA_SNAPSHOT, action->weapon) )
		{
			if ( tuAimed && currentTU >= action->actor->getActionTUs(BA_AIMEDSHOT, action->weapon) )
			{
				action->type = BA_AIMEDSHOT;
			}
			return;
		}
		action->type = BA_SNAPSHOT;
		return;
	}


	if ( distance > 12 )
	{
		if ( tuAimed && currentTU >= action->actor->getActionTUs(BA_AIMEDSHOT, action->weapon) )
		{
			action->type = BA_AIMEDSHOT;
			return;
		}
		if ( distance < 20
			&& tuSnap
			&& currentTU >= action->actor->getActionTUs(BA_SNAPSHOT, action->weapon) )
		{
			action->type = BA_SNAPSHOT;
			return;
		}
	}
	
	if ( tuSnap && currentTU >= action->actor->getActionTUs(BA_SNAPSHOT, action->weapon) )
	{
			action->type = BA_SNAPSHOT;
			return;
	}
	if ( tuAimed && currentTU >= action->actor->getActionTUs(BA_AIMEDSHOT, action->weapon) )
	{
			action->type = BA_AIMEDSHOT;
			return;
	}
	if ( tuAuto && currentTU >= action->actor->getActionTUs(BA_AUTOSHOT, action->weapon) )
	{
			action->type = BA_AUTOSHOT;
	}
}

void AggroBAIState::setWasHit(bool wasHit)
{
	_wasHit = wasHit;
}

bool AggroBAIState::getWasHit()
{
	return _wasHit;
}

}
