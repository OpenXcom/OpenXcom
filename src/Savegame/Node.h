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
#include "../Battlescape/Position.h"
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

enum NodeRank{NR_SCOUT=0, NR_XCOM, NR_SOLDIER, NR_NAVIGATOR, NR_LEADER, NR_ENGINEER, NR_MISC1, NR_MEDIC, NR_MISC2};

/**
 * Represents a node/spawnpoint in the battlescape, loaded from RMP files.
 * @sa http://www.ufopaedia.org/index.php?title=ROUTES
 */
class Node
{
private:
	int _id;
	Position _pos;
	int _segment;
	std::vector<int> _nodeLinks;
	int _type;
	int _rank;
	int _flags;
	int _reserved;
	int _priority;
	bool _allocated;
	bool _dummy;
public:
	static const int CRAFTSEGMENT = 1000;
	static const int UFOSEGMENT = 2000;
	static const int TYPE_FLYING = 0x01; // non-flying unit can not spawn here when this bit is set
	static const int TYPE_SMALL = 0x02; // large unit can not spawn here when this bit is set
	static const int TYPE_DANGEROUS = 0x04; // an alien was shot here, stop patrolling to it like an idiot with a death wish
	static const int nodeRank[8][7]; // maps alien ranks to node (.RMP) ranks
	/// Creates a Node.
	Node();
	Node(int id, Position pos, int segment, int type, int rank, int flags, int reserved, int priority);
	/// Cleans up the Node.
	~Node();
	/// Loads the node from YAML.
	void load(const YAML::Node& node);
	/// Saves the node to YAML.
	YAML::Node save() const;
	/// get the node's id
	int getID() const;
	/// get the node's paths
	std::vector<int> *getNodeLinks();
	/// Gets node's rank.
	NodeRank getRank() const;
	/// Gets node's priority.
	int getPriority() const;
	/// Gets the node's position.
	Position getPosition() const;
	/// Gets the node's segment.
	int getSegment() const;
	/// Gets the node's type.
	int getType() const;
	/// Sets the node's type, surprisingly
	void setType(int type);
	/// gets "flags" variable, which is really the patrolling desirability value
	int getFlags() const { return _flags; }
	/// compares the _flags variables of the nodes (for the purpose of patrol decisions!)
	bool operator<(Node &b) const { return _flags < b.getFlags(); };
	bool isAllocated() const;
	void allocateNode();
	void freeNode();
	bool isTarget() const;
	void setDummy(bool dummy);
	bool isDummy() const;

};

}
