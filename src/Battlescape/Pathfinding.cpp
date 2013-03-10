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
#include <list>
#include "Pathfinding.h"
#include "PathfindingNode.h"
#include "PathfindingOpenSet.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Ruleset/MapData.h"
#include "../Ruleset/Armor.h"
#include "../Savegame/BattleUnit.h"
#include "../Engine/Game.h"
#include "../Battlescape/TileEngine.h"

namespace OpenXcom
{

/**
 * Sets up a Pathfinding.
 * @param save pointer to SavedBattleGame object.
 */
Pathfinding::Pathfinding(SavedBattleGame *save) : _save(save), _nodes(), _unit(0), _pathPreviewed(false)
{
	_size = _save->getMapSizeXYZ();
	// Initialize one node per tile
	_nodes.reserve(_size);
	Position p;
	for (int i = 0; i < _size; ++i)
	{
		_save->getTileCoords(i, &p.x, &p.y, &p.z);
		_nodes.push_back(PathfindingNode(p));
	}
}

/**
 * Deletes the Pathfinding.
 * @internal This is required to be here because it requires the PathfindingNode class definition.
 */
Pathfinding::~Pathfinding()
{
	// Nothing more to do here.
}

/**
 * Gets the Node on a given position on the map.
 * @param pos position
 * @return Pointer to node.
 */
PathfindingNode *Pathfinding::getNode(const Position& pos)
{
	return &_nodes[_save->getTileIndex(pos)];
}

/**
 * Calculate the shortest path.
 * @param unit
 * @param endPosition
 */

void Pathfinding::calculate(BattleUnit *unit, Position endPosition, BattleUnit *target, int maxTUCost)
{
	_totalTUCost = 0;
	// i'm DONE with these out of bounds errors.
	if (endPosition.x > _save->getMapSizeX() - unit->getArmor()->getSize() || endPosition.y > _save->getMapSizeY() - unit->getArmor()->getSize() || endPosition.x < 0 || endPosition.y < 0) return;

	bool sneak = _save->getSneakySetting() && unit->getFaction() == FACTION_HOSTILE;
	
	Position startPosition = unit->getPosition();
	_movementType = unit->getArmor()->getMovementType();
	if (target != 0 && maxTUCost == -1)  // pathfinding for missile
	{
		_movementType = MT_FLY;
		maxTUCost = 10000;
	}
	_unit = unit;

	Tile *destinationTile = _save->getTile(endPosition);

	// check if destination is not blocked
	if (isBlocked(destinationTile, MapData::O_FLOOR, target) || isBlocked(destinationTile, MapData::O_OBJECT, target)) return;

	// the following check avoids that the unit walks behind the stairs if we click behind the stairs to make it go up the stairs.
	// it only works if the unit is on one of the 2 tiles on the stairs, or on the tile right in front of the stairs.
	if (isOnStairs(startPosition, endPosition))
	{
		endPosition.z++;
		destinationTile = _save->getTile(endPosition);
	}	
	while (endPosition.z != _save->getMapSizeZ() && destinationTile->getTerrainLevel() == -24)
	{
		endPosition.z++;
		destinationTile = _save->getTile(endPosition);
	}
	Tile *tileBelow = _save->getTile(endPosition + Position(0,0,-1));
	// check if we have floor, else lower destination (for non flying units only, because otherwise they never reached this place)
	while (canFallDown(destinationTile, _unit->getArmor()->getSize()) && _movementType != MT_FLY)
	{
		endPosition.z--;
		destinationTile = _save->getTile(endPosition);
	}
	int size = unit->getArmor()->getSize()-1;
	if (size >= 1)
	{
		int its = 0;
		for (int x = 0; x <= size; x += size)
		{
			for (int y = 0; y <= size; y += size)
			{
				if (x || y)
				{
					int dir[3] = {4,2,3};
					Tile *checkTile = _save->getTile(endPosition + Position(x, y, 0));
					if (isBlocked(unit->getTile(), checkTile, dir[its], target) || 
						(checkTile->getUnit() &&
						checkTile->getUnit() != unit &&
						checkTile->getUnit()->getVisible() &&
						checkTile->getUnit() != target))
						return;
					++its;
				}
			}
		}
	}
	// Strafing move allowed only to adjacent squares on same z. "Same z" rule mainly to simplify walking render.
	_strafeMove = _save->getStrafeSetting() && Game::getCtrlKeyDown() && !Game::getShiftKeyDown() && (startPosition.z == endPosition.z) && 
							(abs(startPosition.x - endPosition.x) <= 1) && (abs(startPosition.y - endPosition.y) <= 1);

	_path.clear();

	// look for a possible fast and accurate bresenham path and skip A*
	if (startPosition.z == endPosition.z && bresenhamPath(startPosition,endPosition, target, sneak))
	{
		std::reverse(_path.begin(), _path.end()); //paths are stored in reverse order
		return;
	}
	else
	{
		_path.clear(); // if bresenham failed, we shouldn't keep the path it was attempting, in case A* fails too.
		_totalTUCost = 0;
	}
	// Now try through A*.
	if (!aStarPath(startPosition, endPosition, target, sneak, maxTUCost))
	{
		_path.clear();
	}
}

/**
 * Calculate the shortest path using a simple A-Star algorithm.
 * The unit information and movement type must have already been set.
 * The path information is set only if a valid path is found.
 * @param startPosition The position to start from.
 * @param endPosition The position we want to reach.
 * @return True if a path exists, false otherwise.
 */
bool Pathfinding::aStarPath(const Position &startPosition, const Position &endPosition, BattleUnit *target, bool sneak, int maxTUCost)
{
	// reset every node, so we have to check them all
	for (std::vector<PathfindingNode>::iterator it = _nodes.begin(); it != _nodes.end(); ++it)
		it->reset();

	// start position is the first one in our "open" list
	PathfindingNode *start = getNode(startPosition);
	start->connect(0, 0, 0, endPosition);
	PathfindingOpenSet openList;
	openList.push(start);
	bool missile = (target && maxTUCost == 10000);
	// if the open list is empty, we've reached the end
	while(!openList.empty())
	{
		PathfindingNode *currentNode = openList.pop();
		Position const &currentPos = currentNode->getPosition();
		currentNode->setChecked();
		if (currentPos == endPosition) // We found our target.
		{
			_path.clear();
			PathfindingNode *pf = currentNode;
			while (pf->getPrevNode())
			{
				_path.push_back(pf->getPrevDir());
				pf = pf->getPrevNode();
			}
			return true;
		}

		// Try all reachable neighbours.
		for (int direction = 0; direction < 10; direction++)
		{
			Position nextPos;
			int tuCost = getTUCost(currentPos, direction, &nextPos, _unit, target, missile);
			if (tuCost >= 255) // Skip unreachable / blocked
				continue;
			if (sneak && _save->getTile(nextPos)->getVisible()) tuCost *= 5; // avoid being seen
			PathfindingNode *nextNode = getNode(nextPos);
			if (nextNode->isChecked()) // Our algorithm means this node is already at minimum cost.
				continue;
			_totalTUCost = currentNode->getTUCost(missile) + tuCost;
			// If this node is unvisited or has only been visited from inferior paths...
			if ((!nextNode->inOpenSet() || nextNode->getTUCost(missile) > _totalTUCost) && _totalTUCost <= maxTUCost)
			{
				nextNode->connect(_totalTUCost, currentNode, direction, endPosition);
				openList.push(nextNode);
			}
		}
	}
	// Unble to reach the target
	return false;
}

/**
 * Get's the TU cost to move from 1 tile to the other(ONE STEP ONLY). But also updates the endPosition, because it is possible
 * the unit goes upstairs or falls down while walking.
 * @param startPosition
 * @param direction
 * @param endPosition pointer
 * @param unit
 * @return TU cost - 255 if movement impossible
 */
int Pathfinding::getTUCost(const Position &startPosition, int direction, Position *endPosition, BattleUnit *unit, BattleUnit *target, bool missile)
{
	_unit = unit;
	directionToVector(direction, endPosition);
	*endPosition += startPosition;
	bool fellDown = false;
	bool triedStairs = false;
	int size = _unit->getArmor()->getSize() - 1;
	int cost = 0;
	int numberOfPartsGoingUp = 0;
	int numberOfPartsGoingDown = 0;
	int numberOfPartsFalling = 0;
	int numberOfPartsChangingHeight = 0;
	int totalCost = 0;

	for (int x = 0; x <= size; ++x)
		for (int y = 0; y <= size; ++y)
		{
			Position offset = Position (x, y, 0);
			Tile *startTile = _save->getTile(startPosition + offset);
			Tile *destinationTile = _save->getTile(*endPosition + offset);
			Tile *belowDestination = _save->getTile(*endPosition + offset + Position(0,0,-1));
			Tile *aboveDestination = _save->getTile(*endPosition + offset + Position(0,0,1));

			// this means the destination is probably outside the map
			if (startTile == 0 || destinationTile == 0)
				return 255;
			/*
			// check if the destination tile can be walked over
			if (isBlocked(destinationTile, MapData::O_FLOOR, missileTarget) || isBlocked(destinationTile, MapData::O_OBJECT, missileTarget))
				return 255;
			*/
			// can't walk on top of other units
			if (_save->getTile(*endPosition + Position(x,y,-1))
				&& _save->getTile(*endPosition + Position(x,y,-1))->getUnit()
				&& _save->getTile(*endPosition + Position(x,y,-1))->getUnit() != _unit
				&& _save->getTile(*endPosition + Position(x,y,-1))->getUnit() != target
				&& !_save->getTile(*endPosition + Position(x,y,-1))->getUnit()->isOut()
				&& _movementType != MT_FLY && _save->getTile(*endPosition + offset)->hasNoFloor(belowDestination))
				return 255;

			// if we are on a stairs try to go up a level
			if (direction < DIR_UP && startTile->getTerrainLevel() <= -16 && !aboveDestination->hasNoFloor(destinationTile) && !triedStairs)
			{
					numberOfPartsGoingUp++;

					if (numberOfPartsGoingUp > size)
					{
						endPosition->z++;
						destinationTile = _save->getTile(*endPosition + offset);
						belowDestination = _save->getTile(*endPosition + Position(x,y,-1));
						triedStairs = true;
					}
			}
			else if (!fellDown && _movementType != MT_FLY && belowDestination && canFallDown(destinationTile) && belowDestination->getTerrainLevel() <= -12)
			{
					numberOfPartsGoingDown++;

					if (numberOfPartsGoingDown == (size + 1)*(size + 1))
					{
						endPosition->z--;
						destinationTile = _save->getTile(*endPosition + offset);
						belowDestination = _save->getTile(*endPosition + Position(x,y,-1));
						fellDown = true;
					}
			}

			// this means the destination is probably outside the map
			if (!destinationTile)
				return 255;

			if (direction < DIR_UP && endPosition->z == startTile->getPosition().z)
			{
				// check if we can go this way
				if (isBlocked(startTile, destinationTile, direction, target))
					return 255;
				if (startTile->getTerrainLevel() - destinationTile->getTerrainLevel() > 8)
					return 255;

			}
			else if (direction >= DIR_UP)
			{
				// check if we can go up or down through gravlift or fly
				if (validateUpDown(unit, startPosition + offset, direction))
				{
					cost = 8; // vertical movement by flying suit or grav lift
				}
				else
				{
					return 255;
				}
			}

			int wallcost = 0; // walking through rubble walls
			if (!triedStairs && !fellDown)
			{
				if (direction == 7 || direction == 0 || direction == 1)
					wallcost += startTile->getTUCost(MapData::O_NORTHWALL, _movementType);
				if (direction == 1 || direction == 2 || direction == 3)
					wallcost += destinationTile->getTUCost(MapData::O_WESTWALL, _movementType);
				if (direction == 3 || direction == 4 || direction == 5)
					wallcost += destinationTile->getTUCost(MapData::O_NORTHWALL, _movementType);
				if (direction == 5 || direction == 6 || direction == 7)
					wallcost += startTile->getTUCost(MapData::O_WESTWALL, _movementType);
			}
			// check if we have floor, else fall down
			if (_movementType != MT_FLY && !fellDown && canFallDown(startTile))
			{
				numberOfPartsFalling++;

				if (numberOfPartsFalling == (size+1)*(size+1))
				{
					*endPosition = startPosition + Position(0,0,-1);
					destinationTile = _save->getTile(*endPosition + offset);
					belowDestination = _save->getTile(*endPosition + Position(x,y,-1));
					fellDown = true;
				}
			}

			// check if the destination tile can be walked over
			if ((isBlocked(destinationTile, MapData::O_FLOOR, target) || isBlocked(destinationTile, MapData::O_OBJECT, target)) && !fellDown)
			{
				return 255;
			}

			// can't walk through other units
			if (_save->getTile(*endPosition + Position(x,y,0))
				&& _save->getTile(*endPosition + Position(x,y,0))->getUnit()
				&& _save->getTile(*endPosition + Position(x,y,0))->getUnit() != _unit
				&& _save->getTile(*endPosition + Position(x,y,0))->getUnit() != target
				&& !_save->getTile(*endPosition + Position(x,y,0))->getUnit()->isOut())
				return 255;

			// if we don't want to fall down and there is no floor, we can't know the TUs so it's default to 4
			if (direction < DIR_UP && !fellDown && destinationTile->hasNoFloor(0))
			{
				cost = 4;
			}
			// calculate the cost by adding floor walk cost and object walk cost
			if (direction < DIR_UP)
			{
				cost += destinationTile->getTUCost(MapData::O_FLOOR, _movementType);
				if (!fellDown && !triedStairs && destinationTile->getMapData(MapData::O_OBJECT))
				{
					cost += destinationTile->getTUCost(MapData::O_OBJECT, _movementType);
				}
			}

			// diagonal walking (uneven directions) costs 50% more tu's
			if (direction < DIR_UP && direction & 1)
			{
				wallcost /= 2;
				cost = (int)((double)cost * 1.5);
			}
			cost += wallcost;

			// Strafing costs +1 for forwards-ish or sidewards, propose +2 for backwards-ish directions
			// Maybe if flying then it makes no difference?
			if (_save->getStrafeSetting() && _strafeMove) {
				if (size) {
					// 4-tile units not supported.
					// Turn off strafe move and continue
					_strafeMove = false;
				}
				else
				{
					if (std::min(abs(8 + direction - _unit->getDirection()), std::min( abs(_unit->getDirection() - direction), abs(8 + _unit->getDirection() - direction))) > 2) {
						// Strafing backwards-ish currently unsupported, turn it off and continue.
						_strafeMove = false;
					}
					else
					{
						if (_unit->getDirection() != direction) {
							cost += 1;
						}
					}
				}
			}
			totalCost += cost;
			cost = 0;
		}

	// for bigger sized units, check the path between part 1,1 and part 0,0 at end position
	if (size)
	{
		totalCost /= (size+1)*(size+1);
		Tile *startTile = _save->getTile(*endPosition + Position(1,1,0));
		Tile *destinationTile = _save->getTile(*endPosition);
		int tmpDirection = 7;
		if (isBlocked(startTile, destinationTile, tmpDirection, target))
			return 255;
		if (!fellDown && abs(startTile->getTerrainLevel() - destinationTile->getTerrainLevel()) > 10)
			return 255;
		// also check if we change level, that there are two parts changing level,
		// so a big sized unit can not go up a small sized stairs
		if (numberOfPartsChangingHeight == 1)
			return 255;
	}
	
	if (missile)
		return 0;
	else
		return totalCost;
}

/*
 * Converts direction to a vector. Direction starts north = 0 and goes clockwise.
 * @param direction
 * @param vector pointer to a position (which acts as a vector)
 */
void Pathfinding::directionToVector(const int direction, Position *vector)
{
	int x[10] = {0, 1, 1, 1, 0, -1, -1, -1,0,0};
	int y[10] = {-1, -1, 0, 1, 1, 1, 0, -1,0,0};
	int z[10] = {0, 0, 0, 0, 0, 0, 0, 0, 1, -1};
	vector->x = x[direction];
	vector->y = y[direction];
	vector->z = z[direction];
}

/*
 * Check whether a path is ready and gives first direction.
 * @return direction where the unit needs to go next, -1 if it's the end of the path.
 */
int Pathfinding::getStartDirection()
{
	if (_path.empty()) return -1;
	return _path.back();
}

/*
 * Dequeue next path direction. Ie returns direction and removes from queue.
 * @return direction where the unit needs to go next, -1 if it's the end of the path.
 */
int Pathfinding::dequeuePath()
{
	if (_path.empty()) return -1;
	int last_element = _path.back();
	_path.pop_back();
	return last_element;
}

/*
 * Abort path clears the path vector.
 */
void Pathfinding::abortPath()
{
	_totalTUCost = 0;
	_path.clear();
}


/*
 * Whether a certain part of a tile blocks movement.
 * @param tile can be null pointer
 * @param movementType
 * @return true/false
 */
bool Pathfinding::isBlocked(Tile *tile, const int part, BattleUnit *missileTarget)
{
	if (tile == 0) return true; // probably outside the map here

	if (part == O_BIGWALL)
	{
		if (tile->getMapData(MapData::O_OBJECT) &&
			tile->getMapData(MapData::O_OBJECT)->isBigWall() &&
			tile->getTUCost(MapData::O_OBJECT, _movementType) == 255)
			return true; // blocking part
		else
			return false;
	}

	if (part == MapData::O_FLOOR)
	{
		BattleUnit *unit = tile->getUnit();
		if (unit == 0 || unit == _unit || unit == missileTarget || unit->isOut()) return false;
		if (unit->getFaction() == _unit->getFaction() ||		// unit know, where his faction members
			(_unit->getFaction() == FACTION_PLAYER && unit->getVisible())) return true;		// player know all visible units
		if (_unit->getFaction() != FACTION_PLAYER)
		{
			if (_save->eyesOnTarget(_unit->getFaction(), unit)) return true;
			// aliens know the location of all XCom agents sighted by all other aliens due to sharing locations over their space-walkie-talkies		
		
#if 0
			for (std::vector<BattleUnit*>::iterator i = _unit->getVisibleUnits()->begin(); i != _unit->getVisibleUnits()->end(); ++i)
			{
				if ((*i)->getTile() == tile) return true;	// unit know, were all visible for him units
			}
#endif
		}
	}

	if (tile->getTUCost(part, _movementType) == 255) return true; // blocking part

	return false;
}

/**
 * Whether going from one tile to another blocks movement.
 * @param startTile
 * @param endTile
 * @param direction
 * @return true/false
 */
bool Pathfinding::isBlocked(Tile *startTile, Tile *endTile, const int direction, BattleUnit *missileTarget)
{

	// check if the difference in height between start and destination is not too high
	// so we can not jump to the highest part of the stairs from the floor
	// stairs terrainlevel goes typically -8 -16 (2 steps) or -4 -12 -20 (3 steps)
	// this "maximum jump height" is therefore set to 8

	static const Position oneTileNorth = Position(0, -1, 0);
	static const Position oneTileEast = Position(1, 0, 0);
	static const Position oneTileSouth = Position(0, 1, 0);
	static const Position oneTileWest = Position(-1, 0, 0);
	Position pos1 (startTile->getPosition());

	switch(direction)
	{
	case 0:	// north
		if (isBlocked(startTile, MapData::O_NORTHWALL, missileTarget)) return true;
		break;
	case 1: // north east
		if (isBlocked(startTile,MapData::O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(endTile,MapData::O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileEast),MapData::O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileEast),MapData::O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileNorth),O_BIGWALL, missileTarget) && isBlocked(_save->getTile(pos1 + oneTileNorth),O_BIGWALL, missileTarget)) return true;
		break;
	case 2: // east
		if (isBlocked(endTile,MapData::O_WESTWALL, missileTarget)) return true;
		break;
	case 3: // south east
		if (isBlocked(endTile,MapData::O_WESTWALL, missileTarget)) return true;
		if (isBlocked(endTile,MapData::O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileEast),MapData::O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileSouth),MapData::O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileSouth),O_BIGWALL, missileTarget) && isBlocked(_save->getTile(pos1 + oneTileEast),O_BIGWALL, missileTarget)) return true;
		break;
	case 4: // south
		if (isBlocked(endTile,MapData::O_NORTHWALL, missileTarget)) return true;
		break;
	case 5: // south west
		if (isBlocked(endTile,MapData::O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(startTile,MapData::O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileSouth),MapData::O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileSouth),MapData::O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileSouth),O_BIGWALL, missileTarget) && isBlocked(_save->getTile(pos1 + oneTileWest),O_BIGWALL, missileTarget)) return true;
		break;
	case 6: // west
		if (isBlocked(startTile,MapData::O_WESTWALL, missileTarget)) return true;
		break;
	case 7: // north west
		if (isBlocked(startTile,MapData::O_WESTWALL, missileTarget)) return true;
		if (isBlocked(startTile,MapData::O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileNorth),MapData::O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileWest),MapData::O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(pos1 + oneTileNorth),O_BIGWALL, missileTarget) && isBlocked(_save->getTile(pos1 + oneTileWest),O_BIGWALL, missileTarget)) return true;
		break;
	}

	return false;
}

