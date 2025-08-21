/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <algorithm>
#include "Pathfinding.h"
#include "PathfindingOpenSet.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Mod/Armor.h"
#include "../Savegame/BattleUnit.h"
#include "../Engine/Options.h"
#include "BattlescapeGame.h"

namespace OpenXcom
{

int Pathfinding::red = 3;
int Pathfinding::yellow = 10;
int Pathfinding::green = 4;

/**
 * Sets up a Pathfinding.
 * @param save pointer to SavedBattleGame object.
 */
Pathfinding::Pathfinding(SavedBattleGame *save) : _save(save), _unit(0), _pathPreviewed(false), _strafeMove(false), _totalTUCost(0), _modifierUsed(false), _movementType(MT_WALK)
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
 * @param pos Position.
 * @return Pointer to node.
 */
PathfindingNode *Pathfinding::getNode(Position pos)
{
	return &_nodes[_save->getTileIndex(pos)];
}

/**
 * Calculates the shortest path.
 * @param unit Unit taking the path.
 * @param endPosition The position we want to reach.
 * @param target Target of the path.
 * @param maxTUCost Maximum time units the path can cost.
 */
void Pathfinding::calculate(BattleUnit *unit, Position endPosition, BattleUnit *target, int maxTUCost)
{
	_totalTUCost = 0;
	_path.clear();
	// i'm DONE with these out of bounds errors.
	if (endPosition.x > _save->getMapSizeX() - unit->getArmor()->getSize() || endPosition.y > _save->getMapSizeY() - unit->getArmor()->getSize() || endPosition.x < 0 || endPosition.y < 0) return;

	bool sneak = Options::sneakyAI && unit->getFaction() == FACTION_HOSTILE;

	Position startPosition = unit->getPosition();
	_movementType = unit->getMovementType();
	if (target != 0 && maxTUCost == -1)  // pathfinding for missile
	{
		_movementType = MT_FLY;
		maxTUCost = 10000;
	}
	_unit = unit;

	Tile *destinationTile = _save->getTile(endPosition);

	// check if destination is not blocked
	if (isBlocked(destinationTile, O_FLOOR, target) || isBlocked(destinationTile, O_OBJECT, target)) return;

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
	// make sure we didn't just try to adjust the Z of our destination outside the map
	// this occurs in rare circumstances where an object has terrainLevel -24 on the top floor
	// and is considered passable terrain for whatever reason (usually bigwall type objects)
	if (endPosition.z == _save->getMapSizeZ())
	{
		return; // Icarus is a bad role model for XCom soldiers.
	}
	// check if we have floor, else lower destination (for non flying units only, because otherwise they never reached this place)
	while (canFallDown(destinationTile, _unit->getArmor()->getSize()) && _movementType != MT_FLY)
	{
		endPosition.z--;
		destinationTile = _save->getTile(endPosition);
	}
	// check if destination is not blocked
	if (isBlocked(destinationTile, O_FLOOR, target) || isBlocked(destinationTile, O_OBJECT, target)) return;
	int size = unit->getArmor()->getSize()-1;
	if (size >= 1)
	{
		int its = 0;
		const int dir[3] = {4,2,3};
		for (int x = 0; x <= size; x += size)
		{
			for (int y = 0; y <= size; y += size)
			{
				if (x || y)
				{
					Tile *checkTile = _save->getTile(endPosition + Position(x, y, 0));
					if ((isBlocked(destinationTile, checkTile, dir[its], unit) &&
						isBlocked(destinationTile, checkTile, dir[its], target))||
						(checkTile->getUnit() &&
						checkTile->getUnit() != unit &&
						checkTile->getUnit()->getVisible() &&
						checkTile->getUnit() != target))
						return;
					if (x && y)
					{
						if ((checkTile->getMapData(O_NORTHWALL) && checkTile->getMapData(O_NORTHWALL)->isDoor()) ||
							(checkTile->getMapData(O_WESTWALL) && checkTile->getMapData(O_WESTWALL)->isDoor()))
							return;
					}
					++its;
				}
			}
		}
	}
	// Strafing move allowed only to adjacent squares on same z. "Same z" rule mainly to simplify walking render.
	_strafeMove = Options::strafe && (SDL_GetModState() & KMOD_CTRL) != 0 && (startPosition.z == endPosition.z) &&
							(abs(startPosition.x - endPosition.x) <= 1) && (abs(startPosition.y - endPosition.y) <= 1);

	// look for a possible fast and accurate bresenham path and skip A*
	if (startPosition.z == endPosition.z && bresenhamPath(startPosition,endPosition, target, sneak))
	{
		std::reverse(_path.begin(), _path.end()); //paths are stored in reverse order
		return;
	}
	else
	{
		abortPath(); // if bresenham failed, we shouldn't keep the path it was attempting, in case A* fails too.
	}
	// Now try through A*.
	if (!aStarPath(startPosition, endPosition, target, sneak, maxTUCost))
	{
		abortPath();
	}
}

/**
 * Calculates the shortest path using a simple A-Star algorithm.
 * The unit information and movement type must have already been set.
 * The path information is set only if a valid path is found.
 * @param startPosition The position to start from.
 * @param endPosition The position we want to reach.
 * @param target Target of the path.
 * @param sneak Is the unit sneaking?
 * @param maxTUCost Maximum time units the path can cost.
 * @return True if a path exists, false otherwise.
 */
bool Pathfinding::aStarPath(Position startPosition, Position endPosition, BattleUnit *target, bool sneak, int maxTUCost)
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
	while (!openList.empty())
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
			if (sneak && _save->getTile(nextPos)->getVisible()) tuCost *= 2; // avoid being seen
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
	// Unable to reach the target
	return false;
}

