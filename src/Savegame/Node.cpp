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
}

/**
 * clean up node.
 */
Node::~Node()
{
}

/**
 * Loads the UFO from a YAML file.
 * @param node YAML node.
 */
void Node::load(const YAML::Node &node)
{
	node["id"] >> _id;
	node["position"] >> _pos;
	//node["segment"] >> _segment;
	node["type"] >> _type;
	node["rank"] >> _rank;
	node["flags"] >> _flags;
	//node["reserved"] >> _reserved;
	node["priority"] >> _priority;
	node["allocated"] >> _allocated;
	node["links"] >> _nodeLinks;
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
	//out << YAML::Key << "segment" << YAML::Value << _segment;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "rank" << YAML::Value << _rank;
	out << YAML::Key << "flags" << YAML::Value << _flags;
	//out << YAML::Key << "reserved" << YAML::Value << _reserved;
	out << YAML::Key << "priority" << YAML::Value << _priority;
	out << YAML::Key << "allocated" << YAML::Value << _allocated;
	out << YAML::Key << "links" << YAML::Value << YAML::Flow << _nodeLinks;
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

/// get the node's paths
std::vector<int> *Node::getNodeLinks()
{
	return &_nodeLinks;
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

void Node::setType(int type)
{
    _flags = type;
}


}
