/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "../Savegame/BattleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Sound.h"
#include "../Engine/Options.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Logger.h"
#include "../Ruleset/Ruleset.h"
#include "UnitFallBState.h"

namespace OpenXcom
{

/**
 * Sets up an UnitWalkBState.
 * @param parent Pointer to the Battlescape.
 * @param action Pointer to an action.
 */
UnitWalkBState::UnitWalkBState(BattlescapeGame *parent, BattleAction action) : BattleState(parent, action), _unit(0), _pf(0), _terrain(0), _falling(false), _beforeFirstStep(false), _numUnitsSpotted(0), _preMovementCost(0)
{

}

/**
 * Deletes the UnitWalkBState.
 */
UnitWalkBState::~UnitWalkBState()
{

}

/**
 * Initializes the state.
 */
void UnitWalkBState::init()
{
	_unit = _action.actor;
	_numUnitsSpotted = _unit->getUnitsSpottedThisTurn().size();
	setNormalWalkSpeed();
	_pf = _parent->getPathfinding();
	_terrain = _parent->getTileEngine();
	_target = _action.target;
	if (Options::traceAI) { Log(LOG_INFO) << "Walking from: " << _unit->getPosition() << "," << " to " << _target;}
	int dir = _pf->getStartDirection();
	if (!_action.strafe && dir != -1 && dir != _unit->getDirection())
	{
		_beforeFirstStep = true;
	}
}

/**
 * Runs state functionality every cycle.
 */
void UnitWalkBState::think()
{
	bool unitSpotted = false;
	bool onScreen = (_unit->getVisible() && _parent->getMap()->getCamera()->isOnScreen(_unit->getPosition(), true));
	if (_unit->isKneeled())
	{
		if (_parent->kneel(_unit))
		{
			_unit->setCache(0);
			_terrain->calculateFOV(_unit);
			_parent->getMap()->cacheUnit(_unit);
			return;
		}
		else
		{
			_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
			_pf->abortPath();
			_parent->popState();
			return;
		}
	}
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
			_unit->lookAt(_unit->getDestination(), (_unit->getTurretType() != -1));	// turn to undiscovered unit
			_pf->abortPath();
		}

