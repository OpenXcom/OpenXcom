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

#include "UnitWalkBState.h"
#include "ProjectileFlyBState.h"
#include "TileEngine.h"
#include "Pathfinding.h"
#include "BattlescapeState.h"
#include "Map.h"
#include "Camera.h"
#include "BattleAIState.h"
#include "ExplosionBState.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"
#include "../Ruleset/Armor.h"

namespace OpenXcom
{

/**
 * Sets up an UnitWalkBState.
 */
UnitWalkBState::UnitWalkBState(BattlescapeGame *parent, BattleAction action) : BattleState(parent, action), _unit(0), _pf(0), _terrain(0)
{

}

/**
 * Deletes the UnitWalkBState.
 */
UnitWalkBState::~UnitWalkBState()
{

}

void UnitWalkBState::init()
{
	_unit = _action.actor;
	setNormalWalkSpeed();
	_pf = _parent->getPathfinding();
	_terrain = _parent->getTileEngine();
	_target = _action.target;
}

void UnitWalkBState::think()
{
	bool unitspotted = false;
	bool onScreen = (_unit->getVisible() && _parent->getMap()->getCamera()->isOnScreen(_unit->getPosition()));
	if (_unit->isOut())
	{
		_action.strafe = false;
		_pf->abortPath();
		_parent->popState();
		return;
	}

	if (_unit->getStatus() == STATUS_WALKING || _unit->getStatus() == STATUS_FLYING)
	{
		playMovementSound();

		_unit->keepWalking(onScreen); // advances the phase

		// unit moved from one tile to the other, update the tiles
		if (_unit->getPosition() != _unit->getLastPosition())
		{
			int size = _unit->getArmor()->getSize() - 1;
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getSave()->getTile(_unit->getLastPosition() + Position(x,y,0))->setUnit(0);
				}
			}
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getSave()->getTile(_unit->getPosition() + Position(x,y,0))->setUnit(_unit);
				}
			}
			if (!_parent->getMap()->getCamera()->isOnScreen(_unit->getPosition()) && _unit->getFaction() != FACTION_PLAYER && _unit->getVisible())
				_parent->getMap()->getCamera()->centerOnPosition(_unit->getPosition());
			// if the unit changed level, camera changes level with
			_parent->getMap()->getCamera()->setViewHeight(_unit->getPosition().z);
		}

		// is the step finished?
		if (_unit->getStatus() == STATUS_STANDING)
		{
			// if the unit burns floortiles, burn floortiles
			if (_unit->getSpecialAbility() == SPECAB_BURNFLOOR)
			{
				_unit->getTile()->destroy(MapData::O_FLOOR);
			}

			// move our personal lighting with us
			_terrain->calculateUnitLighting();
			unitspotted = _terrain->calculateFOV(_unit);

			BattleAction action;
			
			// check for proximity grenades (1 tile around the unit in every direction) (for large units, we need to check every tile it occupies)
			int size = _unit->getArmor()->getSize() - 1;
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					for (int tx = -1; tx < 2; tx++)
					{
						for (int ty = -1; ty < 2; ty++)
						{
							Tile *t = _parent->getSave()->getTile(_unit->getPosition() + Position(x,y,0) + Position(tx,ty,0));
							if (t)
							for (std::vector<BattleItem*>::iterator i = t->getInventory()->begin(); i != t->getInventory()->end(); ++i)
							{
								if ((*i)->getRules()->getBattleType() == BT_PROXIMITYGRENADE && (*i)->getExplodeTurn() > 0)
								{
									Position p;
									p.x = t->getPosition().x*16 + 8;
									p.y = t->getPosition().y*16 + 8;
									p.z = t->getPosition().z*24 + t->getTerrainLevel();
									_parent->statePushNext(new ExplosionBState(_parent, p, (*i), (*i)->getPreviousOwner()));
									t->getInventory()->erase(i);
									_action.strafe = false;
									return;
								}
							}
						}
					}
				}
			}

			// check for reaction fire
			if (_terrain->checkReactionFire(_unit, &action))
			{
				_parent->statePushBack(new ProjectileFlyBState(_parent, action));
				// unit got fired upon - stop walking
				_action.strafe = false;
				_pf->abortPath();
				return;
			}
		}
		else if (onScreen)
		{
			// make sure the unit sprites are up to date
			if (_pf->getStrafeMove())
			{
				// This is where we fake out the strafe movement direction so the unit "moonwalks"
				int dirTemp = _unit->getDirection();
				_unit->setDirection(_unit->getFaceDirection());
				_parent->getMap()->cacheUnit(_unit);
				_unit->setDirection(dirTemp);
			}
			else
			{
				_parent->getMap()->cacheUnit(_unit);
			}
		}
	}

	// we are just standing around, shouldn't we be walking?
	if (_unit->getStatus() == STATUS_STANDING)
	{
		// check if we did spot new units
		if (unitspotted && _unit->getCharging() == 0)
		{
			_parent->getMap()->cacheUnit(_unit);
			_action.strafe = false;
			_pf->abortPath();
			return;
		}

		if (onScreen || _parent->getSave()->getDebugMode())
		{
			setNormalWalkSpeed();
		}
		else
		{
			_parent->setStateInterval(0);
		}
		int dir = _pf->getStartDirection();
		if ( (dir < 0) || (dir > 7) )
		{
			_action.strafe = false;
		}
		if ((_unit->getTurretType() > -1) && _action.strafe)
		{
			// Turret-and-Ctrl-down, turn the turret instead of moving.
			// TU cost: (in 1/8ths turn) 1 = 1, 2 = 1, 3 = 2, 4 = 2
			// Basically half the cost of actually turning.
			int dirTurr  = _unit->getTurretDirection();
			int dirTurrTo =_unit->getDirectionTo(_action.target);
			// 
			int turnSides = std::min(abs(8 + dirTurr - dirTurrTo), std::min( abs(dirTurrTo - dirTurr), abs(8 +dirTurrTo - dirTurr)));
			int tu = 0;
			if (turnSides == 0)
			{
				_action.strafe = false;
				_unit->abortTurn();
			}
			else
			{
				tu = (turnSides + 1) / 2;
			}
			if (tu > _unit->getTimeUnits() && !_parent->getSave()->getDebugMode())
			{
				_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
				_action.strafe = false;
				_pf->abortPath();
				return;
			}
			// Set up the look, and spend the tu's. This also sets STATUS_TURNING and _toDirectionTurret.
			_unit->lookAt((_action.target), true);
			_unit->spendTimeUnits(tu, _parent->getSave()->getDebugMode());
			// Dequeue everything, because we don't want to walk to the click.
			while (_pf->getStartDirection() > -1)
			{
				dir = _pf->dequeuePath();
			}
			return;
		}
		else if (dir != -1)
		{
			if (_pf->getStrafeMove())
			{
				_unit->setFaceDirection(_unit->getDirection());
			}

			Position destination;
			int tu = _pf->getTUCost(_unit->getPosition(), dir, &destination, _unit, 0); // gets tu cost, but also gets the destination position.
			int energy = tu;

			if (_action.run)
			{
				tu *= 0.75;
				energy *= 1.5;
			}

			if (tu > _unit->getTimeUnits())
			{
				_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
				_action.strafe = false;
				_pf->abortPath();
				_parent->getMap()->cacheUnit(_unit);
				return;
			}

			if (_parent->checkReservedTU(_unit, tu) == false)
			{
				_action.strafe = false;
				_pf->abortPath();
				_parent->getMap()->cacheUnit(_unit);
				return;
			}

			// we are looking in the wrong way, turn first (unless strafing)
			// we are not using the turn state, because turning during walking costs no tu
			if (dir != _unit->getDirection() && dir < Pathfinding::DIR_UP && !_pf->getStrafeMove())
			{
				_unit->lookAt(dir, true);
				return;
			}

			// now open doors (if any)
			int door = _terrain->unitOpensDoor(_unit);
			if (door == 3)
			{
				return; // don't start walking yet, wait for the ufo door to open
			}
			if (door == 0)
			{
				_parent->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(3)->play(); // normal door
			}
			if (door == 1)
			{
				_parent->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(20)->play(); // ufo door
				return; // don't start walking yet, wait for the ufo door to open
			}

			// now start moving
			dir = _pf->dequeuePath();
			if (_unit->spendTimeUnits(tu))
			{
				if (_unit->spendEnergy(energy))
				{
					_unit->startWalking(dir, destination, _parent->getSave()->getTile(destination), onScreen);
				}
				else
				{
					_action.result = "STR_NOT_ENOUGH_ENERGY";
				_parent->getMap()->cacheUnit(_unit);
					_parent->popState();
				}
			}
			else
			{
				_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
				_parent->getMap()->cacheUnit(_unit);
				_parent->popState();
			}
			// make sure the unit sprites are up to date
			if (onScreen)
			{
				if (_pf->getStrafeMove())
				{
					// This is where we fake out the strafe movement direction so the unit "moonwalks"
					int dirTemp = _unit->getDirection();
					_unit->setDirection(_unit->getFaceDirection());
					_parent->getMap()->cacheUnit(_unit);
					_unit->setDirection(dirTemp);
				}
				else
				{
					_parent->getMap()->cacheUnit(_unit);
				}
			}
		}
		else
		{
			postPathProcedures();
			return;
		}
	}

	// turning during walking costs no tu
	if (_unit->getStatus() == STATUS_TURNING)
	{
		if (_action.strafe && _unit->getTurretType() > -1)
		{
			_unit->turn(true);
			if (_unit->getStatus() == STATUS_STANDING)
			{
				_action.strafe = false;
			}
		}
		else
		{
			_unit->turn();
		}

		unitspotted = _terrain->calculateFOV(_unit);

		// make sure the unit sprites are up to date
		if (onScreen)
			_parent->getMap()->cacheUnit(_unit);
		if (unitspotted && _unit->getStatus() != STATUS_PANICKING && _unit->getCharging() == 0)
		{
			_pf->abortPath();
			_parent->getMap()->cacheUnit(_unit);
			_action.strafe = false;
			return;
		}
	}
}

