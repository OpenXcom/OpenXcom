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
#include "PathfindingNode.h"
#include "Position.h"
#include <math.h>

namespace OpenXcom
{

/**
 * Sets up a PathfindingNode.
 * @param pos Position.
 */
PathfindingNode::PathfindingNode(Position pos) : _pos(pos), _openentry(0)
{

}

/**
 * Deletes the PathfindingNode.
 */
PathfindingNode::~PathfindingNode()
{

}

/**
* Get the node position
* @return node position
*/
const Position &PathfindingNode::getPosition() const
{
	return _pos;
}
/**
 * Reset node.
 */
void PathfindingNode::reset()
{
	_checked = false;
	_openentry = 0;
}

/**
* Is checked?
* @return bool
*/
bool PathfindingNode::isChecked() const
{
	return _checked;
}

/**
 * Get TU cost.
 * @return cost
 */
int PathfindingNode::getTUCost() const
{
	return _tuCost;
}

/**
 * Get previous node
 * @return pointer to previous node
 */
PathfindingNode* PathfindingNode::getPrevNode() const
{
	return _prevNode;
}

/**
 * Get previous walking direction how we got on this node.
 * @return previous vector
 */
int PathfindingNode::getPrevDir() const
{
	return _prevDir;
}

/**
 * Connect node. This will connect the node to the previous node along the path to @a target
 * and update the pathfinding information.
 * @param tuCost The total cost of the path so far.
 * @param prevNode The previous node along the path.
 * @param prevDir The direction FROM the previous node.
 * @param target The target position (used to update our guess cost).
*/
void PathfindingNode::connect(int tuCost, PathfindingNode* prevNode, int prevDir, const Position &target)
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
 * Connect node. This will connect the node to the previous node along the path.
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
