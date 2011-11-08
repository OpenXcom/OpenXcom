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
#include "PathfindingNode.h"
#include "Position.h"

namespace OpenXcom
{

/**
 * Sets up a PathfindingNode.
 * @param pos Position.
 */
PathfindingNode::PathfindingNode(Position pos) : _pos(pos)
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
}
/**
* Check node. The pathfinding marks every node as checked, storing some additional info.
* @param tuCost
* @param stepsNum
* @param prevNode
* @param prevDir
*/
void PathfindingNode::check(int tuCost, int stepsNum, PathfindingNode* prevNode, int prevDir)
{
	_checked = true;
	_tuCost = tuCost;
	_stepsNum = stepsNum;
	_prevNode = prevNode;
	_prevDir = prevDir;
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
 * Get steps num
 * @return steps num
 */
int PathfindingNode::getStepsNum() const
{
	return _stepsNum;
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

}