/*
 * Abort unit walking.
 */
void UnitWalkBState::cancel()
{
	_pf->abortPath();
}

/*
 * Handle some calculations when the path is finished.
 */
void UnitWalkBState::postPathProcedures()
{
	if (_unit->getCharging() != 0)
	{
		_unit->lookAt(_unit->getCharging()->getPosition() + Position(_unit->getArmor()->getSize()-1, _unit->getArmor()->getSize()-1, 0), false);
		while (_unit->getStatus() == STATUS_TURNING)
			_unit->turn();
		if (_parent->getTileEngine()->validMeleeRange(_unit, _action.actor->getCharging()))
		{
			_action.target = _action.actor->getCharging()->getPosition();
			_action.weapon = _action.actor->getMainHandWeapon();
			_action.type = BA_HIT;
			_action.TU = _action.actor->getActionTUs(_action.type, _action.weapon);
			_unit->setCharging(0);
		}
	}
	_unit->setCache(0);
	_terrain->calculateUnitLighting();
	_terrain->calculateFOV(_unit);
	_parent->getMap()->cacheUnit(_unit);
	_parent->popState();
	this->_action.strafe = false;
}

/*
 * Handle some calculations when the walking finished.
 */
void UnitWalkBState::setNormalWalkSpeed()
{
	if (_unit->getFaction() == FACTION_PLAYER)
		_parent->setStateInterval(Options::getInt("battleXcomSpeed"));
	else
		_parent->setStateInterval(Options::getInt("battleAlienSpeed"));
}


