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
	MovementType _movementType;
	/// Gets the node at certain position.
	PathfindingNode *getNode(const Position& pos);
	/// Determines whether a tile blocks a certain movementType.
	bool isBlocked(Tile *tile, const int part, BattleUnit *missileTarget, int bigWallExclusion = -1);
	/// Tries to find a straight line path between two positions.
	bool bresenhamPath(const Position& origin, const Position& target, BattleUnit *missileTarget, bool sneak = false, int maxTUCost = 1000);
	/// Tries to find a path between two positions.
	bool aStarPath(const Position& origin, const Position& target, BattleUnit *missileTarget, bool sneak = false, int maxTUCost = 1000);
	/// Determines whether a unit can fall down from this tile.
	bool canFallDown(Tile *destinationTile);
	/// Determines whether a unit can fall down from this tile.
	bool canFallDown(Tile *destinationTile, int size);
	/// Determines the additional TU cost of going one step from start to destination if going through a closed UFO door.
	int getOpeningUfoDoorCost(int direction, Position start, Position destination);
	BattleUnit *_unit;
	bool _pathPreviewed;
	bool _strafeMove;
	int _totalTUCost;
	Tile *getTile(const Position& origin, int direction) const;
public:
	enum directions {DIR_HN, DIR_HNE, DIR_HE, DIR_HSE, DIR_HS, DIR_HSW, DIR_HW, DIR_HNW,
		DIR_UP, DIR_UN, DIR_UNE, DIR_UE, DIR_USE, DIR_US, DIR_USW, DIR_UW, DIR_UNW,
		DIR_DOWN, DIR_DN, DIR_DNE, DIR_DE, DIR_DSE, DIR_DS, DIR_DSW, DIR_DW, DIR_DNW, DIR_TOTAL};
	static const int DIR_CARD_STEP = 2;
	std::vector<int> _path;
	/// Determines whether the unit is going up a stairs.
	bool isOnStairs(const Position &startPosition, const Position &endPosition);
	/// Determines whether or not movement between starttile and endtile is possible in the direction.
	bool isBlocked(Tile *startTile, Tile *endTile, const int direction, BattleUnit *missileTarget);
	enum bigWallTypes{ BLOCK = 1, BIGWALLNESW, BIGWALLNWSE, BIGWALLWEST, BIGWALLNORTH, BIGWALLEAST, BIGWALLSOUTH, BIGWALLEASTANDSOUTH};
	static const int O_BIGWALL = -1;
	/// Creates a new Pathfinding class.
	Pathfinding(SavedBattleGame *save);
	/// Cleans up the Pathfinding.
	~Pathfinding();
	/// Calculates the shortest path.
	void calculate(BattleUnit *unit, Position endPosition, BattleUnit *missileTarget = 0, int maxTUCost = 1000);
	/// Converts direction to a vector.
	static void directionToVector(int direction, Position *vector);
	/// Converts direction to a vector - horizontal component only.
	static void directionToVectorH(int direction, Position *vector);
	/// Converts a 3d direction into horizontal direction
	static int horizontalDirection(int direction);
	/// handles turning
	static void turnRight(int &direction);
	static void turnLeft(int &direction);
	static void turn(int &direction, int toDirection);
	/// Converts a vector to a direction.
	static void vectorToDirection(const Position &vector, int &dir);
	/// Checks whether a path is ready and gives the first direction.
	int getStartDirection();
	/// Dequeues a direction.
	int dequeuePath();
	/// Gets the TU cost to move from 1 tile to the other.
	int getTUCost(const Position &startPosition, const int direction, Position *endPosition, BattleUnit *unit, BattleUnit *target, bool missile);
	/// Aborts the current path.
	void abortPath();
	/// Gets the strafe move setting.
	bool getStrafeMove() const;
	/// Checks, for the up/down button, if the movement is valid.
	int costUpDown(BattleUnit *bu, Position startPosition, const int direction);
	/// Previews the path.
	bool previewPath(bool bRemove = false);
	/// Removes the path preview.
	bool removePreview();
	/// Sets _unit in order to abuse low-level pathfinding functions from outside the class.
	void setUnit(BattleUnit *unit);
	/// Gets all reachable tiles, based on cost.
	std::vector<int> findReachable(BattleUnit *unit, int tuMax);
	/// Gets _totalTUCost; finds out whether we can hike somewhere in this turn or not.
	int getTotalTUCost() const { return _totalTUCost; }
	/// Gets the path preview setting.
	bool isPathPreviewed() const;
};

}

#endif
