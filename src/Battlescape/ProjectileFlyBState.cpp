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
#include "ProjectileFlyBState.h"
#include "ExplosionBState.h"
#include "Projectile.h"
#include "TileEngine.h"
#include "Map.h"
#include "Pathfinding.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Sound.h"
#include "../Ruleset/RuleItem.h"
#include "../Engine/Options.h"
#include "../Ruleset/Armor.h"
#include "Camera.h"

namespace OpenXcom
{

/**
 * Sets up an ProjectileFlyBState.
 */
ProjectileFlyBState::ProjectileFlyBState(BattlescapeGame *parent, BattleAction action, Position origin) : BattleState(parent, action), _unit(0), _ammo(0), _projectileItem(0), _origin(origin), _autoshotCounter(0), _projectileImpact(0), _initialized(false)
{
}

ProjectileFlyBState::ProjectileFlyBState(BattlescapeGame *parent, BattleAction action) : BattleState(parent, action), _unit(0), _ammo(0), _projectileItem(0), _origin(action.actor->getPosition()), _autoshotCounter(0), _projectileImpact(0), _initialized(false)
{
	;
}


/**
 * Deletes the ProjectileFlyBState.
 */
ProjectileFlyBState::~ProjectileFlyBState()
{

}

/**
 * init sequence:
 * - check if shot is valid
 * - calculate base accuracy
 */
void ProjectileFlyBState::init()
{
	if (_initialized) return;
	_initialized = true;

	BattleItem *weapon = _action.weapon;
	_projectileItem = 0;
	_autoshotCounter = 0;

	if (!weapon) // can't shoot without weapon
		return;

	if (!_parent->getSave()->getTile(_action.target)) // invalid target position
		return;

	if (_action.actor->getTimeUnits() < _action.TU)
	{
		_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
		_parent->popState();
		return;
	}

	_unit = _action.actor;
	_ammo = weapon->getAmmoItem();
	if (_unit->isOut())
	{
		// something went wrong - we can't shoot when dead or unconscious
		_parent->popState();
		return;
	}

	// autoshot will default back to snapshot if it's not possible
	if (weapon->getRules()->getAccuracyAuto() == 0 && _action.type == BA_AUTOSHOT)
		_action.type = BA_SNAPSHOT;

	// snapshot defaults to "hit" if it's a melee weapon
	// (in case of reaction "shots" with a melee weapon)
	if (weapon->getRules()->getBattleType() == BT_MELEE && _action.type == BA_SNAPSHOT)
		_action.type = BA_HIT;

	switch (_action.type)
	{
	case BA_SNAPSHOT:
	case BA_AIMEDSHOT:
	case BA_AUTOSHOT:
	case BA_LAUNCH:
		if (_ammo == 0)
		{
			_action.result = "STR_NO_AMMUNITION_LOADED";
			_parent->popState();
			return;
		}
		if (_ammo->getAmmoQuantity() == 0)
		{
			_action.result = "STR_NO_ROUNDS_LEFT";
			_parent->popState();
			return;
		}
		break;
	case BA_THROW:
		if (!validThrowRange(&_action))
		{
			// out of range
			_action.result = "STR_OUT_OF_RANGE";
			_parent->popState();
			return;
		}
		_projectileItem = weapon;
		break;
	case BA_HIT:
		if (!_parent->getTileEngine()->validMeleeRange(_action.actor->getPosition(), _action.actor->getDirection(), _action.actor->getArmor()->getSize(), _action.actor->getHeight(), 0))
		{
			_action.result = "STR_THERE_IS_NO_ONE_THERE";
			_parent->popState();
			return;
		}
		break;
	case BA_PANIC:
	case BA_MINDCONTROL:
		_parent->statePushFront(new ExplosionBState(_parent, Position((_action.target.x*16)+8,(_action.target.y*16)+8,(_action.target.z*24)+10), weapon, _action.actor));
		return;
	default:
		_parent->popState();
		return;
	}

	if (createNewProjectile() == true)
	{
		BattleAction action;
		BattleUnit *potentialVictim = _parent->getSave()->getTile(_action.target)->getUnit();
		if (potentialVictim && potentialVictim->getFaction() != _unit->getFaction())
		{
			if (_parent->getSave()->getTileEngine()->checkReactionFire(_unit, &action, potentialVictim, false))
			{
				action.cameraPosition = _action.cameraPosition;
				_parent->statePushBack(new ProjectileFlyBState(_parent, action));
			}
		}
	}
}

/**
 * - create a projectile sprite & add it to the map
 * - calculate it's trajectory
 * @return whether it succeeded
 */
bool ProjectileFlyBState::createNewProjectile()
{
	// create a new projectile
	++_action.autoShotCounter;
	Projectile *projectile = new Projectile(_parent->getResourcePack(), _parent->getSave(), _action, _origin);

	_autoshotCounter++;
	// add the projectile on the map
	_parent->getMap()->setProjectile(projectile);
	_parent->setStateInterval(Options::getInt("battleFireSpeed"));

	// let it calculate a trajectory
	_projectileImpact = -1;
	if (_action.type == BA_THROW)
	{
		if (projectile->calculateThrow(_unit->getThrowingAccuracy()))
		{
			_projectileItem->moveToOwner(0);
			_unit->setCache(0);
			_parent->getMap()->cacheUnit(_unit);
			_parent->getResourcePack()->getSound("BATTLE.CAT", 39)->play();
			_unit->addThrowingExp();
		}
		else
		{
			// unable to throw here
			delete projectile;
			_parent->getMap()->setProjectile(0);
			_action.result = "STR_UNABLE_TO_THROW_HERE";
			_parent->popState();
			return false;
		}
	}
	else if (_unit->getType() == "CELATID") // special code for the "spit" trajectory
	{
		if (projectile->calculateThrow(_unit->getFiringAccuracy(_action.type, _action.weapon)))
		{
			// set the soldier in an aiming position
			_unit->aim(true);
			_parent->getMap()->cacheUnit(_unit);
			// and we have a lift-off
			if (_action.weapon->getRules()->getFireSound() != -1)
				_parent->getResourcePack()->getSound("BATTLE.CAT", _action.weapon->getRules()->getFireSound())->play();
		}
		else
		{
			// no line of fire
			delete projectile;
			_parent->getMap()->setProjectile(0);
			_action.result = "STR_NO_LINE_OF_FIRE";
			_parent->popState();
			return false;
		}
	}
	else
	{
		_projectileImpact = projectile->calculateTrajectory(_unit->getFiringAccuracy(_action.type, _action.weapon));
		if (_projectileImpact != -1 || _action.type == BA_LAUNCH)
		{
				// set the soldier in an aiming position
				_unit->aim(true);
				_parent->getMap()->cacheUnit(_unit);
				// and we have a lift-off
				if (_action.weapon->getRules()->getFireSound() != -1)
					_parent->getResourcePack()->getSound("BATTLE.CAT", _action.weapon->getRules()->getFireSound())->play();
				if (!_parent->getSave()->getDebugMode() && _action.type != BA_LAUNCH && _ammo->spendBullet() == false)
				{
					_parent->getSave()->removeItem(_ammo);
					_action.weapon->setAmmoItem(0);
				}
		}
		else
		{
			// no line of fire
			delete projectile;
			_parent->getMap()->setProjectile(0);
			_action.result = "STR_NO_LINE_OF_FIRE";
			_parent->popState();
			return false;
		}
	}

	return true;
}

/**
 * Animates the projectile (move to the next point in it's trajectory).
 * If the animation is finished the projectile sprite is removed from the map.
 * And this state is finished.
 */
void ProjectileFlyBState::think()
{
	/* TODO refactoring : store the projectile in this state, instead of getting it from the map each time? */
	if (_parent->getMap()->getProjectile() == 0)
	{
		if (_action.type == BA_AUTOSHOT && _autoshotCounter < 3 && !_action.actor->isOut() && _ammo->getAmmoQuantity() != 0)
		{
			createNewProjectile();
		}
		else
		{
			if (_action.cameraPosition.z != -1)
			{
				_parent->getMap()->getCamera()->setMapOffset(_action.cameraPosition);
			}
			_parent->popState();
		}
	}
	else
	{
		if(!_parent->getMap()->getProjectile()->move())
		{
			// impact !
			if (_action.type == BA_THROW)
			{
				Position pos = _parent->getMap()->getProjectile()->getPosition(-1);
				pos.x /= 16;
				pos.y /= 16;
				pos.z /= 24;
				BattleItem *item = _parent->getMap()->getProjectile()->getItem();
				_parent->getResourcePack()->getSound("BATTLE.CAT", 38)->play();

				if (Options::getBool("battleInstantGrenade") && item->getRules()->getBattleType() == BT_GRENADE && item->getExplodeTurn() > 0)
				{
					// it's a hot grenade to explode immediately
					_parent->statePushFront(new ExplosionBState(_parent, _parent->getMap()->getProjectile()->getPosition(-1), item, _action.actor));
				}
				else
				{
					_parent->dropItem(pos, item);
				}
			}
			else if (_action.type == BA_LAUNCH && _action.waypoints.size() > 1 && _projectileImpact == -1)
			{
				_origin = _action.waypoints.front();
				_action.waypoints.pop_front();
				_action.target = _action.waypoints.front();
				// launch the next projectile in the waypoint cascade
				_parent->statePushBack(new ProjectileFlyBState(_parent, _action, _origin));
			}
			else
			{
				if (_action.type == BA_LAUNCH && _ammo->spendBullet() == false)
				{
					_parent->getSave()->removeItem(_ammo);
					_action.weapon->setAmmoItem(0);
				}

				if (_projectileImpact != 5) // out of map
				{
					int offset = 0;
					// explosions impact not inside the voxel but one step back
					if (_ammo && (
						_ammo->getRules()->getDamageType() == DT_HE ||
						_ammo->getRules()->getDamageType() == DT_IN))
					{
						offset = -1;
					}
					_parent->statePushFront(new ExplosionBState(_parent, _parent->getMap()->getProjectile()->getPosition(offset), _ammo, _action.actor));
				}
				else
				{
					_unit->aim(false);
					_parent->getMap()->cacheUnits();
				}
			}

			delete _parent->getMap()->getProjectile();
			_parent->getMap()->setProjectile(0);
		}
	}
}

/*
 * Flying projectiles cannot be cancelled.
 */
void ProjectileFlyBState::cancel()
{
}

/*
 * Validate the throwing range.
 * @return true when range is valid.
 */
bool ProjectileFlyBState::validThrowRange(BattleAction *action)
{
	// Throwing Distance roughly = 2.5 \D7 Strength / Weight
	// note that all coordinates and thus also distances below are in number of tiles (not in voxels).
	double maxDistance = 2.5 * action->actor->getStats()->strength / action->weapon->getRules()->getWeight();
	int xdiff = action->target.x - action->actor->getPosition().x;
	int ydiff = action->target.y - action->actor->getPosition().y;
	int zdiff = action->target.z - action->actor->getPosition().z;
	double realDistance = sqrt((double)(xdiff*xdiff)+(double)(ydiff*ydiff));

	// throwing off a building of 1 level lets you throw 2 tiles further than normal range,
	// throwing up the roof of this building lets your throw 2 tiles less further
	realDistance += zdiff*2;

	return realDistance < maxDistance;
}

}
