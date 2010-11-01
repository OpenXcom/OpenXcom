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
#include <fstream>
#include "Tile.h"

/**
*
*/
Tile::Tile(): _visible(true),_light(255)
{
	_TerrainObjectID[0] = 0;
	_TerrainObjectID[1] = 0;
	_TerrainObjectID[2] = 0;
	_TerrainObjectID[3] = 0;
}

/**
 * 
 */
Tile::~Tile()
{

}

/**
 * Get the TerrainObjectID reference of a part of the tile.
 * @param part the part 0-3.
 */
int Tile::getTerrainObjectID(int part)
{
	if (part<0 || part>3)
		throw "unkown TerrainObjectID part";
	return _TerrainObjectID[part];
}

/**
 * Set the TerrainObjectID references of part 0 to 3
 * @param TerrainObjectID an array of 4 int's that hold the references
 */
void Tile::setTerrainObjectID(int TerrainObjectID[])
{
	_TerrainObjectID[0] = TerrainObjectID[0];
	_TerrainObjectID[1] = TerrainObjectID[1];
	_TerrainObjectID[2] = TerrainObjectID[2];
	_TerrainObjectID[3] = TerrainObjectID[3];
}