/**
 * Gets the TU cost to move from 1 tile to the other (ONE STEP ONLY).
 * But also updates the endPosition, because it is possible
 * the unit goes upstairs or falls down while walking.
 * @param startPosition The position to start from.
 * @param direction The direction we are facing.
 * @param endPosition The position we want to reach.
 * @param unit The unit moving.
 * @param target The target unit.
 * @param missile Is this a guided missile?
 * @return TU cost or 255 if movement is impossible.
 */
int Pathfinding::getTUCost(Position startPosition, int direction, Position *endPosition, BattleUnit *unit, BattleUnit *target, bool missile)
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
			if (!x && !y && _movementType != MT_FLY && canFallDown(startTile, size+1))
			{
				if (direction != DIR_DOWN)
				{
					return 255; //cannot walk on air
				}
				else
				{
					fellDown = true;
				}
			}
			if (direction < DIR_UP && startTile->getTerrainLevel() > - 16)
			{
				// check if we can go this way
				if (isBlocked(startTile, destinationTile, direction, target))
					return 255;
				if (startTile->getTerrainLevel() - destinationTile->getTerrainLevel() > 8)
					return 255;
			}

			// this will later be used to re-cast the start tile again.
			Position verticalOffset (0, 0, 0);

			// if we are on a stairs try to go up a level
			if (direction < DIR_UP && startTile->getTerrainLevel() <= -16 && aboveDestination && !aboveDestination->hasNoFloor(destinationTile))
			{
					numberOfPartsGoingUp++;
					verticalOffset.z++;

					if (!triedStairs)
					{
						endPosition->z++;
						destinationTile = _save->getTile(*endPosition + offset);
						belowDestination = _save->getTile(*endPosition + Position(x,y,-1));
						triedStairs = true;
					}
			}
			else if (direction < DIR_UP && !fellDown && _movementType != MT_FLY && belowDestination && canFallDown(destinationTile) && belowDestination->getTerrainLevel() <= -12)
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
			else if (!missile && _movementType == MT_FLY && belowDestination && belowDestination->getUnit() && belowDestination->getUnit() != unit)
			{
				// 2 or more voxels poking into this tile = no go
				if (belowDestination->getUnit()->getHeight() + belowDestination->getUnit()->getFloatHeight() - belowDestination->getTerrainLevel() > 26)
				{
					return 255;
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
			else if (direction >= DIR_UP && !fellDown)
			{
				// check if we can go up or down through gravlift or fly
				if (validateUpDown(unit, startPosition + offset, direction, missile))
				{
					cost = 8; // vertical movement by flying suit or grav lift
				}
				else
				{
					return 255;
				}
			}

			// check if we have floor, else fall down
			if (_movementType != MT_FLY && !fellDown && canFallDown(startTile))
			{
				numberOfPartsFalling++;

				if (numberOfPartsFalling == (size+1)*(size+1) && direction != DIR_DOWN)
				{
						return 0;
				}
			}
			startTile = _save->getTile(startTile->getPosition() + verticalOffset);


			if (direction < DIR_UP && numberOfPartsGoingUp != 0)
			{
				// check if we can go this way
				if (isBlocked(startTile, destinationTile, direction, target))
					return 255;
				if (startTile->getTerrainLevel() - destinationTile->getTerrainLevel() > 8)
					return 255;
			}

			int wallCounter = 0;
			int wallTmp = 0;
			int wallcost = 0; // walking through rubble walls, but don't charge for walking diagonally through doors (which is impossible),
							// they're a special case unto themselves, if we can walk past them diagonally, it means we can go around,
							// as there is no wall blocking us.
			if (direction == 0 || direction == 7 || direction == 1)
			{
				wallTmp += startTile->getTUCost(O_NORTHWALL, _movementType);
				if (wallTmp > 0)
				{
					wallcost += wallTmp;
					wallCounter += 1;
				}
			}
			if (!fellDown && (direction == 2 || direction == 1 || direction == 3))
			{
				wallTmp += destinationTile->getTUCost(O_WESTWALL, _movementType);
				if (wallTmp > 0)
				{
					wallcost += wallTmp;
					wallCounter += 1;
				}
			}
			if (!fellDown && (direction == 4 || direction == 3 || direction == 5))
			{
				wallTmp += destinationTile->getTUCost(O_NORTHWALL, _movementType);
				if (wallTmp > 0)
				{
					wallcost += wallTmp;
					wallCounter += 1;
				}
			}
			if (direction == 6 || direction == 5 || direction == 7)
			{
				wallTmp += startTile->getTUCost(O_WESTWALL, _movementType);
				if (wallTmp > 0)
				{
					wallcost += wallTmp;
					wallCounter += 1;
				}
			}
			// "average" cost: https://openxcom.org/forum/index.php?topic=12589.0
			if (wallCounter > 0)
			{
				wallcost /= wallCounter;
			}

			// don't let tanks phase through doors.
			if (x && y)
			{
				if ((destinationTile->getMapData(O_NORTHWALL) && destinationTile->getMapData(O_NORTHWALL)->isDoor()) ||
					(destinationTile->getMapData(O_WESTWALL) && destinationTile->getMapData(O_WESTWALL)->isDoor()))
				{
					return 255;
				}
			}
			// check if the destination tile can be walked over
			if (isBlocked(destinationTile, O_FLOOR, target) || isBlocked(destinationTile, O_OBJECT, target))
			{
				return 255;
			}

			// if we don't want to fall down and there is no floor, we can't know the TUs so it's default to 4
			if (direction < DIR_UP && !fellDown && destinationTile->hasNoFloor(0))
			{
				cost = 4;
			}
			// calculate the cost by adding floor walk cost and object walk cost
			if (direction < DIR_UP)
			{
				cost += destinationTile->getTUCost(O_FLOOR, _movementType);
				if (!fellDown && !triedStairs && destinationTile->getMapData(O_OBJECT))
				{
					cost += destinationTile->getTUCost(O_OBJECT, _movementType);
				}
				// climbing up a level costs one extra
				if (verticalOffset.z > 0)
				{
					cost++;
				}
			}

			// diagonal walking (uneven directions) costs 50% more tu's
			if (direction < DIR_UP && direction & 1)
			{
				cost = (int)((double)cost * 1.5);
			}
			cost += wallcost;
			if (_unit->getFaction() != FACTION_PLAYER &&
				_unit->getSpecialAbility() < SPECAB_BURNFLOOR &&
				destinationTile->getFire() > 0)
				cost += 32; // try to find a better path, but don't exclude this path entirely.

			// TFTD thing: underwater tiles on fire or filled with smoke cost 2 TUs more for whatever reason.
			if (_save->getDepth() > 0 && (destinationTile->getFire() > 0 || destinationTile->getSmoke() > 0))
			{
				cost += 2;
			}

			if (missile && destinationTile->getUnit())
			{
				BattleUnit *unitHere = destinationTile->getUnit();
				if (unitHere != target && !unitHere->isOut())
				{
					if (unitHere->getFaction() == _unit->getFaction())
					{
						return 255; // consider any tile occupied by a friendly as being blocked
					}
					else if (unit->getUnitRules() && unitHere->getTurnsSinceSpotted() <= unit->getUnitRules()->getIntelligence())
					{
						return 255; // consider any tile occupied by a known unit that isn't our target as being blocked
					}
				}
			}

			// Strafing costs +1 for forwards-ish or sidewards, propose +2 for backwards-ish directions
			// Maybe if flying then it makes no difference?
			if (Options::strafe && _strafeMove) {
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

	// for bigger sized units, check the path between parts in an X shape at the end position
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
		startTile = _save->getTile(*endPosition + Position(1,0,0));
		destinationTile = _save->getTile(*endPosition + Position(0,1,0));
		tmpDirection = 5;
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

/**
 * Converts direction to a vector. Direction starts north = 0 and goes clockwise.
 * @param direction Source direction.
 * @param vector Pointer to a position (which acts as a vector).
 */
void Pathfinding::directionToVector(int direction, Position *vector)
{
	int x[10] = {0, 1, 1, 1, 0, -1, -1, -1,0,0};
	int y[10] = {-1, -1, 0, 1, 1, 1, 0, -1,0,0};
	int z[10] = {0, 0, 0, 0, 0, 0, 0, 0, 1, -1};
	vector->x = x[direction];
	vector->y = y[direction];
	vector->z = z[direction];
}

/**
 * Converts direction to a vector. Direction starts north = 0 and goes clockwise.
 * @param vector Pointer to a position (which acts as a vector).
 * @param dir Resulting direction.
 */
void Pathfinding::vectorToDirection(Position vector, int &dir)
{
	dir = -1;
	int x[8] = {0, 1, 1, 1, 0, -1, -1, -1};
	int y[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
	for (int i = 0; i < 8; ++i)
	{
		if (x[i] == vector.x && y[i] == vector.y)
		{
			dir = i;
			return;
		}
	}
}

/**
 * Checks whether a path is ready and gives the first direction.
 * @return Direction where the unit needs to go next, -1 if it's the end of the path.
 */
int Pathfinding::getStartDirection() const
{
	if (_path.empty()) return -1;
	return _path.back();
}

/**
 * Dequeues the next path direction. Ie returns the direction and removes it from queue.
 * @return Direction where the unit needs to go next, -1 if it's the end of the path.
 */
int Pathfinding::dequeuePath()
{
	if (_path.empty()) return -1;
	int last_element = _path.back();
	_path.pop_back();
	return last_element;
}

/**
 * Aborts the current path. Clears the path vector.
 */
void Pathfinding::abortPath()
{
	_totalTUCost = 0;
	_path.clear();
}


/**
 * Determines whether a certain part of a tile blocks movement.
 * @param tile Specified tile, can be a null pointer.
 * @param part Part of the tile.
 * @param missileTarget Target for a missile.
 * @return True if the movement is blocked.
 */
bool Pathfinding::isBlocked(Tile *tile, const int part, BattleUnit *missileTarget, int bigWallExclusion) const
{
	if (tile == 0) return true; // probably outside the map here

	if (part == O_BIGWALL)
	{
		if (tile->getMapData(O_OBJECT) &&
			tile->getMapData(O_OBJECT)->getBigWall() != 0 &&
			tile->getMapData(O_OBJECT)->getBigWall() <= BIGWALLNWSE &&
			tile->getMapData(O_OBJECT)->getBigWall() != bigWallExclusion)
			return true; // blocking part
		else
			return false;
	}
	if (part == O_WESTWALL)
	{
		if (tile->getMapData(O_OBJECT) &&
			(tile->getMapData(O_OBJECT)->getBigWall() == BIGWALLWEST ||
			tile->getMapData(O_OBJECT)->getBigWall() == BIGWALLWESTANDNORTH))
			return true; // blocking part
		Tile *tileWest = _save->getTile(tile->getPosition() + Position(-1, 0, 0));
		if (!tileWest) return true;	// do not look outside of map
		if (tileWest->getMapData(O_OBJECT) &&
			(tileWest->getMapData(O_OBJECT)->getBigWall() == BIGWALLEAST ||
			tileWest->getMapData(O_OBJECT)->getBigWall() == BIGWALLEASTANDSOUTH))
			return true; // blocking part
	}
	if (part == O_NORTHWALL)
	{
		if (tile->getMapData(O_OBJECT) &&
			(tile->getMapData(O_OBJECT)->getBigWall() == BIGWALLNORTH ||
			 tile->getMapData(O_OBJECT)->getBigWall() == BIGWALLWESTANDNORTH))
			return true; // blocking part
		Tile *tileNorth = _save->getTile(tile->getPosition() + Position(0, -1, 0));
		if (!tileNorth)
			return true; // do not look outside of map
		if (tileNorth->getMapData(O_OBJECT) &&
			(tileNorth->getMapData(O_OBJECT)->getBigWall() == BIGWALLSOUTH ||
			 tileNorth->getMapData(O_OBJECT)->getBigWall() == BIGWALLEASTANDSOUTH))
			return true; // blocking part
	}
	if (part == O_FLOOR)
	{
		if (tile->getUnit())
		{
			BattleUnit *unit = tile->getUnit();
			if (unit == _unit || unit == missileTarget || unit->isOut())
				return false;
			if (_unit)
			{
				if (_unit->getFaction() == FACTION_PLAYER && unit->getVisible())
					return true; // player know all visible units
				if (_unit->getFaction() == unit->getFaction())
					return true;
				if (_unit->getFaction() == FACTION_HOSTILE &&
					std::find(_unit->getUnitsSpottedThisTurn().begin(), _unit->getUnitsSpottedThisTurn().end(), unit) != _unit->getUnitsSpottedThisTurn().end())
					return true;
			}
		}
		else if (tile->hasNoFloor(0) && _movementType != MT_FLY) // this whole section is devoted to making large units not take part in any kind of falling behaviour
		{
			Position pos = tile->getPosition();
			while (pos.z >= 0)
			{
				Tile *t = _save->getTile(pos);
				BattleUnit *unit = t->getUnit();

				if (unit != 0 && unit != _unit)
				{
					// don't let large units fall on other units
					if (_unit && _unit->getArmor()->getSize() > 1)
					{
						return true;
					}
					// don't let any units fall on large units
					if (unit != _unit && unit != missileTarget && !unit->isOut() && unit->getArmor()->getSize() > 1)
					{
						return true;
					}
				}
				// not gonna fall any further, so we can stop checking.
				if (!t->hasNoFloor(0))
				{
					break;
				}
				pos.z--;
			}
		}
	}
	// missiles can't pathfind through closed doors.
	{
		TilePart tp = (TilePart)part;
		if (missileTarget != 0 && tile->getMapData(tp) &&
			(tile->getMapData(tp)->isDoor() ||
			 (tile->getMapData(tp)->isUFODoor() &&
			  !tile->isUfoDoorOpen(tp))))
		{
			return true;
		}
	}
	if (tile->getTUCost(part, _movementType) == 255) return true; // blocking part
	return false;
}

/**
 * Determines whether going from one tile to another blocks movement.
 * @param startTile The tile to start from.
 * @param endTile The tile we want to reach.
 * @param direction The direction we are facing.
 * @param missileTarget Target for a missile.
 * @return True if the movement is blocked.
 */
bool Pathfinding::isBlocked(Tile *startTile, Tile * /* endTile */, const int direction, BattleUnit *missileTarget)
{

	// check if the difference in height between start and destination is not too high
	// so we can not jump to the highest part of the stairs from the floor
	// stairs terrainlevel goes typically -8 -16 (2 steps) or -4 -12 -20 (3 steps)
	// this "maximum jump height" is therefore set to 8

	const Position currentPosition = startTile->getPosition();
	static const Position oneTileNorth = Position(0, -1, 0);
	static const Position oneTileEast = Position(1, 0, 0);
	static const Position oneTileSouth = Position(0, 1, 0);
	static const Position oneTileWest = Position(-1, 0, 0);

	switch(direction)
	{
	case 0:	// north
		if (isBlocked(startTile, O_NORTHWALL, missileTarget)) return true;
		break;
	case 1: // north-east
		if (isBlocked(startTile,O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileNorth + oneTileEast),O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileEast),O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileEast),O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileEast), O_BIGWALL, missileTarget, BIGWALLNESW)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileNorth), O_BIGWALL, missileTarget, BIGWALLNESW)) return true;
		break;
	case 2: // east
		if (isBlocked(_save->getTile(currentPosition + oneTileEast), O_WESTWALL, missileTarget)) return true;
		break;
	case 3: // south-east
		if (isBlocked(_save->getTile(currentPosition + oneTileEast), O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileSouth), O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileSouth + oneTileEast), O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileSouth + oneTileEast), O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileEast), O_BIGWALL, missileTarget, BIGWALLNWSE)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileSouth), O_BIGWALL, missileTarget, BIGWALLNWSE)) return true;
		break;
	case 4: // south
		if (isBlocked(_save->getTile(currentPosition + oneTileSouth), O_NORTHWALL, missileTarget)) return true;
		break;
	case 5: // south-west
		if (isBlocked(startTile, O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileSouth), O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileSouth), O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileSouth), O_BIGWALL, missileTarget, BIGWALLNESW)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileWest), O_BIGWALL, missileTarget, BIGWALLNESW)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileSouth + oneTileWest), O_NORTHWALL, missileTarget)) return true;
		break;
	case 6: // west
		if (isBlocked(startTile, O_WESTWALL, missileTarget)) return true;
		break;
	case 7: // north-west
		if (isBlocked(startTile, O_WESTWALL, missileTarget)) return true;
		if (isBlocked(startTile, O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileWest), O_NORTHWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileNorth), O_WESTWALL, missileTarget)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileNorth), O_BIGWALL, missileTarget, BIGWALLNWSE)) return true;
		if (isBlocked(_save->getTile(currentPosition + oneTileWest), O_BIGWALL, missileTarget, BIGWALLNWSE)) return true;
		break;
	}

	return false;
}

