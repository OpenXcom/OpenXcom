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
#include "Node.h"
#include "NodeLink.h"

namespace OpenXcom
{


Node::Node() : _id(0), _segment(0), _type(0), _rank(0), _flags(0), _reserved(0), _priority(0), _allocated(false)
{

}

/**
 * Initializes a Node.
 * @param id
 * @param pos
 * @param segment
 * @param type
 * @param rank
 * @param flags
 * @param reserved
 * @param priority
 */
Node::Node(int id, Position pos, int segment, int type, int rank, int flags, int reserved, int priority) : _id(id), _pos(pos), _segment(segment), _type(type), _rank(rank), _flags(flags), _reserved(reserved), _priority(priority), _allocated(false)
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
 * Loads the UFO from a YAML file.
 * @param node YAML node.
 */
void Node::load(const YAML::Node &node)
{
	int a = 0;
	node["id"] >> _id;
	node["position"] >> _pos;
	node["segment"] >> _segment;
	node["type"] >> _type;
	node["rank"] >> _rank;
	node["flags"] >> _flags;
	node["reserved"] >> _reserved;
	node["priority"] >> _priority;
	node["allocated"] >> _allocated;
	for (int i=0; i < 5; i++)
	{
		node["links"][i]["connectedNodeId"] >> a;
		_nodeLinks[i] = new NodeLink(a, 0, 0);
		_nodeLinks[i]->load(node["links"][i]);
	}
}

/**
 * Saves the UFO to a YAML file.
 * @param out YAML emitter.
 */
void Node::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "position" << YAML::Value << _pos;
	out << YAML::Key << "segment" << YAML::Value << _segment;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "rank" << YAML::Value << _rank;
	out << YAML::Key << "flags" << YAML::Value << _flags;
	out << YAML::Key << "reserved" << YAML::Value << _reserved;
	out << YAML::Key << "priority" << YAML::Value << _priority;
	out << YAML::Key << "allocated" << YAML::Value << _allocated;
	out << YAML::Key << "links" << YAML::Value;
	out << YAML::BeginSeq;
	for (int i=0; i < 5; i++)
	{
		if (_nodeLinks[i])
		{
			_nodeLinks[i]->save(out);
		}
		else
		{
			out << YAML::BeginMap;
			out << YAML::Key << "connectedNodeId" << YAML::Value << -1;
			out << YAML::EndMap;
		}
	}
	out << YAML::EndSeq;
	out << YAML::EndMap;
}

/**
 * Get the node's id
 * @return unique id
 */
int Node::getID() const
{
	return _id;
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

/**
 * Gets the Node's segment.
 * @return segment
 */
int Node::getSegment() const
{
	return _segment;
}

/**
 * Gets the Node's nodelink.
 * @param index 0-4
 * @return nodelink
 */
NodeLink *Node::getNodeLink(int index)
{
	return _nodeLinks[index];
}

/**
 * Gets the Node's type.
 * @return type
 */
int Node::getType() const
{
	return _type;
}

bool Node::isAllocated() const
{
	return _allocated;
}

void Node::allocate()
{
	_allocated = true;
}

void Node::free()
{
	_allocated = false;
}

bool Node::isTarget() const
{
	return _reserved == 5;
}

}