/**
 * We can fall down here, if the tile does not exist, the tile has no floor
 * the current position is higher than 0, if there is no unit standing below us
 * @param here
 * @return bool
 */
bool Pathfinding::canFallDown(Tile *here)
{
	if (here->getPosition().z == 0)
		return false;
	for (int z = 1; z <= here->getPosition().z; ++z)
	{
		if (_save->selectUnit(here->getPosition() - Position(0, 0, z)) &&
			_save->selectUnit(here->getPosition() - Position(0, 0, z)) != _unit &&
			!_save->selectUnit(here->getPosition() - Position(0, 0, z))->isOut())
			return false;
	}

	Tile* tileBelow = _save->getTile(here->getPosition() - Position (0,0,1));

	return here->hasNoFloor(tileBelow);
}

bool Pathfinding::canFallDown(Tile *here, int size)
{
	for (int x = 0; x != size; ++x)
	{
		for (int y = 0; y != size; ++y)
		{
			Position checkPos = here->getPosition() + Position(x,y,0);
			Tile *checkTile = _save->getTile(checkPos);
			if (!canFallDown(checkTile))
				return false;
		}
	}
	return true;
}
/**
 * We are going upstairs here?
 * @param startPosition
 * @param endPosition
 * @return bool
 */
bool Pathfinding::isOnStairs(const Position &startPosition, const Position &endPosition)
{
	//condition 1 : endposition has to the south a terrainlevel -16 object (upper part of the stairs)
	if (_save->getTile(endPosition + Position(0, 1, 0)) && _save->getTile(endPosition + Position(0, 1, 0))->getTerrainLevel() == -16)
	{
		// condition 2 : one position further to the south there has to be a terrainlevel -8 object (lower part of the stairs)
		if (_save->getTile(endPosition + Position(0, 2, 0)) && _save->getTile(endPosition + Position(0, 2, 0))->getTerrainLevel() != -8)
		{
			return false;
		}

		// condition 3 : the start position has to be on either of the 3 tiles to the south of the endposition
		if (startPosition == endPosition + Position(0, 1, 0) || startPosition == endPosition + Position(0, 2, 0) || startPosition == endPosition + Position(0, 3, 0))
		{
			return true;
		}
	}

	// same for the east-west oriented stairs.
	if (_save->getTile(endPosition + Position(1, 0, 0)) && _save->getTile(endPosition + Position(1, 0, 0))->getTerrainLevel() == -16)
	{
		if (_save->getTile(endPosition + Position(2, 0, 0)) && _save->getTile(endPosition + Position(2, 0, 0))->getTerrainLevel() != -8)
		{
			return false;
		}
		if (startPosition == endPosition + Position(1, 0, 0) || startPosition == endPosition + Position(2, 0, 0) || startPosition == endPosition + Position(3, 0, 0))
		{
			return true;
		}
	}
	return false;
}

