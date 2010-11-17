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
#include "Tile.h"
#include "../Resource/TerrainObject.h"

/**
* constructor
*/
Tile::Tile(const Position& pos): _visible(true), _light(255), _pos(pos)
{
	_terrainObjects[0] = 0;
	_terrainObjects[1] = 0;
	_terrainObjects[2] = 0;
	_terrainObjects[3] = 0;
}

/**
 * destructor
 */
Tile::~Tile()
{

}

/**
 * Get the TerrainObject pointer of a part of the tile.
 * @param part the part 0-3.
 * @return pointer to terrainobject
 */
TerrainObject *Tile::getTerrainObject(int part)
{
	if (part < 0 || part > 3)
	{
		throw "unkown TerrainObjectID part";
	}
	return _terrainObjects[part];
}

/**
 * Set the TerrainObject references of part 0 to 3.
 * @param tob pointer to the terrain object
 * @param part the part number
 */
void Tile::setTerrainObject(TerrainObject *tob, int part)
{
	_terrainObjects[part] = tob;
}

/**
 * Get mapdatafilename.
 * @param part the part number 0-3
 * @return a string which contains the MCD filename
 */
std::string Tile::getName(int part)
{
	return _name[part];
}

/**
 * Sets mapdatafilename.
 * @param name the name of the specific MCD file
 * @param part the part number 0-3
 */
void Tile::setName(std::string name, int part)
{
	_name[part] = name;
}

/**
 * Get the TU cost to walk over a certain part of the tile.
 * @param part
 * @return TU cost
 */
int Tile::getTUCost(int part, MovementType movementType)
{
	if (_terrainObjects[part])
		return _terrainObjects[part]->getTUCost(movementType);
	else
		return 0;
}

/**
 * Whether this tile has a floor or not. If no object defined as floor, it has no floor.
 * @return bool
 */
bool Tile::hasNoFloor()
{
	if (_terrainObjects[O_FLOOR])
		return _terrainObjects[O_FLOOR]->isNoFloor();
	else
		return true;
}

/**
 * Whether this tile has a big wall.
 * @return bool
 */
bool Tile::isBigWall()
{
	if (_terrainObjects[O_OBJECT])
		return _terrainObjects[O_OBJECT]->isBigWall();
	else
		return false;
}

/**
 * If an object stand on this tile, how high is it standing.
 * @return the level in pixels
 */
int Tile::getTerrainLevel()
{
	int level = 0;

	if (_terrainObjects[O_FLOOR])
		level = _terrainObjects[O_FLOOR]->getTerrainLevel();
	if (_terrainObjects[O_OBJECT])
		level += _terrainObjects[O_OBJECT]->getTerrainLevel();

	return level;
}

/**
 * Gets the tile's position.
 * @return position
 */
const Position& Tile::getPosition() const
{
	return _pos;
}