/*
 * Handle the stepping sounds.
 */
void UnitWalkBState::playMovementSound()
{
	if ((!_unit->getVisible() && !_parent->getSave()->getDebugMode()) || !_parent->getMap()->getCamera()->isOnScreen(_unit->getPosition())) return;

	if (_unit->getMoveSound() != -1)
	{
		// if a sound is configured in the ruleset, play that one
		if (_unit->getWalkingPhase() == 0)
		{
			_parent->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(_unit->getMoveSound())->play();
		}
	}
	else
	{
		if (_unit->getStatus() == STATUS_WALKING)
		{
			// play footstep sound 1
			if (_unit->getWalkingPhase() == 3)
			{
				Tile *tile = _unit->getTile();
				if (tile->getFootstepSound())
				{
					_parent->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(22 + (tile->getFootstepSound()*2))->play();
				}
			}
			// play footstep sound 2
			if (_unit->getWalkingPhase() == 7)
			{
				Tile *tile = _unit->getTile();
				if (tile->getFootstepSound())
				{
					_parent->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(23 + (tile->getFootstepSound()*2))->play();
				}
			}
		}
		else
		{
			// play default flying sound
			if (_unit->getWalkingPhase() == 0)
			{
				_parent->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(15)->play();
			}
		}
	}
}

}