/**
 * Check for the up/down button if the movement is valid. Either is a grav lift or the unit can fly and there are no obstructions.
 * @param bu Pointer to unit.
 * @param startPosition Unit starting position.
 * @param direction Up or Down
 * @return bool Whether it's valid.
 */
bool Pathfinding::validateUpDown(BattleUnit *bu, Position startPosition, const int direction)
{
	Position endPosition;
	directionToVector(direction, &endPosition);
	endPosition += startPosition;
	Tile *startTile = _save->getTile(startPosition);
	Tile *belowStart = _save->getTile(startPosition + Position(0,0,-1));
	Tile *destinationTile = _save->getTile(endPosition);
	if (startTile->getMapData(MapData::O_FLOOR) && destinationTile && destinationTile->getMapData(MapData::O_FLOOR) &&
		(startTile->getMapData(MapData::O_FLOOR)->isGravLift() && destinationTile->getMapData(MapData::O_FLOOR)->isGravLift()))
	{
		return true;
	}
	else
	{
		if (bu->getArmor()->getMovementType() == MT_FLY)
		{
			if ((direction == DIR_UP && destinationTile && !destinationTile->getMapData(MapData::O_FLOOR)) // flying up only possible when there is no roof
				|| (direction == DIR_DOWN && destinationTile && startTile->hasNoFloor(belowStart)) // falling down only possible when there is no floor
				)
			{
				return true;
			}
		}
	}

	return false;
}