		// unit moved from one tile to the other, update the tiles
		if (_unit->getPosition() != _unit->getLastPosition())
		{
			int size = _unit->getArmor()->getSize() - 1;
			bool largeCheck = true;
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
				}
			}
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
							_falling = false;
							_pf->dequeuePath();
							_parent->getSave()->addFallingUnit(_unit);
							_parent->statePushFront(new UnitFallBState(_parent));
							return;
						}
					}
				}
			}

			if (!_parent->getMap()->getCamera()->isOnScreen(_unit->getPosition(), true) && _unit->getFaction() != FACTION_PLAYER && _unit->getVisible())
				_parent->getMap()->getCamera()->centerOnPosition(_unit->getPosition());
			// if the unit changed level, camera changes level with
			_parent->getMap()->getCamera()->setViewLevel(_unit->getPosition().z);
		}

		// is the step finished?
		if (_unit->getStatus() == STATUS_STANDING)
		{
			// update the TU display
			_parent->getSave()->getBattleState()->updateSoldierInfo();
			// if the unit burns floortiles, burn floortiles
			if (_unit->getSpecialAbility() == SPECAB_BURNFLOOR)
			{
				_unit->getTile()->ignite(1);
				Position here = (_unit->getPosition() * Position(16,16,24)) + Position(8,8,-(_unit->getTile()->getTerrainLevel()));
				_parent->getTileEngine()->hit(here, _unit->getStats()->strength, _parent->getRuleset()->getDamageType(DT_IN), _unit);
			}

			// move our personal lighting with us
			_terrain->calculateUnitLighting();
			if (_unit->getFaction() != FACTION_PLAYER)
			{
				_unit->setVisible(false);
			}
			_terrain->calculateFOV(_unit->getPosition());
			unitSpotted = (!_falling && !_action.desperate && _parent->getPanicHandled() && _numUnitsSpotted != _unit->getUnitsSpottedThisTurn().size());

			if (_parent->checkForProximityGrenades(_unit))
			{
				_parent->popState();
				return;
			}
			if (unitSpotted)
			{
				_unit->setCache(0);
				_parent->getMap()->cacheUnit(_unit);
				_pf->abortPath();
				_parent->popState();
				return;
			}
			// check for reaction fire
			if (!_falling)
			{
				if (_terrain->checkReactionFire(_unit))
				{
					// unit got fired upon - stop walking
					_unit->setCache(0);
					_parent->getMap()->cacheUnit(_unit);
					_pf->abortPath();
					_parent->popState();
					return;
				}
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
		if (unitSpotted && !_action.desperate && _unit->getCharging() == 0 && !_falling)
		{
			if (Options::traceAI) { Log(LOG_INFO) << "Uh-oh! Company!"; }
			_unit->_hidingForTurn = false; // clearly we're not hidden now
			_parent->getMap()->cacheUnit(_unit);
			postPathProcedures();
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
			if (_unit->getFaction() == FACTION_HOSTILE &&
				_parent->getSave()->getTile(destination)->getFire() > 0)
			{
				tu -= 32; // we artificially inflate the TU cost by 32 points in getTUCost under these conditions, so we have to deflate it here.
			}
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
			if (dir >= Pathfinding::DIR_UP)
			{
				energy = 0;
			}
			if (tu > _unit->getTimeUnits())
			{
				if (_parent->getPanicHandled() && tu < 255)
				{
					_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
				}
				_pf->abortPath();
				_unit->setCache(0);
				_parent->getMap()->cacheUnit(_unit);
				_parent->popState();
				return;
			}

			if (energy / 2 > _unit->getEnergy())
			{
				if (_parent->getPanicHandled())
				{
					_action.result = "STR_NOT_ENOUGH_ENERGY";
				}
				_pf->abortPath();
				_unit->setCache(0);
				_parent->getMap()->cacheUnit(_unit);
				_parent->popState();
				return;
			}

			if (_parent->getPanicHandled() && _parent->checkReservedTU(_unit, tu) == false)
			{
				_pf->abortPath();
				_unit->setCache(0);
				_parent->getMap()->cacheUnit(_unit);
				return;
			}

			// we are looking in the wrong way, turn first (unless strafing)
			// we are not using the turn state, because turning during walking costs no tu
			if (dir != _unit->getDirection() && dir < Pathfinding::DIR_UP && !_pf->getStrafeMove())
			{
				_unit->lookAt(dir);
				_unit->setCache(0);
				_parent->getMap()->cacheUnit(_unit);
				return;
			}

			// now open doors (if any)
			if (dir < Pathfinding::DIR_UP)
			{
				int door = _terrain->unitOpensDoor(_unit, false, dir);
				if (door == 3)
				{
					return; // don't start walking yet, wait for the ufo door to open
				}
				if (door == 0)
				{
					_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), ResourcePack::DOOR_OPEN)->play(); // normal door
				}
				if (door == 1)
				{
					_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), ResourcePack::SLIDING_DOOR_OPEN)->play(); // ufo door
					return; // don't start walking yet, wait for the ufo door to open
				}
			}
			for (int x = _unit->getArmor()->getSize() - 1; x >= 0; --x)
			{
				for (int y = _unit->getArmor()->getSize() - 1; y >= 0; --y)
				{
					BattleUnit* unitInMyWay = _parent->getSave()->getTile(destination + Position(x,y,0))->getUnit();
					BattleUnit* unitBelowMyWay = 0;
					Tile* belowDest = _parent->getSave()->getTile(destination + Position(x,y,-1));
					if (belowDest)
					{
						unitBelowMyWay = belowDest->getUnit();
					}
					// can't walk into units in this tile, or on top of other units sticking their head into this tile
					if (!_falling &&
						((unitInMyWay && unitInMyWay != _unit)
						|| (belowDest && unitBelowMyWay && unitBelowMyWay != _unit &&
						(-belowDest->getTerrainLevel() + unitBelowMyWay->getFloatHeight() + unitBelowMyWay->getHeight())
						>= 28)))  // 4+ voxels poking into the tile above, we don't kick people in the head here at XCom.
					{
						_action.TU = 0;
						_pf->abortPath();
						_unit->setCache(0);
						_parent->getMap()->cacheUnit(_unit);
						_parent->popState();
						return;
					}
				}
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
					_unit->startWalking(dir, destination, tileBelow, onScreen);
					_beforeFirstStep = false;
				}
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
		// except before the first step.
		if (_beforeFirstStep)
		{
			_preMovementCost++;
		}

		_unit->turn();

		// calculateFOV is unreliable for setting the unitSpotted bool, as it can be called from various other places
		// in the code, ie: doors opening, and this messes up the result.
		_terrain->calculateFOV(_unit);
		unitSpotted = (!_falling && !_action.desperate && _parent->getPanicHandled() && _numUnitsSpotted != _unit->getUnitsSpottedThisTurn().size());

		// make sure the unit sprites are up to date
		_unit->setCache(0);
		_parent->getMap()->cacheUnit(_unit);

		if (unitSpotted && !_action.desperate && !_unit->getCharging() && !_falling)
		{
			if (_beforeFirstStep)
			{
				_unit->spendTimeUnits(_preMovementCost);
			}
			if (Options::traceAI) { Log(LOG_INFO) << "Egads! A turn reveals new units! I must pause!"; }
			_unit->_hidingForTurn = false; // not hidden, are we...
			_pf->abortPath();
			_unit->abortTurn(); //revert to a standing state.
			_unit->setCache(0);
			_parent->getMap()->cacheUnit(_unit);
			_parent->popState();
		}
	}
}

