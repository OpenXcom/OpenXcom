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
#include <list>
#include "Pathfinding.h"
#include "PathfindingNode.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/TerrainObject.h"
#include "../Savegame//BattleUnit.h"

/**
 * Sets up a Pathfinding.
 * @param save pointer to SavedBattleGame object.
 */
Pathfinding::Pathfinding(SavedBattleGame *save) : _save(save), _nodes()
{
	_size = _save->getHeight() * _save->getLength() * _save->getWidth();
	/* allocate the array and the objects in it */
	_nodes = new PathfindingNode*[_size];
	int x, y, z;
	for (int i = 0; i < _size; i++)
	{
		_save->getTileCoords(i, &x, &y, &z);
		_nodes[i] = new PathfindingNode(Position(x, y, z));
	}

}

/**
 * Deletes the Pathfinding.
 */
Pathfinding::~Pathfinding()
{
	for (int i = 0; i < _size; i++)
	{
		delete _nodes[i];
	}
	delete _nodes;

}

/** 
 * Gets the Node on a given position on the map.
 * @param pos position
 * @return Pointer to node.
 */
PathfindingNode *Pathfinding::getNode(const Position& pos)
{
	return _nodes[_save->getTileIndex(pos)];
}

/**
 * Calculate the shortest path using a simple brute force algorithm. 
 * Which works okay with small maps.
 * @param unit
 * @param endPosition
 */