/*
* Preview path, marks tiles.
*/
bool Pathfinding::previewPath(bool bRemove)
{
	if (_path.empty())
		return false;
 
	if (!bRemove && _pathPreviewed)
		return false;
 
	_pathPreviewed = !bRemove;
 
	Position pos = _unit->getPosition();
	Position destination;
	int tus = _unit->getTimeUnits();
	int size = _unit->getArmor()->getSize() - 1;
 
	for (std::vector<int>::reverse_iterator i = _path.rbegin(); i != _path.rend(); ++i)
	{
		int dir = *i;
		int tu = getTUCost(pos, dir, &destination, _unit, 0, false); // gets tu cost, but also gets the destination position.
		tus -= tu;
		pos = destination;
		for (int x = size; x >= 0; x--)
		{
			for (int y = size; y >= 0; y--)
			{
				Tile *tile = _save->getTile(pos + Position(x,y,0));
				Tile *tileBelow = _save->getTile(pos + Position(x,y,-1));
				if (!tile->getMapData(MapData::O_FLOOR) && tileBelow && tileBelow->getTerrainLevel() == -24)
				{
					tileBelow->setMarkerColor(bRemove?0:(tus>0?4:3));
				}
				tile->setMarkerColor(bRemove?0:(tus>0?4:3));
			}
		}
	}
	return true;
}

