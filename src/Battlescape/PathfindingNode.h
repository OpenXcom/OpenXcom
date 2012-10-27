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
#ifndef OPENXCOM_PATHFINDINGNODE_H
#define OPENXCOM_PATHFINDINGNODE_H

#include "Position.h"

namespace OpenXcom
{

class PathfindingOpenSet;
class OpenSetEntry;

/**
 * A class that holds pathfinding info for a certain node on the map.
 */
class PathfindingNode
{
private:
	Position _pos;
	bool _checked;
	int _tuCost;
	PathfindingNode* _prevNode;
	int _prevDir;
	/// Approximate cost to reach goal position.
	int _tuGuess;
	// Invasive field needed by PathfindingOpenSet
	OpenSetEntry *_openentry;
	friend class PathfindingOpenSet;
public:
	/// Creates a new PathfindingNode class
	PathfindingNode(Position pos);
	/// Cleans up the PathfindingNode.
	~PathfindingNode();
	/// Get the node position
	const Position &getPosition() const;
	/// Reset node.
	void reset();
	/// is checked?
	bool isChecked() const;
	/// Mark as checked
	void setChecked() { _checked = true; }
	/// get TU cost
	int getTUCost() const;
	/// get steps num
	int getStepsNum() const;
	/// get previous node
	PathfindingNode* getPrevNode() const;
	/// get previous walking direction
	int getPrevDir() const;
	/// Is this node already in a PathfindingOpenSet?
	bool inOpenSet() const { return _openentry; }
	/// Get approximate cost to reach target position.
	int getTUGuess() const { return _tuGuess; }
	/// Connect to previous node along the path.
	void connect(int tuCost, PathfindingNode* prevNode, int prevDir, const Position &target);
	/// Connect to previous node along a visit.
	void connect(int tuCost, PathfindingNode* prevNode, int prevDir);
};

/** Compare PathfindingNode pointers based on TU cost. */
class MinNodeCosts
{
public:
	bool operator()(const PathfindingNode *a, const PathfindingNode *b) const
	{
		return a->getTUCost() < b->getTUCost();
	}
};

}

#endif