/**
 * Aborts unit walking.
 */
void UnitWalkBState::cancel()
{
	if (_parent->getSave()->getSide() == FACTION_PLAYER && _parent->getPanicHandled())
	_pf->abortPath();
}

/**
 * Handles some calculations when the path is finished.
 */
void UnitWalkBState::postPathProcedures()
{
	_action.TU = 0;
	if (_unit->getFaction() != FACTION_PLAYER)
	{
		int dir = _action.finalFacing;
		if (_action.finalAction)
		{
			_unit->dontReselect();
		}
		if (_unit->getCharging() != 0)
		{
			dir = _parent->getTileEngine()->getDirectionTo(_unit->getPosition(), _unit->getCharging()->getPosition());
			if (_parent->getTileEngine()->validMeleeRange(_unit, _action.actor->getCharging(), dir))
			{
				BattleAction action;
				action.actor = _unit;
				action.target = _unit->getCharging()->getPosition();
				action.weapon = _unit->getMainHandWeapon();
				action.type = BA_HIT;
				action.TU = _unit->getActionTUs(action.type, action.weapon);
				action.targeting = true;
				_unit->setCharging(0);
				_parent->statePushBack(new ProjectileFlyBState(_parent, action));
			}
		}
		else if (_unit->_hidingForTurn)
		{
			dir = _unit->getDirection() + 4;
			_unit->_hidingForTurn = false;
			_unit->dontReselect();
		}
		if (dir != -1)
		{
			if (dir >= 8)
			{
				dir -= 8;
			}
			_unit->lookAt(dir);
			while (_unit->getStatus() == STATUS_TURNING)
			{
				_unit->turn();
				_parent->getTileEngine()->calculateFOV(_unit);
			}
		}
	}
	else if (!_parent->getPanicHandled())
	{
		//todo: set the unit to aggrostate and try to find cover?
		_unit->setTimeUnits(0);
	}

	_unit->setCache(0);
	_terrain->calculateUnitLighting();
	_terrain->calculateFOV(_unit);
	_parent->getMap()->cacheUnit(_unit);
	if (!_falling)
		_parent->popState();
}

/**
 * Handles some calculations when the walking is finished.
 */
void UnitWalkBState::setNormalWalkSpeed()
{
	if (_unit->getFaction() == FACTION_PLAYER)
		_parent->setStateInterval(Options::battleXcomSpeed);
	else
		_parent->setStateInterval(Options::battleAlienSpeed);
}


/**
 * Handles the stepping sounds.
 */
void UnitWalkBState::playMovementSound()
{
	if ((!_unit->getVisible() && !_parent->getSave()->getDebugMode()) || !_parent->getMap()->getCamera()->isOnScreen(_unit->getPosition(), true)) return;

	if (_unit->getMoveSound() != -1)
	{
		// if a sound is configured in the ruleset, play that one
		if (_unit->getWalkingPhase() == 0)
		{
			_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), _unit->getMoveSound())->play();
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
					_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), ResourcePack::WALK_OFFSET + (tile->getFootstepSound(tileBelow)*2))->play();
				}
			}
			// play footstep sound 2
			if (_unit->getWalkingPhase() == 7)
			{
				if (tile->getFootstepSound(tileBelow))
				{
					_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), 1 + ResourcePack::WALK_OFFSET + (tile->getFootstepSound(tileBelow)*2))->play();
				}
			}
		}
		else
		{
			// play default flying sound
			if (_unit->getWalkingPhase() == 1 && !_falling)
			{
				_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), ResourcePack::FLYING_SOUND)->play();
			}
		}
	}
}

}
