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
#ifndef OPENXCOM_PATHFINDING_H
#define OPENXCOM_PATHFINDING_H

#include <vector>
#include "Position.h"
#include "../Ruleset/MapData.h"

namespace OpenXcom
{

class Position;
class SavedBattleGame;
class PathfindingNode;
class Tile;
class BattleUnit;

/**
 * A utility class that calculates the shortest path between two points on the battlescape map.
 */
class Pathfinding
{
private:
	SavedBattleGame *_save;
	std::vector<PathfindingNode> _nodes;
	int _size;
	std::vector<int> _path;
	MovementType _movementType;
	/// Gets the node at certain position.
	PathfindingNode *getNode(const Position& pos);
	/// whether a tile blocks a certain movementType
	bool isBlocked(Tile *tile, const int part);
	bool isBlocked(Tile *startTile, Tile *endTile, const int direction);
	bool canFallDown(Tile *destinationTile);
	bool isOnStairs(const Position &startPosition, const Position &endPosition);
	BattleUnit *_unit;
	bool _pathPreviewed;
	///Try to find a straight line path between two positions.
	bool bresenhamPath(const Position& origin, const Position& target);
	///Try to find a path between two positions.
	bool aStarPath(const Position& origin, const Position& target);
public:
	static const int DIR_UP = 8;
	static const int DIR_DOWN = 9;
	static const int O_BIGWALL = -1;
	/// Creates a new Pathfinding class
	Pathfinding(SavedBattleGame *save);
	/// Cleans up the Pathfinding.
	~Pathfinding();
	/// Calculate the shortest path.
	void calculate(BattleUnit *unit, Position endPosition);
	/// Converts direction to a vector.
	static void directionToVector(const int direction, Position *vector);
	/// Check whether a path is ready gives the first direction.
	int getStartDirection();
	/// Dequeue a direction.
	int dequeuePath();
	/// Get's the TU cost to move from 1 tile to the other.
	int getTUCost(const Position &startPosition, const int direction, Position *endPosition, BattleUnit *unit);
	/// Abort the current path.
	void abortPath();
	bool validateUpDown(BattleUnit *bu, Position startPosition, const int direction);
	bool previewPath(bool bRemove = false);
	bool removePreview();
	/// Get all reachable tiles, based on cost.
	std::vector<int> findReachable(BattleUnit *unit, int tuMax);
};

}

#endif
