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
#include "../Battlescape/Position.h"
#include "../Resource/TerrainObject.h"

class Surface;
class TerrainObject;

/**
 * basic element of which a battle map is build
 */
class Tile
{
protected:
	std::string _name[4];
	TerrainObject *_terrainObjects[4];
	bool _visible; // is this tile seen by the XCom team
	int _light; // the amount of light on this tile
	Position _pos;

public:
	/// Creates a tile.
	Tile(const Position& pos);
	/// Cleans up a tile.
	~Tile();
	/// Gets name for the part
	std::string getName(int part);
	/// sets name for the part
	void setName(std::string name, int part);
	/// Gets a pointer to the terrainobject for a specific part of the tile.
	TerrainObject *getTerrainObject(int part);
	/// Sets the pointer to the terrainobject for a specific part of the tile
	void setTerrainObject(TerrainObject *tob, int part);
	/// Get the TU cost to walk over a certain part of the tile.
	int getTUCost(int part, MovementType movementType);
	/// Checks if this tile has a floor.
	bool hasNoFloor();
	/// Checks if this tile is a big wall.
	bool isBigWall();
	/// Get terrain level.
	int getTerrainLevel();
	/// Gets the tile's position.
	const Position& getPosition() const;

};

#endif
