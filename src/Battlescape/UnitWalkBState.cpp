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

#include "UnitWalkBState.h"
#include "ProjectileFlyBState.h"
#include "TileEngine.h"
#include "Pathfinding.h"
#include "BattlescapeState.h"
#include "Map.h"
#include "Camera.h"
#include "BattleAIState.h"
#include "AggroBAIState.h"
#include "ExplosionBState.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"
#include "../Ruleset/RuleArmor.h"

namespace OpenXcom
{

/**
 * Sets up an UnitWalkBState.
 */
UnitWalkBState::UnitWalkBState(BattlescapeState *parent, BattleAction action) : BattleState(parent), _unit(0), _pf(0), _terrain(0), _action(action)
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
	_pf = _parent->getGame()->getSavedGame()->getBattleGame()->getPathfinding();
	_terrain = _parent->getGame()->getSavedGame()->getBattleGame()->getTileEngine();
	_target = _action.target;
}

void UnitWalkBState::think()
{
	bool unitspotted = false;

	// during a walking cycle we make step sounds
	if (_unit->getStatus() == STATUS_WALKING || _unit->getStatus() == STATUS_FLYING)
	{

		if (_unit->getVisible() && _unit->getStatus() == STATUS_WALKING)
		{
			if (_unit->getUnit()->getArmor()->getSize() > 1)
			{
				// play hwp engine sound
				if (_unit->getWalkingPhase() == 0)
				{
					// tank with threads "walks"
					if (_unit->getUnit()->getArmor()->getMovementType() == MT_WALK)
						_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(14)->play();
					else // hovering tank hovers
						_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(40)->play();
				}
			}
			else
			{
				// play footstep sound 1
				if (_unit->getWalkingPhase() == 3)
				{
					Tile *tile = _unit->getTile();
					if (tile->getFootstepSound())
					{
						_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(22 + (tile->getFootstepSound()*2))->play();
					}
				}
				// play footstep sound 2
				if (_unit->getWalkingPhase() == 7)
				{
					Tile *tile = _unit->getTile();
					if (tile->getFootstepSound())
					{
						_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(23 + (tile->getFootstepSound()*2))->play();
					}
				}
			}
		}

		_unit->keepWalking(); // advances the phase

		// unit moved from one tile to the other, update the tiles
		if (_unit->getPosition() != _unit->getLastPosition())
		{
			int size = _unit->getUnit()->getArmor()->getSize() - 1;
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getGame()->getSavedGame()->getBattleGame()->getTile(_unit->getLastPosition() + Position(x,y,0))->setUnit(0);
				}
			}
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getGame()->getSavedGame()->getBattleGame()->getTile(_unit->getPosition() + Position(x,y,0))->setUnit(_unit);
				}
			}

			// if the unit changed level, camera changes level with
			_parent->getMap()->getCamera()->setViewHeight(_unit->getPosition().z);
		}

		// is the step finished?
		if (_unit->getStatus() == STATUS_STANDING)
		{
			// move our personal lighting with us
			_terrain->calculateUnitLighting();

			// check if we can spot new units
			unitspotted = _terrain->calculateFOV(_unit);
			if (unitspotted)
			{
				_pf->abortPath();

				// a hostile unit will aggro on the new unit if it sees one
				if (_unit->getFaction() == FACTION_HOSTILE)
				{
					AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(_unit->getCurrentAIState());
					if (aggro == 0)
					{
						_unit->setAIState(new AggroBAIState(_parent->getGame()->getSavedGame()->getBattleGame(), _unit));
					}
					_parent->handleAI(_unit);
				}
				return;
			}

			BattleAction action;
			
			// check for proximity grenades (for large units, we need to check every tile it occupies)
			// TODO : actually it needs to test within a range (of 1 tile) around the grenade instead on the tile itself...
			int size = _unit->getUnit()->getArmor()->getSize() - 1;
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					Tile *t = _parent->getGame()->getSavedGame()->getBattleGame()->getTile(_unit->getPosition() + Position(x,y,0));
					for (std::vector<BattleItem*>::iterator i = t->getInventory()->begin(); i != t->getInventory()->end(); ++i)
					{
						if ((*i)->getRules()->getBattleType() == BT_PROXIMITYGRENADE && (*i)->getExplodeTurn() > 0)
						{
							Position p;
							p.x = t->getPosition().x*16 + 8;
							p.y = t->getPosition().y*16 + 8;
							p.z = t->getPosition().z*24 + t->getTerrainLevel();
							_parent->statePushBack(new ExplosionBState(_parent, p, (*i), (*i)->getPreviousOwner()));
							t->getInventory()->erase(i);
							return;
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
		if (_unit->getVisible())
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
				_result = "STR_NOT_ENOUGH_TIME_UNITS";
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
				_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(3)->play(); // normal door
			}
			if (door == 1)
			{
				_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(20)->play(); // ufo door
				return; // don't start walking yet, wait for the ufo door to open
			}

			// now start moving
			dir = _pf->dequeuePath();
			if (_unit->spendTimeUnits(tu, _parent->dontSpendTUs()))
			{
				if (_unit->spendEnergy(tu, _parent->dontSpendTUs()))
				{
					_unit->startWalking(dir, destination);
				}
				else
				{
					_result = "STR_NOT_ENOUGH_ENERGY";
					_parent->popState();
				}
			}
			else
			{
				_result = "STR_NOT_ENOUGH_TIME_UNITS";
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
 * Get the action result. Returns error messages or an empty string when everything went fine.
 * @return returnmessage Empty when everything is fine.
 */
std::string UnitWalkBState::getResult() const
{
	return _result;
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
	if (_parent->getGame()->getSavedGame()->getBattleGame()->getDebugMode())
	{
		_parent->setStateInterval(1);
	}
	else
	{
		if (_unit->getFaction() == FACTION_PLAYER)
			_parent->setStateInterval(Options::getInt("battleXcomSpeed"));
		else
			_parent->setStateInterval(Options::getInt("battleAlienSpeed"));
	}
}

}
