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
#include "Node.h"
#include "NodeLink.h"

namespace OpenXcom
{

/**
 * Initializes a Node.
 * @param id
 * @param x
 * @param y
 * @param z
 * @param segment
 * @param type
 * @param rank
 * @param flags
 * @param reserved
 * @param priority
 */
Node::Node(int id, Position pos, int segment, int type, int rank, int flags, int reserved, int priority) : _id(id), _pos(pos), _segment(segment), _type(type), _rank(rank), _flags(flags), _reserved(reserved), _priority(priority)
{
	_nodeLinks[0] = 0;
	_nodeLinks[1] = 0;
	_nodeLinks[2] = 0;
	_nodeLinks[3] = 0;
	_nodeLinks[4] = 0;
}

/**
 * clean up node.
 */
Node::~Node()
{
	delete _nodeLinks[0];
	delete _nodeLinks[1];
	delete _nodeLinks[2];
	delete _nodeLinks[3];
	delete _nodeLinks[4];
}

/**
 * Assign a node link to this node.
 * @param link pointer to the link
 * @param index 0-4
 */
void Node::assignNodeLink(NodeLink *link, int index)
{
	_nodeLinks[index] = link;
}

/**
 * Get the rank of units that can spawn on this node.
 * @return noderank
 */
NodeRank Node::getRank() const
{
	return (NodeRank)_rank;
}

/**
 * Get the priority of this spawnpoint.
 * @return priority
 */
int Node::getPriority() const
{
	return _priority;
}

/**
 * Gets the Node's position.
 * @return position
 */
const Position& Node::getPosition() const
{
	return _pos;
}

}
