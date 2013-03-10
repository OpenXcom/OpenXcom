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
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Logger.h"
#include "UnitFallBState.h"

namespace OpenXcom
{

/**
 * Sets up an UnitWalkBState.
 */
UnitWalkBState::UnitWalkBState(BattlescapeGame *parent, BattleAction action, const Position finalFacing, const bool pathfindForFinalTurn) : BattleState(parent, action), _unit(0), _pf(0), _terrain(0), _falling(false), _finalFacing(finalFacing), _pathfindForFinalTurn(pathfindForFinalTurn)
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
	if (_parent->getSave()->getTraceSetting()) { Log(LOG_INFO) << "Walking from: " << _unit->getPosition().x << "," << _unit->getPosition().y << "," << _unit->getPosition().z << "," << " to " << _target.x << "," << _target.y << "," << _target.z;}
}

void UnitWalkBState::think()
{
	bool unitspotted = false;
	bool onScreen = (_unit->getVisible() && _parent->getMap()->getCamera()->isOnScreen(_unit->getPosition()));
	Tile *tileBelow = _parent->getSave()->getTile(_unit->getPosition() + Position(0,0,-1));

	if (_unit->isOut())
	{
		_pf->abortPath();
		_parent->popState();
		return;
	}

	if (_unit->getStatus() == STATUS_WALKING || _unit->getStatus() == STATUS_FLYING)
	{
		if ((_parent->getSave()->getTile(_unit->getDestination())->getUnit() == 0) || // next tile must be not occupied
			(_parent->getSave()->getTile(_unit->getDestination())->getUnit() == _unit))
		{
			playMovementSound();
			_unit->keepWalking(tileBelow, onScreen); // advances the phase
		}
		else if (!_falling)
		{
			_unit->lookAt(_unit->getDestination(), true);	// turn to undiscovered unit
			_pf->abortPath();
		}

		// unit moved from one tile to the other, update the tiles
		if (_unit->getPosition() != _unit->getLastPosition())
		{
			int size = _unit->getArmor()->getSize() - 1;
			bool largeCheck = true;
			bool visibilityFlag = false;
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					Tile *otherTileBelow = _parent->getSave()->getTile(_unit->getPosition() + Position(x,y,-1));
					if (!_parent->getSave()->getTile(_unit->getPosition() + Position(x,y,0))->hasNoFloor(otherTileBelow) || _unit->getArmor()->getMovementType() == MT_FLY)
						largeCheck = false;
					_parent->getSave()->getTile(_unit->getLastPosition() + Position(x,y,0))->setUnit(0);
				}
			}
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getSave()->getTile(_unit->getPosition() + Position(x,y,0))->setUnit(_unit, _parent->getSave()->getTile(_unit->getPosition() + Position(x,y,-1)));
					if (_parent->getSave()->getTile(_unit->getPosition())->getVisible())
						visibilityFlag = true;
				}
			}

			_unit->setVisible(visibilityFlag);

			_falling = largeCheck && _unit->getPosition().z != 0 && _unit->getTile()->hasNoFloor(tileBelow) && _unit->getArmor()->getMovementType() != MT_FLY && _unit->getWalkingPhase() == 0;
			
			if (_falling)
			{
				for (int x = _unit->getArmor()->getSize() - 1; x >= 0; --x)
				{
					for (int y = _unit->getArmor()->getSize() - 1; y >= 0; --y)
					{
						Tile *otherTileBelow = _parent->getSave()->getTile(_unit->getPosition() + Position(x,y,-1));
						if (otherTileBelow && otherTileBelow->getUnit())
						{
							Position originalPosition(otherTileBelow->getUnit()->getPosition());
							for (int dir = 0; dir < Pathfinding::DIR_UP; dir++)
							{
								Position offset;
								Pathfinding::directionToVector(dir, &offset);
								Tile *t = _parent->getSave()->getTile(originalPosition + offset);
								Tile *bt = _parent->getSave()->getTile(originalPosition + offset + Position(0,0,-1));
								Tile *bu = _parent->getSave()->getTile(originalPosition + Position(0,0,-1));
								if (t && !_parent->getPathfinding()->isBlocked(otherTileBelow, t, dir, 0) && t->getUnit() == 0 && (!t->hasNoFloor(bt) || otherTileBelow->getUnit()->getArmor()->getMovementType() == MT_FLY))
								{
									_falling = false;
									_pf->dequeuePath();
									otherTileBelow->getUnit()->startWalking(dir, t->getPosition(), t, bu, bt, onScreen);
									_parent->getSave()->addFallingUnit(otherTileBelow->getUnit());
									_parent->getSave()->addFallingUnit(_unit);
									_parent->statePushFront(new UnitFallBState(_parent));
									return;
								}
							}
						}
					}
				}
			}

			if (!_parent->getMap()->getCamera()->isOnScreen(_unit->getPosition()) && _unit->getFaction() != FACTION_PLAYER && _unit->getVisible())
				_parent->getMap()->getCamera()->centerOnPosition(_unit->getPosition());
			// if the unit changed level, camera changes level with
			_parent->getMap()->getCamera()->setViewLevel(_unit->getPosition().z);
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
									return;
								}
							}
						}
					}
				}
			}

			// check for reaction fire
			if (!_falling && !_action.reckless && _terrain->checkReactionFire(_unit, &action))
			{
				postPathProcedures();
				action.cameraPosition = _parent->getMap()->getCamera()->getMapOffset();
				_parent->statePushBack(new ProjectileFlyBState(_parent, action));
				// unit got fired upon - stop walking
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
	if (_unit->getStatus() == STATUS_STANDING || _unit->getStatus() == STATUS_PANICKING)
	{
		// check if we did spot new units
		if (unitspotted && !_action.desperate && _unit->getCharging() == 0 && !_falling)
		{
			if (_parent->getSave()->getTraceSetting()) { Log(LOG_INFO) << "Uh-oh! Company!"; }			
			_unit->_hidingForTurn = false; // clearly we're not hidden now
			_parent->getMap()->cacheUnit(_unit);
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
		if (_falling)
		{
			dir = Pathfinding::DIR_DOWN;
		}

		if (dir != -1)
		{
			if (_pf->getStrafeMove())
			{
				_unit->setFaceDirection(_unit->getDirection());
			}

			Position destination;
			int tu = _pf->getTUCost(_unit->getPosition(), dir, &destination, _unit, 0, false); // gets tu cost, but also gets the destination position.
			if (_falling)
			{
				tu = 0;
			}
			int energy = tu;
			if (_action.run)
			{
				tu *= 0.75;
				energy *= 1.5;
			}

			if (tu > _unit->getTimeUnits())
			{
				_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
				_pf->abortPath();
				_parent->getMap()->cacheUnit(_unit);
				return;
			}

			if (_parent->checkReservedTU(_unit, tu) == false)
			{
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
				_parent->getResourcePack()->getSound("BATTLE.CAT", 3)->play(); // normal door
			}
			if (door == 1)
			{
				_parent->getResourcePack()->getSound("BATTLE.CAT", 20)->play(); // ufo door
				return; // don't start walking yet, wait for the ufo door to open
			}

			// now start moving
			dir = _pf->dequeuePath();
			if (_falling)
			{
				dir = Pathfinding::DIR_DOWN;
			}

			if (_unit->spendTimeUnits(tu))
			{
				if (_unit->spendEnergy(energy))
				{
					Tile *tileBelow = _parent->getSave()->getTile(_unit->getPosition() + Position(0,0,-1));
					Tile *tileBelowDestination = _parent->getSave()->getTile(destination + Position(0,0,-1));
					_unit->startWalking(dir, destination, _parent->getSave()->getTile(destination), tileBelow, tileBelowDestination, onScreen);
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
					_unit->setDirection(dirTemp);
				}
				_parent->getMap()->cacheUnit(_unit);
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
		}
		else
		{
			_unit->turn();
		}

		unitspotted = _terrain->calculateFOV(_unit);

		// make sure the unit sprites are up to date
		if (onScreen)
			_parent->getMap()->cacheUnit(_unit);
		if (unitspotted && !(_action.desperate || _unit->getCharging()) && !_falling)
		{
			if (_parent->getSave()->getTraceSetting()) { Log(LOG_INFO) << "Egads! A turn reveals new units! I must pause!"; }
			_unit->_hidingForTurn = false; // not hidden, are we...
			_pf->abortPath();
			_parent->getMap()->cacheUnit(_unit);
			return;
		}
	}
}

/*
 * Abort unit walking.
 */
void UnitWalkBState::cancel()
{
	if (_parent->getSave()->getSide() == FACTION_PLAYER && _parent->getPanicHandled())
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
		if (_parent->getTileEngine()->validMeleeRange(_unit, _action.actor->getCharging(), _unit->getDirection()))
		{
			_action.target = _action.actor->getCharging()->getPosition();
			_action.weapon = _action.actor->getMainHandWeapon();
			_action.type = BA_HIT;
			_action.TU = _action.actor->getActionTUs(_action.type, _action.weapon);
			_unit->setCharging(0);
		}
	} else if (_parent->getSave()->getTile(_finalFacing) != 0) // check that _finalFacing points to a valid tile; out of bounds value indicates no final turn
    {
        if (_pathfindForFinalTurn)
        {        
            // if we can't see the target, try to face where they might come from        
            _pf->abortPath();
            _pf->calculate(_unit, _finalFacing, _parent->getSave()->getTile(_finalFacing)->getUnit());

            if (_pf->getStartDirection() != -1)
            {
                _unit->lookAt(_pf->getStartDirection(), false);
            } else
            {
                _unit->lookAt(_finalFacing);
            }
            _pf->abortPath();
        } else
        {
            _unit->lookAt(_finalFacing); // this duplicated call looks weird but let's not run the pathfinding code if we don't have to; lookAt(), otoh, is very cheap
        }

        while (_unit->getStatus() == STATUS_TURNING) // cheat-turn by recommendation of warboy; use no time-units to face our foes in battle and such
            _unit->turn();


    }

	_unit->setCache(0);
	_terrain->calculateUnitLighting();
	_terrain->calculateFOV(_unit);
	_parent->getMap()->cacheUnit(_unit);
	if (!_falling)
		_parent->popState();
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
			_parent->getResourcePack()->getSound("BATTLE.CAT", _unit->getMoveSound())->play();
		}
	}
	else
	{
		if (_unit->getStatus() == STATUS_WALKING)
		{
			Tile *tile = _unit->getTile();
			Tile *tileBelow = _parent->getSave()->getTile(tile->getPosition() + Position(0,0,-1));
			// play footstep sound 1
			if (_unit->getWalkingPhase() == 3)
			{
				if (tile->getFootstepSound(tileBelow))
				{
					_parent->getResourcePack()->getSound("BATTLE.CAT", 22 + (tile->getFootstepSound(tileBelow)*2))->play();
				}
			}
			// play footstep sound 2
			if (_unit->getWalkingPhase() == 7)
			{
				if (tile->getFootstepSound(tileBelow))
				{
					_parent->getResourcePack()->getSound("BATTLE.CAT", 23 + (tile->getFootstepSound(tileBelow)*2))->play();
				}
			}
		}
		else
		{
			// play default flying sound
			if (_unit->getWalkingPhase() == 0 && !_falling)
			{
				_parent->getResourcePack()->getSound("BATTLE.CAT", 15)->play();
			}
		}
	}
}

}
