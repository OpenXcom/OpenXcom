/*
 * Copyright 2010 OpenXcom Developers
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
#ifndef OPENXCOM_SPAWNPOINT_H
#define OPENXCOM_SPAWNPOINT_H


class Spawnpoint;

struct SpawnPointConnection
{
	int _connectedNodeID;
	int _distance;
	int _travelType;
	Spawnpoint *_connectedNode;
};

/**
 * Represents a spawnpoint in the battlescape.
 */
class Spawnpoint
{
private:
	int _id,_x,_y,_z;
	int _segment;
	SpawnPointConnection _connections[5];
	int _type;
	int _rank;
	int _flags;
	int _reserved;
	int _priority;
public:
	/// Creates a spawnpoint.
	Spawnpoint();
	/// Cleans up the spawnpoint.
	~Spawnpoint();
	/// Gets the spawnpoint's ID.
	int getId();
	/// Sets the spawnpoint's ID.
	void setId(int id);
};

#endif
