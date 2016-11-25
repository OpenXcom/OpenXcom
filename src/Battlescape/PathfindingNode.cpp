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
#include "PathfindingNode.h"
#include <cmath>

namespace OpenXcom
{

/**
 * Sets up a PathfindingNode.
 * @param pos Position.
 */
PathfindingNode::PathfindingNode(Position pos) : _pos(pos), _checked(0), _tuCost(0), _prevNode(0), _prevDir(0), _tuGuess(0), _openentry(0)
{

}

/**
 * Deletes the PathfindingNode.
 */
PathfindingNode::~PathfindingNode()
{

}

/**
 * Gets the node position.
 * @return Node position.
 */
Position PathfindingNode::getPosition() const
{
	return _pos;
}

/**
 * Resets the node.
 */
void PathfindingNode::reset()
{
	_checked = false;
	_openentry = 0;
}

/**
 * Gets the checked status of this node.
 * @return True, if this node was checked.
 */
bool PathfindingNode::isChecked() const
{
	return _checked;
}

/**
 * Gets the TU cost.
 * @param missile Is this a missile?
 * @return The TU cost.
 */
int PathfindingNode::getTUCost(bool missile) const
{
	if (missile)
		return 0;
	else
		return _tuCost;
}

/**
 * Gets the previous node.
 * @return Pointer to the previous node.
 */
PathfindingNode* PathfindingNode::getPrevNode() const
{
	return _prevNode;
}

/**
 * Gets the previous walking direction for how we got on this node.
 * @return Previous vector.
 */
int PathfindingNode::getPrevDir() const
{
	return _prevDir;
}

/**
 * Connects the node. This will connect the node to the previous node along the path to @a target
 * and update the pathfinding information.
 * @param tuCost The total cost of the path so far.
 * @param prevNode The previous node along the path.
 * @param prevDir The direction FROM the previous node.
 * @param target The target position (used to update our guess cost).
 */
void PathfindingNode::connect(int tuCost, PathfindingNode* prevNode, int prevDir, Position target)
{
	_tuCost = tuCost;
	_prevNode = prevNode;
	_prevDir = prevDir;
	if (!inOpenSet()) // Otherwise we have this already.
	{
		Position d = target - _pos;
		d *= d;
		_tuGuess = 4 * sqrt((double)d.x + d.y + d.z);
	}
}

/**
 * Connects the node. This will connect the node to the previous node along the path.
 * @param tuCost The total cost of the path so far.
 * @param prevNode The previous node along the path.
 * @param prevDir The direction FROM the previous node.
 */
void PathfindingNode::connect(int tuCost, PathfindingNode* prevNode, int prevDir)
{
	_tuCost = tuCost;
	_prevNode = prevNode;
	_prevDir = prevDir;
	_tuGuess = 0;
}


}