void Pathfinding::calculate(BattleUnit *unit, Position &endPosition)
{
	std::list<PathfindingNode*> openList;
	Position currentPos, nextPos, startPosition = unit->getPosition();
	int tuCost;

	_movementType = WALK; // should be parameter
	_unit = unit;

	Tile *destinationTile = _save->getTile(endPosition);

	// check if destination is not blocked
	if (isBlocked(destinationTile, O_FLOOR) || isBlocked(destinationTile, O_OBJECT)) return;

	// check if we have floor, else lower destination (for non flying units only, because otherwise they never reached this place)
	while (canFallDown(destinationTile))
	{
		endPosition.z--;
		destinationTile = _save->getTile(endPosition);
	}

	_path.clear();

	// reset every node, so we have to check them all
	for (int i = 0; i < _size; i++)
		_nodes[i]->reset();

	// start position is the first one in our "open" list
    openList.push_back(getNode(startPosition));
    openList.front()->check(0, 0, 0, 0);

	// if the open list is empty, we've reached the end
    while(!openList.empty())
	{
		// this algorithm expands in all directions
        for (int direction = 0; direction < 8; direction++)
		{
			currentPos = openList.front()->getPosition();
            tuCost = getTUCost(currentPos, direction, &nextPos, unit);
            if(tuCost < 255)
			{
				if( (!getNode(nextPos)->isChecked() ||
                getNode(nextPos)->getTUCost() > getNode(currentPos)->getTUCost() + tuCost) &&
                (!getNode(endPosition)->isChecked() ||
                getNode(endPosition)->getTUCost() > getNode(currentPos)->getTUCost() + tuCost)
                )
				{
					getNode(nextPos)->check(getNode(currentPos)->getTUCost() + tuCost, 
											getNode(currentPos)->getStepsNum() + 1,
											getNode(currentPos),
											direction);
					openList.push_back(getNode(nextPos));
                }
            }
        }
		openList.pop_front();
    }

    if(!getNode(endPosition)->isChecked()) return;

    //Backward tracking of the path
    PathfindingNode* pf = getNode(endPosition);
    for (int i = getNode(endPosition)->getStepsNum(); i > 0; i--)
	{
		_path.push_back(pf->getPrevDir());
        pf=pf->getPrevNode();
    }

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
int Pathfinding::getTUCost(const Position &startPosition, int direction, Position *endPosition, BattleUnit *unit)
{
	_unit = unit;
	directionToVector(direction, endPosition);
	*endPosition += startPosition;
	bool fellDown = false;

	Tile *startTile = _save->getTile(startPosition);
	Tile *destinationTile = _save->getTile(*endPosition);

	// this means the destination is probably outside the map
	if (!destinationTile)
		return 255;

	// check if we can go this way
	if (isBlocked(startTile, destinationTile, direction))
		return 255;

	// if we are on a stairs try to go up a level
	if (startTile->getTerrainLevel() < -12)
	{
		endPosition->z++;
		destinationTile = _save->getTile(*endPosition);
	}

	// this means the destination is probably outside the map
	if (!destinationTile)
		return 255;

	// check if we have floor, else fall down
	while (canFallDown(destinationTile))
	{
		endPosition->z--;
		destinationTile = _save->getTile(*endPosition);
		fellDown = true;
	}

	// check if the destination tile can be walked over
	if ((isBlocked(destinationTile, O_FLOOR) || isBlocked(destinationTile, O_OBJECT)) && !fellDown)
		return 255;

	// calculate the cost by adding floor walk cost and object walk cost
	int cost = destinationTile->getTUCost(O_FLOOR, _movementType) + destinationTile->getTUCost(O_OBJECT, _movementType);
	
	// diagonal walking (uneven directions) costs 50% more tu's
	if (direction & 1)
	{
		cost = (int)((double)cost * 1.5);
	}

	return cost;
}

/*
 * Converts direction to a vector. Direction starts north = 0 and goes clockwise.
 * @param direction
 * @param vector pointer to a position (which acts as a vector)
 */
void Pathfinding::directionToVector(const int direction, Position *vector)
{
	int x[8] = {0, 1, 1, 1, 0, -1, -1, -1};
	int y[8] = {1, 1, 0, -1, -1, -1, 0, 1};
	vector->x = x[direction];
	vector->y = y[direction];
	vector->z = 0;
}

/*
 * Check whether a path is ready and dequeue it.
 * @return direction where the unit needs to go next, -1 if it's the end of the path.
 */
int Pathfinding::dequeuePath()
{
	if (_path.size() == 0) return -1;
	int last_element = _path.back();
	_path.pop_back();
	return last_element;
}


/*
 * Whether a certain part of a tile blocks movement.
 * @param tile can be null pointer
 * @param movementType
 * @return true/false
 */
bool Pathfinding::isBlocked(Tile *tile, const int part)
{
	if (tile == 0) return true; // probably outside the map here

	if (tile->getTUCost(part, _movementType) == 255) return true; // blocking part

	BattleUnit *unit = _save->selectUnit(tile->getPosition());
	if (unit != 0 && unit != _unit && (part==0 || part==3)) return true;

	if (tile->isBigWall()) return true; // big walls block every part

	return false;
}

/**
 * Whether going from one tile to another blocks movement.
 * @param startTile
 * @param endTile
 * @param direction
 * @return true/false
 */
bool Pathfinding::isBlocked(Tile *startTile, Tile *endTile, const int direction)
{

	// check if the difference in height between start and destination is not too high
	// so we can not jump to the highest part of the stairs from the floor
	// stairs terrainlevel goes typically -8 -16 (2 steps) or -4 -12 -20 (3 steps)
	// this "maximum jump height" is therefore set to 8
	if (startTile->getTerrainLevel() - endTile->getTerrainLevel() > 8)
		return true;

	switch(direction)
	{
	case 0:	// north
		if (isBlocked(startTile, O_NORTHWALL)) return true;
		break;
	case 1: // north east
		if (isBlocked(startTile,O_NORTHWALL)) return true;
		if (isBlocked(endTile,O_WESTWALL)) return true;
		if (isBlocked(_save->getTile(startTile->getPosition() + Position(1, 0, 0)),O_WESTWALL)) return true;
		if (isBlocked(_save->getTile(startTile->getPosition() + Position(1, 0, 0)),O_NORTHWALL)) return true;
		break;
	case 2: // east
		if (isBlocked(endTile,O_WESTWALL)) return true;
		break;
	case 3: // south east
		if (isBlocked(endTile,O_WESTWALL)) return true;
		if (isBlocked(endTile,O_NORTHWALL)) return true;
		if (isBlocked(_save->getTile(startTile->getPosition() + Position(1, 0, 0)),O_WESTWALL)) return true;
		if (isBlocked(_save->getTile(startTile->getPosition() + Position(0, -1, 0)),O_NORTHWALL)) return true;
		break;
	case 4: // south
		if (isBlocked(endTile,O_NORTHWALL)) return true;
		break;
	case 5: // south west
		if (isBlocked(endTile,O_NORTHWALL)) return true;
		if (isBlocked(startTile,O_WESTWALL)) return true;
		if (isBlocked(_save->getTile(startTile->getPosition() + Position(0, -1, 0)),O_WESTWALL)) return true;
		if (isBlocked(_save->getTile(startTile->getPosition() + Position(0, -1, 0)),O_NORTHWALL)) return true;
		break;
	case 6: // west
		if (isBlocked(startTile,O_WESTWALL)) return true;
		break;
	case 7: // north west
		if (isBlocked(startTile,O_WESTWALL)) return true;
		if (isBlocked(startTile,O_NORTHWALL)) return true;
		if (isBlocked(_save->getTile(startTile->getPosition() + Position(0, 1, 0)),O_WESTWALL)) return true;
		if (isBlocked(_save->getTile(startTile->getPosition() + Position(-1, 0, 0)),O_NORTHWALL)) return true;
		break;
	}

	return false;
}

/**
 * We can fall down here, if the tile does not exist, the tile has no floor
 * the current position is higher than 0
 * @param here
 * @return bool
 */
bool Pathfinding::canFallDown(Tile *here)
{
	return (!here || here->hasNoFloor()) && here->getPosition().z > 0;
}
