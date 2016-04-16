#pragma once
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
#include <vector>
#include "Position.h"
#include "PathfindingNode.h"
#include "../Mod/MapData.h"

namespace OpenXcom
{

class SavedBattleGame;
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
	BattleUnit *_unit;
	bool _pathPreviewed;
	bool _strafeMove;
	int _totalTUCost;
	bool _modifierUsed;
	MovementType _movementType;
	/// Gets the node at certain position.
	PathfindingNode *getNode(const Position& pos);
	/// Determines whether a tile blocks a certain movementType.
	bool isBlocked(Tile *tile, int part, BattleUnit *missileTarget, int bigWallExclusion = -1);
	/// Tries to find a straight line path between two positions.
	bool bresenhamPath(const Position& origin, const Position& target, BattleUnit *missileTarget, bool sneak = false, int maxTUCost = 1000);
	/// Tries to find a path between two positions.
	bool aStarPath(const Position& origin, const Position& target, BattleUnit *missileTarget, bool sneak = false, int maxTUCost = 1000);
	/// Determines whether a unit can fall down from this tile.
	bool canFallDown(Tile *destinationTile);
	/// Determines whether a unit can fall down from this tile.
	bool canFallDown(Tile *destinationTile, int size);
	/// get the tile pointed by the direction from origin
	Tile *getTile(const Position& origin, int direction) const;
	std::vector<int> _path;
public:
	/// symbolic name for directions on all 3 adjacent planes Horizontal, Up, Down and the from North
	/// towards East trhrough South till NorthWest
	enum directions {
		DIR_HN, DIR_HNE, DIR_HE, DIR_HSE, DIR_HS, DIR_HSW, DIR_HW, DIR_HNW,
		DIR_UP, DIR_UN, DIR_UNE, DIR_UE, DIR_USE, DIR_US, DIR_USW, DIR_UW, DIR_UNW,
		DIR_DOWN, DIR_DN, DIR_DNE, DIR_DE, DIR_DSE, DIR_DS, DIR_DSW, DIR_DW, DIR_DNW, DIR_TOTAL
	};
	/// "steps" to be added to cycle cardinal directions
	static const int DIR_CARD_STEP = 2;
	/// steps to perform a full horizontal turn
	static const int DIR_FULL_HTURN = 4;
	/// get the next direction from th table above, checking if 3dFlight is enabled
	static int nextDirection(int direction);
	/// Determines whether the unit is going up a stairs.
	bool isOnStairs(const Position &startPosition, const Position &endPosition);
	/// Determines whether or not movement between starttile and endtile is possible in the direction.
	bool isBlocked(Tile *startTile, Tile *endTile, int direction, BattleUnit *missileTarget);
	enum bigWallTypes{ BLOCK = 1, BIGWALLNESW, BIGWALLNWSE, BIGWALLWEST, BIGWALLNORTH, BIGWALLEAST, BIGWALLSOUTH, BIGWALLEASTANDSOUTH, BIGWALLWESTANDNORTH};
	static const int O_BIGWALL = -1;
	static int red;
	static int green;
	static int yellow;
	/// Creates a new Pathfinding class.
	Pathfinding(SavedBattleGame *save);
	/// Cleans up the Pathfinding.
	~Pathfinding();
	/// Calculates the shortest path.
	void calculate(BattleUnit *unit, Position endPosition, BattleUnit *missileTarget = 0, int maxTUCost = 1000);
	/// Converts direction to a vector.
	static void directionToVector(int direction, Position *vector);
	/// Converts direction to a vector (horizontal component only).
	static void directionToVectorH(int direction, Position *vector);
	/// Converts a 3d direction into it's horizontal direction projection
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
	int getTUCost(const Position &startPosition, int direction, Position *endPosition, BattleUnit *unit, BattleUnit *target, bool missile);
	/// Aborts the current path.
	void abortPath();
	/// Gets the strafe move setting.
	bool getStrafeMove() const;
	/// Checks, for a vertical or combo move, if the movement is valid (returns MAX_COST overflowing if impossible move).
	int costUpDown(BattleUnit *bu, Position startPosition, const int direction, bool missile = false);
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
	/// Gets the modifier setting.
	bool isModifierUsed() const;
	/// Gets a reference to the path.
	const std::vector<int> &getPath();
	/// Makes a copy to the path.
	std::vector<int> copyPath() const;
};

}
