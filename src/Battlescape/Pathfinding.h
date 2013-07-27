/*
 * Copyright 2010-2013 OpenXcom Developers.
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
	bool isBlocked(Tile *tile, const int part, BattleUnit *missileTarget, int bigWallExclusion = -1);
	///Try to find a straight line path between two positions.
	bool bresenhamPath(const Position& origin, const Position& target, BattleUnit *missileTarget, bool sneak = false, int maxTUCost = 1000);
	///Try to find a path between two positions.
	bool aStarPath(const Position& origin, const Position& target, BattleUnit *missileTarget, bool sneak = false, int maxTUCost = 1000);
	bool canFallDown(Tile *destinationTile);
	bool canFallDown(Tile *destinationTile, int size);
	BattleUnit *_unit;
	bool _pathPreviewed;
	bool _strafeMove;
	int _totalTUCost;
public:
	bool isOnStairs(const Position &startPosition, const Position &endPosition);
	/// whether or not movement between starttile and endtile is possible in the direction.
	bool isBlocked(Tile *startTile, Tile *endTile, const int direction, BattleUnit *missileTarget);
	static const int DIR_UP = 8;
	static const int DIR_DOWN = 9;
	enum bigWallTypes{ BLOCK = 1, BIGWALLNESW, BIGWALLNWSE, BIGWALLWEST, BIGWALLNORTH, BIGWALLEAST, BIGWALLSOUTH, BIGWALLEASTANDSOUTH};
	static const int O_BIGWALL = -1;
	/// Creates a new Pathfinding class
	Pathfinding(SavedBattleGame *save);
	/// Cleans up the Pathfinding.
	~Pathfinding();
	/// Calculate the shortest path.
	void calculate(BattleUnit *unit, Position endPosition, BattleUnit *missileTarget = 0, int maxTUCost = 1000);
	/// Converts direction to a vector.
	static void directionToVector(const int direction, Position *vector);
	/// convert a vector to a direction.
	static void vectorToDirection(const Position &vector, int &dir);
	/// Check whether a path is ready gives the first direction.
	int getStartDirection();
	/// Dequeue a direction.
	int dequeuePath();
	/// Get's the TU cost to move from 1 tile to the other.
	int getTUCost(const Position &startPosition, const int direction, Position *endPosition, BattleUnit *unit, BattleUnit *target, bool missile);
	/// Abort the current path.
	void abortPath();
	bool getStrafeMove() const;
	bool validateUpDown(BattleUnit *bu, Position startPosition, const int direction);
	bool previewPath(bool bRemove = false);
	bool removePreview();
	/// Set _unit in order to abuse low-level pathfinding functions from outside the class
	void setUnit(BattleUnit *unit);
	/// Get all reachable tiles, based on cost.
	std::vector<int> findReachable(BattleUnit *unit, int tuMax);
	/// get _totalTUCost; find out whether we can hike somewhere in this turn or not
	int getTotalTUCost() const { return _totalTUCost; }
	bool isPathPreviewed() const;
};

}

#endif
