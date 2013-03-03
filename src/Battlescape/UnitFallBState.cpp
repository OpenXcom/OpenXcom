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
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"
#include "../Ruleset/Armor.h"

namespace OpenXcom
{

/**
 * Sets up an UnitFallBState.
 */
UnitFallBState::UnitFallBState(BattlescapeGame *parent) : BattleState(parent), _terrain(0)
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
	if (_parent->getSave()->getSide() == FACTION_PLAYER)
		_parent->setStateInterval(Options::getInt("battleXcomSpeed"));
	else
		_parent->setStateInterval(Options::getInt("battleAlienSpeed"));
		
}

void UnitFallBState::think()
{
	
	for (std::vector<BattleUnit*>::iterator unit = _parent->getSave()->getFallingUnits()->begin(); unit != _parent->getSave()->getFallingUnits()->end();)
	{
		bool largeCheck = true;
		bool falling = true;
		int size = (*unit)->getArmor()->getSize() - 1;
		if ((*unit)->getHealth() == 0 || (*unit)->getStunlevel() > (*unit)->getHealth())
		{
			unit = _parent->getSave()->getFallingUnits()->erase(unit);
			continue;
		}
		bool onScreen = ((*unit)->getVisible() && _parent->getMap()->getCamera()->isOnScreen((*unit)->getPosition()));
		Tile *tileBelow = _parent->getSave()->getTile((*unit)->getPosition() + Position(0,0,-1));
		for (int x = size; x >= 0; x--)
		{
			for (int y = size; y >= 0; y--)
			{
				Tile *otherTileBelow = _parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,-1));
				if (!_parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,0))->hasNoFloor(otherTileBelow) || (*unit)->getArmor()->getMovementType() == MT_FLY)
					largeCheck = false;
			}
		}
		falling = largeCheck && (*unit)->getPosition().z != 0 && (*unit)->getTile()->hasNoFloor(tileBelow) && (*unit)->getArmor()->getMovementType() != MT_FLY && (*unit)->getWalkingPhase() == 0;
		if ((*unit)->getStatus() == STATUS_WALKING || (*unit)->getStatus() == STATUS_FLYING)
		{
			(*unit)->keepWalking(tileBelow, true); // advances the phase

			// make sure the unit sprites are up to date
			_parent->getMap()->cacheUnit(*unit);
		}
		
		// unit moved from one tile to the other, update the tiles
		if ((*unit)->getPosition() != (*unit)->getLastPosition())
		{
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getSave()->getTile((*unit)->getLastPosition() + Position(x,y,0))->setUnit(0);
				}
			}
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,0))->setUnit((*unit), _parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,-1)));
				}
			}
			
			if (falling)
			{
				for (int x = (*unit)->getArmor()->getSize() - 1; x >= 0; --x)
				{
					for (int y = (*unit)->getArmor()->getSize() - 1; y >= 0; --y)
					{
						Tile *otherTileBelow = _parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,-1));
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
								if (t && !_parent->getSave()->getPathfinding()->isBlocked(otherTileBelow, t, dir, 0) && t->getUnit() == 0 && (!t->hasNoFloor(bt) || otherTileBelow->getUnit()->getArmor()->getMovementType() == MT_FLY))
								{
									otherTileBelow->getUnit()->startWalking(dir, t->getPosition(), t, bu, bt, onScreen);
									_parent->getSave()->addFallingUnit(otherTileBelow->getUnit());
									break;
								}
							}
						}
					}
				}
			}
		}
		// we are just standing around, we are done falling.
		if ((*unit)->getStatus() == STATUS_STANDING)
		{
			if (falling)
			{
				Position destination = (*unit)->getPosition() + Position(0,0,-1);
				Tile *tileBelow = _parent->getSave()->getTile(destination);
				Tile *tileBelowDestination = _parent->getSave()->getTile(destination + Position(0,0,-1));
				(*unit)->startWalking(Pathfinding::DIR_DOWN, destination, tileBelow, tileBelow, tileBelowDestination, onScreen);
				(*unit)->setCache(0);
				_parent->getMap()->cacheUnit(*unit);
				++unit;
			}
			else
			{
				// if the unit burns floortiles, burn floortiles
				if ((*unit)->getSpecialAbility() == SPECAB_BURNFLOOR)
				{
					(*unit)->getTile()->destroy(MapData::O_FLOOR);
				}
				// move our personal lighting with us
				_terrain->calculateUnitLighting();
				_parent->getMap()->cacheUnit(*unit);
				(*unit)->setCache(0);
				_terrain->calculateFOV(*unit);

				unit = _parent->getSave()->getFallingUnits()->erase(unit);
			}
		}
		else
		{
			++unit;
		}
	}

	if (_parent->getSave()->getFallingUnits()->empty())
	{
		_parent->popState();
		return;
	}
}

}
