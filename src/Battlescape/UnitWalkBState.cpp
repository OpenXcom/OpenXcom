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

	if (_unit->isOut())
	{
		_pf->abortPath();
		_parent->popState();
		return;
	}

	if (_unit->getStatus() == STATUS_WALKING || _unit->getStatus() == STATUS_FLYING)
	{
		playMovementSound();

		_unit->keepWalking(); // advances the phase

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
				_pf->abortPath();
				return;
			}
		}
		else
		{
			// make sure the unit sprites are up to date
			_parent->getMap()->cacheUnit(_unit);
		}
	}

	// we are just standing around, shouldn't we be walking?
	if (_unit->getStatus() == STATUS_STANDING)
	{
		// check if we did spot new units
		if (unitspotted)
		{
			_pf->abortPath();
			return;
		}

		if (_unit->getVisible() && _parent->getMap()->getCamera()->isOnScreen(_unit->getPosition()))
		{
			setNormalWalkSpeed();
		}
		else
		{
			_parent->setStateInterval(1);
		}
		int dir = _pf->getStartDirection();
		if (dir != -1)
		{
			Position destination;
			int tu = _pf->getTUCost(_unit->getPosition(), dir, &destination, _unit); // gets tu cost, but also gets the destination position.

			if (tu > _unit->getTimeUnits() && !_parent->dontSpendTUs())
			{
				_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
				_pf->abortPath();
				return;
			}

			if (_parent->checkReservedTU(_unit, tu) == false)
			{
				_pf->abortPath();
				return;
			}

			// we are looking in the wrong way, turn first
			// we are not using the turn state, because turning during walking costs no tu
			if (dir != _unit->getDirection() && dir < Pathfinding::DIR_UP)
			{
				_unit->lookAt(dir);
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
			if (_unit->spendTimeUnits(tu, _parent->dontSpendTUs()))
			{
				if (_unit->spendEnergy(tu, _parent->dontSpendTUs()))
				{
					_unit->startWalking(dir, destination, _parent->getSave()->getTile(destination));
				}
				else
				{
					_action.result = "STR_NOT_ENOUGH_ENERGY";
					_parent->popState();
				}
			}
			else
			{
				_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
				_parent->popState();
			}
			// make sure the unit sprites are up to date
			_parent->getMap()->cacheUnit(_unit);
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
		_unit->turn();
		unitspotted = _terrain->calculateFOV(_unit);
		// make sure the unit sprites are up to date
		_parent->getMap()->cacheUnit(_unit);
		if (unitspotted)
		{
			_pf->abortPath();
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
	_terrain->calculateUnitLighting();
	_terrain->calculateFOV(_unit);
	_parent->getMap()->cacheUnit(_unit);
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
	if (!_unit->getVisible() && !_parent->getSave()->getDebugMode()) return;

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
