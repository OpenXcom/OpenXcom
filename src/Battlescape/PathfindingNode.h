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
#ifndef OPENXCOM_PATHFINDINGNODE_H
#define OPENXCOM_PATHFINDINGNODE_H

#include "Position.h"

namespace OpenXcom
{

/**
 * A class that holds pathfinding info for a certain node on the map.
 */
class PathfindingNode
{
private:
	Position _pos;
	bool _checked;
	int _tuCost, _stepsNum;
	PathfindingNode* _prevNode;
	int _prevDir;
public:
	/// Creates a new PathfindingNode class
	PathfindingNode(Position pos);
	/// Cleans up the PathfindingNode.
	~PathfindingNode();
	/// Get the node position
	const Position &getPosition() const;
	/// Reset node.
	void reset();
	/// Check node.
	void check(int tuCost, int stepsNum, PathfindingNode* prevNode, int prevDir);
	/// is checked?
	bool isChecked() const;
	/// get TU cost
	int getTUCost() const;
	/// get steps num
	int getStepsNum() const;
	/// get previous node
	PathfindingNode* getPrevNode() const;
	/// get previous walking direction
	int getPrevDir() const;
};

}

#endif
