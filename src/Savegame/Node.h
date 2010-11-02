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
#ifndef OPENXCOM_NODE_H
#define OPENXCOM_NODE_H

class NodeLink;

/**
 * Represents a node/spawnpoint in the battlescape.
 */
class Node
{
private:
	int _id, _x, _y, _z;
	int _segment;
	NodeLink *_nodeLinks[5];
	int _type;
	int _rank;
	int _flags;
	int _reserved;
	int _priority;
public:
	/// Creates a Node.
	Node();
	/// Cleans up the Node.
	~Node();
	/// Gets the Node's ID.
	int getId();
	/// Sets the Node's ID.
	void setId(int id);
	/// get the node's paths
	//NodeLink getNodeLink(int i);
};

#endif
