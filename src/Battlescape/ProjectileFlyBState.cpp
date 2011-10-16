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
#include "ProjectileFlyBState.h"
#include "ExplosionBState.h"
#include "Projectile.h"
#include "TileEngine.h"
#include "Map.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Ruleset/RuleItem.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Sets up an ProjectileFlyBState.
 */
ProjectileFlyBState::ProjectileFlyBState(BattlescapeState *parent, BattleAction action) : BattleState(parent), _unit(0), _ammo(0), _action(action), _projectileImpact(0), _initialized(false)
{

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

	if (_action.actor->getTimeUnits() < _action.TU && !_parent->dontSpendTUs())
	{
		_result = "STR_NOT_ENOUGH_TIME_UNITS";
		_parent->popState();
		return;
	}

	_unit = _action.actor;
	_ammo = weapon->getAmmoItem();
	if (_unit->isOut())
	{
		// something went wrong
		_parent->popState();
		return;
	}
	if (_action.type != BA_THROW)
	{
		if (_ammo == 0)
		{
			_result = "STR_NO_AMMUNITION_LOADED";
			_parent->popState();
			return;
		}
		if (_ammo->getAmmoQuantity() == 0)
		{
			_result = "STR_NO_ROUNDS_LEFT";
			_parent->popState();
			return;
		}
	}
	// action specific initialisation
	switch (_action.type)
	{
	case BA_AUTOSHOT:
		_baseAcc = weapon->getRules()->getAccuracyAuto();
		break;
	case BA_SNAPSHOT:
		_baseAcc = weapon->getRules()->getAccuracySnap();
		break;
	case BA_AIMEDSHOT:
		_baseAcc = weapon->getRules()->getAccuracyAimed();
		break;
	case BA_THROW:
		if (!validThrowRange())
		{
			// out of range
			_result = "STR_OUT_OF_RANGE";
			_parent->popState();
			return;
		}
		_baseAcc = (int)(_unit->getThrowingAccuracy()*100.0);
		_projectileItem = weapon;
		break;
    default:
        _baseAcc = 0;
	}

	createNewProjectile();

	BattleAction action;
	BattleUnit *potentialVictim = _parent->getGame()->getSavedGame()->getBattleGame()->getTile(_action.target)->getUnit();
	if (potentialVictim && potentialVictim->getFaction() != _unit->getFaction())
	{
		if (_parent->getGame()->getSavedGame()->getBattleGame()->getTileEngine()->checkReactionFire(_unit, &action, potentialVictim, false))
		{
			_parent->statePushBack(new ProjectileFlyBState(_parent, action));
		}
	}
}

/**
 * - create a projectile sprite & add it to the map
 * - calculate it's trajectory
 */
void ProjectileFlyBState::createNewProjectile()
{
	// create a new projectile
	Projectile *projectile = new Projectile(_parent->getGame()->getResourcePack(),
									_parent->getGame()->getSavedGame()->getBattleGame(),
									_action);

	_autoshotCounter++;
	// add the projectile on the map
	_parent->getMap()->setProjectile(projectile);
	_parent->setStateInterval(Options::getInt("battleFireSpeed"));

	// let it calculate a trajectory
	_projectileImpact = -1;
	if (_action.type == BA_THROW)
	{
		if (projectile->calculateThrow(_baseAcc))
		{
			_projectileItem->moveToOwner(0);
			_unit->setCache(0);
			_parent->getMap()->cacheUnit(_unit);
			_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(39)->play();
			_unit->addThrowingExp();
		}
		else
		{
			// unable to throw here
			delete projectile;
			_parent->getMap()->setProjectile(0);
			_result = "STR_UNABLE_TO_THROW_HERE";
			_parent->popState();
			return;
		}
	}
	else
	{
		_projectileImpact = projectile->calculateTrajectory(_unit->getFiringAccuracy(_baseAcc));
		if (_projectileImpact != -1)
		{
				// set the soldier in an aiming position
				_unit->aim(true);
				_parent->getMap()->cacheUnit(_unit);
				// and we have a lift-off
				_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(_action.weapon->getRules()->getFireSound())->play();
				if (!_parent->getGame()->getSavedGame()->getBattleGame()->getDebugMode() && _ammo->spendBullet() == false)
				{
					_parent->getGame()->getSavedGame()->getBattleGame()->removeItem(_ammo);
					_action.weapon->setAmmoItem(0);
				}
		}
		else
		{
			// no line of fire
			delete projectile;
			_parent->getMap()->setProjectile(0);
			_result = "STR_NO_LINE_OF_FIRE";
			_parent->popState();
			return;
		}
	}
}

/**
 * Animates the projectile (move to the next point in it's trajectory).
 * If the animation is finished the projectile sprite is removed from the map.
 * And this state is finished.
 */
void ProjectileFlyBState::think()
{
	if (_parent->getMap()->getProjectile() == 0)
	{
		if (_action.type == BA_AUTOSHOT && _autoshotCounter < 3 && !_action.actor->isOut())
		{
			createNewProjectile();
		}
		else
		{
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
				_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(38)->play();

				if (Options::getBool("battleAltGrenade") && item->getRules()->getBattleType() == BT_GRENADE)
				{
					// it's a hot grenade to explode immediatly
					_parent->statePushFront(new ExplosionBState(_parent, _parent->getMap()->getProjectile()->getPosition(-1), item, _action.actor));
				}
				else
				{
					_parent->dropItem(pos, item);
				}
			}
			else
			{
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
					if (_parent->getMap()->didCameraFollow() && _parent->getGame()->getSavedGame()->getBattleGame()->getSide() == FACTION_PLAYER)
					{
						_parent->getMap()->centerOnPosition(_parent->getGame()->getSavedGame()->getBattleGame()->getSelectedUnit()->getPosition());
					}
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
 * Get the action result. Returns error messages or an empty string when everything went fine.
 * @return returnmessage Empty when everything is fine.
 */
std::string ProjectileFlyBState::getResult() const
{
	return _result;
}

/*
 * Validate the throwing range. 
 * @return true when range is valid.
 */
bool ProjectileFlyBState::validThrowRange()
{
	// Throwing Distance roughly = 2.5 × Strength / Weight
	// note that all coordinates and thus also distances below are in number of tiles (not in voxels).
	double maxDistance = 2.5 * _action.actor->getUnit()->getStrength() / _action.weapon->getRules()->getWeight();
	int xdiff = _action.target.x - _unit->getPosition().x;
	int ydiff = _action.target.y - _unit->getPosition().y;
	int zdiff = _action.target.z - _unit->getPosition().z;
	double realDistance = sqrt((double)(xdiff*xdiff)+(double)(ydiff*ydiff));

	// throwing off a building of 1 level lets you throw 2 tiles further than normal range,
	// throwing up the roof of this building lets your throw 2 tiles less further
	realDistance += zdiff*2;

	return realDistance < maxDistance;
}

}