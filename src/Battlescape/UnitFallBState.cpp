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
#include "../Engine/Options.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/Ruleset.h"

namespace OpenXcom
{

/**
 * Sets up an UnitFallBState.
 * @param parent Pointer to the Battlescape.
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

/**
 * Initializes the state.
 */
void UnitFallBState::init()
{
	_terrain = _parent->getTileEngine();
	if (_parent->getSave()->getSide() == FACTION_PLAYER)
		_parent->setStateInterval(Options::battleXcomSpeed);
	else
		_parent->setStateInterval(Options::battleAlienSpeed);

}

/**
 * Runs state functionality every cycle.
 * Progresses the fall, updates the battlescape, ...
 */
void UnitFallBState::think()
{
	for (std::list<BattleUnit*>::iterator unit = _parent->getSave()->getFallingUnits()->begin(); unit != _parent->getSave()->getFallingUnits()->end();)
	{
		if ((*unit)->getStatus() == STATUS_TURNING)
		{
			(*unit)->abortTurn();
		}
		bool largeCheck = true;
		bool falling = true;
		int size = (*unit)->getArmor()->getSize() - 1;
		if ((*unit)->getHealth() == 0 || (*unit)->getStunlevel() >= (*unit)->getHealth())
		{
			unit = _parent->getSave()->getFallingUnits()->erase(unit);
			continue;
		}
		bool onScreen = ((*unit)->getVisible() && _parent->getMap()->getCamera()->isOnScreen((*unit)->getPosition(), true));
		Tile *tileBelow = _parent->getSave()->getTile((*unit)->getPosition() + Position(0,0,-1));
		for (int x = size; x >= 0; x--)
		{
			for (int y = size; y >= 0; y--)
			{
				Tile *otherTileBelow = _parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,-1));
				if (!_parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,0))->hasNoFloor(otherTileBelow) || (*unit)->getArmor()->getMovementType() == MT_FLY)
				{
					largeCheck = false;
				}
			}
		}

		falling = largeCheck
			&& (*unit)->getPosition().z != 0
			&& (*unit)->getTile()->hasNoFloor(tileBelow)
			&& (*unit)->getArmor()->getMovementType() != MT_FLY
			&& (*unit)->getWalkingPhase() == 0;

		if (falling)
		{
			// Tile(s) unit is falling into.
			for (int x = (*unit)->getArmor()->getSize() - 1; x >= 0; --x)
			{
				for (int y = (*unit)->getArmor()->getSize() - 1; y >= 0; --y)
				{
					Tile *tileBelow = _parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,-1));
					tilesToFallInto.push_back(tileBelow);
				}
			}

			// Check each tile for units that need moving out of the way.
			for (std::vector<Tile*>::iterator i = tilesToFallInto.begin(); i < tilesToFallInto.end(); ++i)
			{
				BattleUnit *unitBelow = (*i)->getUnit();
				if (unitBelow
					&& (*unit) != unitBelow                                                                     // falling units do not fall on themselves
					&& !(std::find(unitsToMove.begin(), unitsToMove.end(), unitBelow) != unitsToMove.end()))    // already added
				{
					unitsToMove.push_back(unitBelow);
				}
			}
		}

		if ((*unit)->getStatus() == STATUS_WALKING || (*unit)->getStatus() == STATUS_FLYING)
		{
			(*unit)->keepWalking(tileBelow, true); 	// advances the phase
			_parent->getMap()->cacheUnit(*unit);	// make sure the unit sprites are up to date
		}

		falling = largeCheck
			&& (*unit)->getPosition().z != 0
			&& (*unit)->getTile()->hasNoFloor(tileBelow)
			&& (*unit)->getArmor()->getMovementType() != MT_FLY
			&& (*unit)->getWalkingPhase() == 0;

		// The unit has moved from one tile to the other.
		if ((*unit)->getPosition() != (*unit)->getLastPosition()  )
		{
			// Reset tiles moved from.
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					// A falling unit might have already taken up this position so check that this unit is still here.
					if (_parent->getSave()->getTile((*unit)->getLastPosition() + Position(x,y,0))->getUnit() == (*unit))
					{
						_parent->getSave()->getTile((*unit)->getLastPosition() + Position(x,y,0))->setUnit(0);
					}
				}
			}
			// Update tiles moved to.
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					_parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,0))->setUnit((*unit), _parent->getSave()->getTile((*unit)->getPosition() + Position(x,y,-1)));
				}
			}

			// Find somewhere to move the unit(s) endanger of being squashed.
			if (!unitsToMove.empty())
			{
				std::vector<Tile*> escapeTiles;
				for (std::vector<BattleUnit*>::iterator ub = unitsToMove.begin(); ub < unitsToMove.end(); )
				{
					BattleUnit *unitBelow = (*ub);
					bool escapeFound = false;

					// We need to move all sections of the unit out of the way.
					std::vector<Position> bodySections;
					for (int x = unitBelow->getArmor()->getSize() - 1; x >= 0; --x)
					{
						for (int y = unitBelow->getArmor()->getSize() - 1; y >= 0; --y)
						{
							Position bs = unitBelow->getPosition() + Position(x, y, 0);
							bodySections.push_back(bs);
						}
					}

					// Check in each compass direction.
					for (int dir = 0; dir < Pathfinding::DIR_UP && !escapeFound; dir++)
					{
						Position offset;
						Pathfinding::directionToVector(dir, &offset);

						for (std::vector<Position>::iterator bs = bodySections.begin(); bs < bodySections.end(); )
						{
							Position originalPosition = (*bs);
							Tile *currentTile = _parent->getSave()->getTile(originalPosition);
							Tile *t = _parent->getSave()->getTile(originalPosition + offset);
							Tile *bt = _parent->getSave()->getTile(originalPosition + offset + Position(0,0,-1));

							bool aboutToBeOccupiedFromAbove = t && std::find(tilesToFallInto.begin(), tilesToFallInto.end(), t) != tilesToFallInto.end();
							bool alreadyTaken = t && std::find(escapeTiles.begin(), escapeTiles.end(), t) != escapeTiles.end();
							bool alreadyOccupied = t && t->getUnit() && (t->getUnit() != unitBelow);
							bool movementBlocked = _parent->getSave()->getPathfinding()->isBlocked(currentTile, t, dir, unitBelow);
							bool hasFloor = t && !t->hasNoFloor(bt);
							bool unitCanFly = unitBelow->getArmor()->getMovementType() == MT_FLY;

							bool canMoveToTile = t && !alreadyOccupied && !alreadyTaken && !aboutToBeOccupiedFromAbove && !movementBlocked && (hasFloor || unitCanFly);
							if (canMoveToTile)
							{
								// Check next section of the unit.
								++bs;
							}
							else
							{
								// Try next direction.
								break;
							}

							// If all sections of the fallen onto unit can be moved, then we move it.
							if (bs == bodySections.end())
							{
								if (_parent->getSave()->addFallingUnit(unitBelow))
								{
									escapeFound = true;
									// Now ensure no other unit escapes to here too.
									for (int x = unitBelow->getArmor()->getSize() - 1; x >= 0; --x)
									{
										for (int y = unitBelow->getArmor()->getSize() - 1; y >= 0; --y)
										{
											Tile *et = _parent->getSave()->getTile(t->getPosition() + Position(x,y,0));
											escapeTiles.push_back(et);
										}
									}

									Tile *bu = _parent->getSave()->getTile(originalPosition + Position(0,0,-1));
									unitBelow->startWalking(dir, unitBelow->getPosition() + offset, bu, onScreen);
									ub = unitsToMove.erase(ub);
								}
							}
						}
					}
					if (!escapeFound)
					{
						unitBelow->knockOut(_parent);
						ub = unitsToMove.erase(ub);
					}
				}
				_parent->checkForCasualties(0,*unit);
			}
		}
		// we are just standing around, we are done falling.
		if ((*unit)->getStatus() == STATUS_STANDING)
		{
			if (falling)
			{
				Position destination = (*unit)->getPosition() + Position(0,0,-1);
				Tile *tileBelow = _parent->getSave()->getTile(destination);
				(*unit)->startWalking(Pathfinding::DIR_DOWN, destination, tileBelow, onScreen);
				(*unit)->setCache(0);
				_parent->getMap()->cacheUnit(*unit);
				++unit;
			}
			else
			{
				// if the unit burns floortiles, burn floortiles
				if ((*unit)->getSpecialAbility() == SPECAB_BURNFLOOR)
				{
					(*unit)->getTile()->ignite(1);
					Position here = ((*unit)->getPosition() * Position(16,16,24)) + Position(8,8,-((*unit)->getTile()->getTerrainLevel()));
					_parent->getTileEngine()->hit(here, (*unit)->getStats()->strength, _parent->getRuleset()->getDamageType(DT_IN), (*unit));
				}
				// move our personal lighting with us
				_terrain->calculateUnitLighting();
				_parent->getMap()->cacheUnit(*unit);
				(*unit)->setCache(0);
				_terrain->calculateFOV(*unit);
				_parent->checkForProximityGrenades(*unit);
				if (_parent->getTileEngine()->checkReactionFire(*unit))
					_parent->getPathfinding()->abortPath();
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
		tilesToFallInto.clear();
		unitsToMove.clear();
		_parent->popState();
		return;
	}
}

}
