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
#include "NodeLink.h"

namespace OpenXcom
{

/**
 * Initializes a NodeLink.
 * @param connectedNodeID
 * @param distance
 * @param travelType
 */
NodeLink::NodeLink(int connectedNodeID, int distance, int travelType): _connectedNodeID(connectedNodeID), _distance(distance), _travelType(travelType)
{
}

/**
 *
 */
NodeLink::~NodeLink()
{
}

int NodeLink::getConnectedNodeID() const
{
	return _connectedNodeID;
}

void NodeLink::setConnectedNodeID(int id)
{
	_connectedNodeID = id;
}

}
