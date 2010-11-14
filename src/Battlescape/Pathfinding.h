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
#ifndef OPENXCOM_PATHFINDING_H
#define OPENXCOM_PATHFINDING_H

#include <vector>

class Position;
class SavedBattleGame;
class PathfindingNode;

/**
 * A class that calculates the shortest path between two points on the battlescape map
 */
class Pathfinding
{
private:
	SavedBattleGame *_save;
	PathfindingNode **_nodes;
	int _size;
	std::vector<int> _path;
	/// Gets the node at certain position.
	PathfindingNode *getNode(const Position& pos);
	/// Get's the TU cost to move from 1 tile to the other.
	int getTUCost(const Position &startPosition, const int direction, Position *endPosition);
public:
	/// Creates a new Pathfinding class
	Pathfinding(SavedBattleGame *save);
	/// Cleans up the Pathfinding.
	~Pathfinding();
	/// Calculate the shortest path.
	void calculate(const Position &startPosition, Position &endPosition);
	/// Converts direction to a vector.
	static void directionToVector(const int direction, Position *vector);
	/// Check whether a path is ready and dequeue it.
	int dequeuePath();
};

#endif