/*
 * Preview path, unmarks tiles.
 */
bool Pathfinding::removePreview()
{
	if (!_pathPreviewed)
		return false;
	previewPath(true);
	return true;
}

// this works in only x/y plane
bool Pathfinding::bresenhamPath(const Position& origin, const Position& target, BattleUnit *targetUnit, bool sneak, int maxTUCost)
{
	int xd[8] = {0, 1, 1, 1, 0, -1, -1, -1};
	int yd[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

	int x, x0, x1, delta_x, step_x;
	int y, y0, y1, delta_y, step_y;
	int z, z0, z1, delta_z, step_z;
	int swap_xy, swap_xz;
	int drift_xy, drift_xz;
	int cx, cy, cz;
	Position lastPoint(origin);
	int dir;
	int lastTUCost = -1;
	Position nextPoint;
	_totalTUCost = 0;

	//start and end points
	x0 = origin.x;	 x1 = target.x;
	y0 = origin.y;	 y1 = target.y;
	z0 = origin.z;	 z1 = target.z;

	//'steep' xy Line, make longest delta x plane
	swap_xy = abs(y1 - y0) > abs(x1 - x0);
	if (swap_xy)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	//do same for xz
	swap_xz = abs(z1 - z0) > abs(x1 - x0);
	if (swap_xz)
	{
		std::swap(x0, z0);
		std::swap(x1, z1);
	}

	//delta is Length in each plane
	delta_x = abs(x1 - x0);
	delta_y = abs(y1 - y0);
	delta_z = abs(z1 - z0);

	//drift controls when to step in 'shallow' planes
	//starting value keeps Line centred
	drift_xy  = (delta_x / 2);
	drift_xz  = (delta_x / 2);

	//direction of line
	step_x = 1;  if (x0 > x1) {  step_x = -1; }
	step_y = 1;  if (y0 > y1) {  step_y = -1; }
	step_z = 1;  if (z0 > z1) {  step_z = -1; }

	//starting point
	y = y0;
	z = z0;

	//step through longest delta (which we have swapped to x)
	for (x = x0; x != (x1+step_x); x += step_x)
	{
		//copy position
		cx = x;	cy = y;	cz = z;

		//unswap (in reverse)
		if (swap_xz) std::swap(cx, cz);
		if (swap_xy) std::swap(cx, cy);

		if (x != x0 || y != y0 || z != z0)
		{
			Position realNextPoint = Position(cx, cy, cz);
			nextPoint = realNextPoint;
			// get direction
			for (dir = 0; dir < 8; ++dir)
			{
				if (xd[dir] == cx-lastPoint.x && yd[dir] == cy-lastPoint.y) break;
			}
			int tuCost = getTUCost(lastPoint, dir, &nextPoint, _unit, targetUnit, (targetUnit && maxTUCost == 10000));
			
			if (sneak && _save->getTile(nextPoint)->getVisible()) return false;
			
			// delete the following
			if (nextPoint == realNextPoint && tuCost < 255 && (tuCost == lastTUCost || (dir&1 && tuCost == lastTUCost*1.5) || (!(dir&1) && tuCost*1.5 == lastTUCost) || lastTUCost == -1)
				&& !isBlocked(_save->getTile(lastPoint), _save->getTile(nextPoint), dir, targetUnit))
			{
				_path.push_back(dir);
			}
			else
			{
				return false;
			}
			if (targetUnit == 0 && tuCost != 255)
			{
				lastTUCost = tuCost;
				_totalTUCost += tuCost;
			}
			lastPoint = Position(cx, cy, cz);
		}
		//update progress in other planes
		drift_xy = drift_xy - delta_y;
		drift_xz = drift_xz - delta_z;

		//step in y plane
		if (drift_xy < 0)
		{
			y = y + step_y;
			drift_xy = drift_xy + delta_x;
		}

		//same in z
		if (drift_xz < 0)
		{
			z = z + step_z;
			drift_xz = drift_xz + delta_x;
		}
	}

	return true;
}

/**
 * Use Dijkstra's algorithm to locate all tiles reachable to @a *unit with a TU cost no more than @a tuMax.
 * @param unit Pointer to the unit.
 * @param tuMax The maximum cost of the path to each tile.
 * @return An array of reachable tiles, sorted in ascending order of cost. The first tile is the start location.
 */
std::vector<int> Pathfinding::findReachable(BattleUnit *unit, int tuMax)
{
	const Position &start = unit->getPosition();

	for (std::vector<PathfindingNode>::iterator it = _nodes.begin(); it != _nodes.end(); ++it)
	{
		it->reset();
	}
	PathfindingNode *startNode = getNode(start);
	startNode->connect(0, 0, 0);
	PathfindingOpenSet unvisited;
	unvisited.push(startNode);
	std::vector<PathfindingNode*> reachable;
	while (!unvisited.empty())
	{
		PathfindingNode *currentNode = unvisited.pop();
		Position const &currentPos = currentNode->getPosition();

		// Try all reachable neighbours.
		for (int direction = 0; direction < 10; direction++)
		{
			Position nextPos;
			int tuCost = getTUCost(currentPos, direction, &nextPos, unit, 0, false);
			if (tuCost == 255) // Skip unreachable / blocked
				continue;
			if (currentNode->getTUCost(false) + tuCost > tuMax) // Run out of TUs
				continue;
			PathfindingNode *nextNode = getNode(nextPos);
			if (nextNode->isChecked()) // Our algorithm means this node is already at minimum cost.
				continue;
			int totalTuCost = currentNode->getTUCost(false) + tuCost;
			// If this node is unvisited or visited from a better path.
			if (!nextNode->inOpenSet() || nextNode->getTUCost(false) > totalTuCost)
			{
				nextNode->connect(totalTuCost, currentNode, direction);
				unvisited.push(nextNode);
			}
		}
		currentNode->setChecked();
		reachable.push_back(currentNode);
	}
	std::sort(reachable.begin(), reachable.end(), MinNodeCosts());
	std::vector<int> tiles;
	tiles.reserve(reachable.size());
	for (std::vector<PathfindingNode*>::const_iterator it = reachable.begin(); it != reachable.end(); ++it)
	{
		tiles.push_back(_save->getTileIndex((*it)->getPosition()));
	}
	return tiles;
}

bool Pathfinding::getStrafeMove() const {
	return _strafeMove;
}

}
