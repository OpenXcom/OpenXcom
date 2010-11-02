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

class Surface;
class TerrainObject;

/**
 * basic element of which a battle map is build
 */
class Tile
{
protected:
	int _terrainObjectID[4]; 
	TerrainObject *_terrainObjects[4];
	bool _visible; // is this tile seen by the XCom team
	int _light; // the amount of light on this tile
	std::string _mapDataFileName[4];

public:
	/// Creates a tile.
	Tile();
	/// Cleans up a tile.
	~Tile();
	/// Gets a pointer to the terrainobject for a specific part of the tile.
	TerrainObject *getTerrainObject(int part);
	/// Gets an ID of the terrainobject for a specific part of the tile.
	int getTerrainObjectID(int part);
	/// Sets the pointer to the terrainobject for a specific part of the tile
	void setTerrainObject(TerrainObject *tob, int part);
	/// Sets the object ID for specific part of the tile
	void setTerrainObjectID(int objectID, int part);
	/// Gets the map data filename for the part
	std::string getMapDataFileName(int part);
	/// sets the map data filename for the part
	void setMapDataFileName(std::string name, int part);
};

#endif
