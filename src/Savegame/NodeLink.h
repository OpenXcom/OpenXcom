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
#ifndef OPENXCOM_NODELINK_H
#define OPENXCOM_NODELINK_H

namespace OpenXcom
{

/**
 * Represents a link to another node on the map.
 */
class NodeLink
{
private:
	int _connectedNodeID;
	int _distance;
	int _travelType;
public:
	/// Creates a nodelink.
	NodeLink(int connectedNodeID, int distance, int travelType);
	/// Cleans up the nodelink.
	~NodeLink();
	/// Get the connected node id
	int getConnectedNodeID() const;
	void setConnectedNodeID(int id);
};

}

#endif