/**
 * Determines whether a unit can fall down from this tile.
 * We can fall down here, if the tile does not exist, the tile has no floor
 * the current position is higher than 0, if there is no unit standing below us.
 * @param here The current tile.
 * @return True if a unit can fall down.
 */
bool Pathfinding::canFallDown(Tile *here) const
{
	if (here->getPosition().z == 0)
		return false;
	Tile* tileBelow = _save->getTile(here->getPosition() - Position (0,0,1));

	return here->hasNoFloor(tileBelow);
}

/**
 * Determines whether a unit can fall down from this tile.
 * We can fall down here, if the tile does not exist, the tile has no floor
 * the current position is higher than 0, if there is no unit standing below us.
 * @param here The current tile.
 * @param size The size of the unit.
 * @return True if a unit can fall down.
 */
bool Pathfinding::canFallDown(Tile *here, int size) const
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
 * Determines whether the unit is going up a stairs.
 * @param startPosition The position to start from.
 * @param endPosition The position we wanna reach.
 * @return True if the unit is going up a stairs.
 */
bool Pathfinding::isOnStairs(Position startPosition, Position endPosition) const
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

	//TFTD stairs 1 : endposition has to the south a terrainlevel -18 object (upper part of the stairs)
	if (_save->getTile(endPosition + Position(0, 1, 0)) && _save->getTile(endPosition + Position(0, 1, 0))->getTerrainLevel() == -18)
	{
		// condition 2 : one position further to the south there has to be a terrainlevel -8 object (lower part of the stairs)
		if (_save->getTile(endPosition + Position(0, 2, 0)) && _save->getTile(endPosition + Position(0, 2, 0))->getTerrainLevel() != -12)
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
	if (_save->getTile(endPosition + Position(1, 0, 0)) && _save->getTile(endPosition + Position(1, 0, 0))->getTerrainLevel() == -18)
	{
		if (_save->getTile(endPosition + Position(2, 0, 0)) && _save->getTile(endPosition + Position(2, 0, 0))->getTerrainLevel() != -12)
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
 * Checks, for the up/down button, if the movement is valid. Either there is a grav lift or the unit can fly and there are no obstructions.
 * @param bu Pointer to unit.
 * @param startPosition Unit starting position.
 * @param direction Up or Down
 * @return bool Whether it's valid.
 */
bool Pathfinding::validateUpDown(BattleUnit *bu, const Position& startPosition, const int direction, bool missile) const
{
	Position endPosition;
	directionToVector(direction, &endPosition);
	endPosition += startPosition;
	Tile *startTile = _save->getTile(startPosition);
	Tile *belowStart = _save->getTile(startPosition + Position(0,0,-1));
	Tile *destinationTile = _save->getTile(endPosition);
	if (startTile->getMapData(O_FLOOR) && destinationTile && destinationTile->getMapData(O_FLOOR) &&
		(startTile->getMapData(O_FLOOR)->isGravLift() && destinationTile->getMapData(O_FLOOR)->isGravLift()))
	{
		if (missile)
		{
			if (direction == DIR_UP)
			{
				if (destinationTile->getMapData(O_FLOOR)->getLoftID(0) != 0)
					return false;
			}
			else if (startTile->getMapData(O_FLOOR)->getLoftID(0) != 0)
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		if (bu->getMovementType() == MT_FLY)
		{
			if ((direction == DIR_UP && destinationTile && destinationTile->hasNoFloor(startTile)) // flying up only possible when there is no roof
				|| (direction == DIR_DOWN && destinationTile && startTile->hasNoFloor(belowStart)) // falling down only possible when there is no floor
				)
			{
				return true;
			}
		}
	}

	return false;
}

/**
 * Marks tiles for the path preview.
 * @param bRemove Remove preview?
 * @return True, if a path is previewed.
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
	if (_unit->isKneeled())
	{
		tus -= 8;
	}
	int energy = _unit->getEnergy();
	int size = _unit->getArmor()->getSize() - 1;
	int total = _unit->isKneeled() ? 8 : 0;
	bool switchBack = false;
	if (_save->getBattleGame()->getReservedAction() == BA_NONE)
	{
		switchBack = true;
		_save->getBattleGame()->setTUReserved(BA_AUTOSHOT);
	}
	_modifierUsed = (SDL_GetModState() & KMOD_CTRL) != 0;
	bool running = Options::strafe && _modifierUsed && _unit->getArmor()->getSize() == 1 && _path.size() > 1;
	for (std::vector<int>::reverse_iterator i = _path.rbegin(); i != _path.rend(); ++i)
	{
		int dir = *i;
		int tu = getTUCost(pos, dir, &destination, _unit, 0, false); // gets tu cost, but also gets the destination position.
		int energyUse = tu;
		if (dir >= Pathfinding::DIR_UP)
		{
			energyUse = 0;
		}
		else if (running)
		{
			tu *= 0.75;
			energyUse *= 1.5;
		}
		energy -= energyUse / 2;
		tus -= tu;
		total += tu;
		bool reserve = _save->getBattleGame()->checkReservedTU(_unit, total, true);
		pos = destination;
		for (int x = size; x >= 0; x--)
		{
			for (int y = size; y >= 0; y--)
			{
				Tile *tile = _save->getTile(pos + Position(x,y,0));
				Tile *tileAbove = _save->getTile(pos + Position(x,y,1));
				if (!bRemove)
				{
					if (i == _path.rend() - 1)
					{
						tile->setPreview(10);
					}
					else
					{
						int nextDir = *(i + 1);
						tile->setPreview(nextDir);
					}
					if ((x && y) || size == 0)
					{
						tile->setTUMarker(std::max(0,tus));
					}
					if (tileAbove && tileAbove->getPreview() == 0 && tu == 0 && _movementType != MT_FLY) //unit fell down, retroactively make the tile above's direction marker to DOWN
					{
						tileAbove->setPreview(DIR_DOWN);
					}
				}
				else
				{
					tile->setPreview(-1);
					tile->setTUMarker(-1);
				}
				tile->setMarkerColor(bRemove?0:((tus>=0 && energy>=0)?(reserve?Pathfinding::green : Pathfinding::yellow) : Pathfinding::red));
			}
		}
	}
	if (switchBack)
	{
		_save->getBattleGame()->setTUReserved(BA_NONE);
	}
	return true;
}

/**
 * Unmarks the tiles used for the path preview.
 * @return True, if the previewed path was removed.
 */
bool Pathfinding::removePreview()
{
	if (!_pathPreviewed)
		return false;
	previewPath(true);
	return true;
}

/**
 * Calculates the shortest path using Brensenham path algorithm.
 * @note This only works in the X/Y plane.
 * @param origin The position to start from.
 * @param target The position we want to reach.
 * @param targetUnit Target of the path.
 * @param sneak Is the unit sneaking?
 * @param maxTUCost Maximum time units the path can cost.
 * @return True if a path exists, false otherwise.
 */
bool Pathfinding::bresenhamPath(Position origin, Position target, BattleUnit *targetUnit, bool sneak, int maxTUCost)
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
			bool isDiagonal = (dir&1);
			int lastTUCostDiagonal = lastTUCost + lastTUCost / 2;
			int tuCostDiagonal = tuCost + tuCost / 2;
			if (nextPoint == realNextPoint && tuCost < 255 && (tuCost == lastTUCost || (isDiagonal && tuCost == lastTUCostDiagonal) || (!isDiagonal && tuCostDiagonal == lastTUCost) || lastTUCost == -1)
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
 * Locates all tiles reachable to @a *unit with a TU cost no more than @a tuMax.
 * Uses Dijkstra's algorithm.
 * @param unit Pointer to the unit.
 * @param tuMax The maximum cost of the path to each tile.
 * @return An array of reachable tiles, sorted in ascending order of cost. The first tile is the start location.
 */
std::vector<int> Pathfinding::findReachable(BattleUnit *unit, int tuMax)
{
	Position start = unit->getPosition();
	int energyMax = unit->getEnergy();
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
			if (currentNode->getTUCost(false) + tuCost > tuMax ||
				(currentNode->getTUCost(false) + tuCost) / 2 > energyMax) // Run out of TUs/Energy
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

/**
 * Gets the strafe move setting.
 * @return Strafe move.
 */
bool Pathfinding::getStrafeMove() const
{
	return _strafeMove;
}

/**
 * Gets the path preview setting.
 * @return True, if paths are previewed.
 */
bool Pathfinding::isPathPreviewed() const
{
	return _pathPreviewed;
}

/**
 * Sets _unit in order to abuse low-level pathfinding functions from outside the class.
 * @param unit Unit taking the path.
 */
void Pathfinding::setUnit(BattleUnit* unit)
{
	_unit = unit;
	if (unit != 0)
	{
		_movementType = unit->getMovementType();
	}
	else
	{
		_movementType = MT_WALK;
	}
}

/**
 * Checks whether a modifier key was used to enable strafing or running.
 * @return True, if a modifier was used.
 */
bool Pathfinding::isModifierUsed() const
{
	return _modifierUsed;
}

/**
 * Gets a reference to the current path.
 * @return the actual path.
 */
const std::vector<int> &Pathfinding::getPath() const
{
	return _path;
}

/**
 * Makes a copy of the current path.
 * @return a copy of the path.
 */
std::vector<int> Pathfinding::copyPath() const
{
	return _path;
}

}
