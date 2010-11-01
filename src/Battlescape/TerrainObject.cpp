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
#include "TerrainObject.h"
#include <fstream>
#include <sstream>


/**
*
*/
TerrainObject::TerrainObject(MCD* data): _data(data)
{
	
}


/**
*
*/
TerrainObject::~TerrainObject()
{
	delete _data;
}

/**
* set the sprite offset
*/
void TerrainObject::setSpriteOffset(int offset)
{
	for (int j=0;j<8;j++)
		_sprites[j] = _data->Frame[j] + offset;

}
/**
* get the sprite frame
*/
int TerrainObject::getSpriteID(int frameID)
{
	return _sprites[frameID];
}

/// whether this is an animated ufo door
bool TerrainObject::isUFODoor()
{
	return _data->UFO_Door == 1;
}

/// gets the P-Level for drawing
int TerrainObject::getPLevel()
{
	return (int)_data->P_Level;
}