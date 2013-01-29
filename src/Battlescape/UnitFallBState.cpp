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

#include "UnitFallBState.h"
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
 * Sets up an UnitFallBState.
 */
UnitFallBState::UnitFallBState(BattlescapeGame *parent, BattleAction action) : BattleState(parent, action), _terrain(0)
{

}

/**
 * Deletes the UnitWalkBState.
 */
UnitFallBState::~UnitFallBState()
{

}

void UnitFallBState::init()
{
	_terrain = _parent->getTileEngine();
}

void UnitFallBState::think()
{
	for (std::vector<BattleUnit*>::iterator _unit = _parent->getSave()->getFallingUnits()->begin(); _unit != _parent->getSave()->getFallingUnits()->end();)
	{
		bool onScreen = ((*_unit)->getVisible() && _parent->getMap()->getCamera()->isOnScreen((*_unit)->getPosition()));
	
		if (onScreen)
		{
			if ((*_unit)->getFaction() == FACTION_PLAYER)
				_parent->setStateInterval(Options::getInt("battleXcomSpeed"));
			else
				_parent->setStateInterval(Options::getInt("battleAlienSpeed"));
		}
		else
		{
			_parent->setStateInterval(0);
		}
		
		if ((*_unit)->getStatus() != STATUS_STANDING)
		{
			(*_unit)->keepWalking(onScreen); // advances the phase

			if (onScreen)
			{
				// make sure the unit sprites are up to date
				_parent->getMap()->cacheUnit(*_unit);
			}
		}

		// unit moved from one tile to the other, update the tiles
		if ((*_unit)->getPosition() != (*_unit)->getLastPosition())
		{
			int size = (*_unit)->getArmor()->getSize() - 1;
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getSave()->getTile((*_unit)->getLastPosition() + Position(x,y,0))->setUnit(0);
				}
			}
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getSave()->getTile((*_unit)->getPosition() + Position(x,y,0))->setUnit((*_unit));
				}
			}
		}

		// we are just standing around, we are done falling.
		if ((*_unit)->getStatus() == STATUS_STANDING)
		{
			// if the unit burns floortiles, burn floortiles
			if ((*_unit)->getSpecialAbility() == SPECAB_BURNFLOOR)
			{
				(*_unit)->getTile()->destroy(MapData::O_FLOOR);
			}

			// move our personal lighting with us
			_terrain->calculateUnitLighting();
			BattleAction action;
			
			// check for proximity grenades (1 tile around the unit in every direction) (for large units, we need to check every tile it occupies)
			int size = (*_unit)->getArmor()->getSize() - 1;
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					for (int tx = -1; tx < 2; tx++)
					{
						for (int ty = -1; ty < 2; ty++)
						{
							Tile *t = _parent->getSave()->getTile((*_unit)->getPosition() + Position(x,y,0) + Position(tx,ty,0));
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
			if (onScreen || _parent->getSave()->getDebugMode())
			{
				_parent->getMap()->cacheUnit(*_unit);
			}
			(*_unit)->setCache(0);
			_terrain->calculateFOV(*_unit);
			_unit = _parent->getSave()->getFallingUnits()->erase(_unit);
		}
		else
		{
			++_unit;
		}
	}

	if (_parent->getSave()->getFallingUnits()->empty())
	{
		_parent->popState();
		return;
	}
}

}
