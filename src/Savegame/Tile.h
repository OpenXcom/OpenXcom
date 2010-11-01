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
#ifndef OPENXCOM_TILE_H
#define OPENXCOM_TILE_H

#include <string>

/**
 * basic element of which a battle map is build
 */
class Tile
{
protected:
	int _TerrainObjectID[4]; // floor, west-wall, north-wall, object
	bool _visible; // is this tile seen by the XCom team
	int _light; // the amount of light on this tile
	std::string _frameset[4]; // holds the identifier of the frameset to draw this part of the tile
	int _framenumber[4]; // the framenumber within the frameset to draw this part of the tile

public:
	/// Creates a tile.
	Tile();
	/// Cleans up a tile.
	~Tile();
	/// Gets a reference to the map control data for a specific part of the tile.
	int getTerrainObjectID(int part);
	/// Sets the 4 terrain object references for this tile.
	void setTerrainObjectID(int TerrainObjectID[]);

};

#endif
