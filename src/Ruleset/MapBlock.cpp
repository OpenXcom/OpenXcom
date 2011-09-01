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
#include "MapBlock.h"

namespace OpenXcom
{

/**
* MapBlock construction
*/
MapBlock::MapBlock(RuleTerrain *terrain, std::string name, int width, int length, bool landingZone):_terrain(terrain), _name(name), _width(width), _length(length), _height(0), _landingZone(landingZone)
{
}

/**
* MapBlock DESTRUCTION.
*/
MapBlock::~MapBlock()
{
}

/**
* Gets the MapBlock name (string).
* @return name
*/
std::string MapBlock::getName() const
{
	return _name;
}

/**
* Gets the MapBlock width.
* @return width in tiles.
*/
int MapBlock::getWidth() const
{
	return _width;
}

/**
* Gets the MapBlock length.
* @return length in tiles.
*/
int MapBlock::getLength() const
{
	return _length;
}

/**
* Sets the MapBlock height.
* @param height
*/
void MapBlock::setHeight(int height)
{
	_height = height;
}

/**
* Gets the MapBlock height.
* @return height
*/
int MapBlock::getHeight() const
{
	return _height;
}

/**
* Is this mapblock usable as a landingzone.
* @return bool whether usable as landingzone.
*/
bool MapBlock::isLandingZone() const
{
	return _landingZone;
}

}
