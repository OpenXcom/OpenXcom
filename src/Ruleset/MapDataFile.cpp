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
#include "MapDataFile.h"
#include <fstream>
#include <sstream>


/**
* MapDataFile construction
*/
MapDataFile::MapDataFile(std::string name, int size):_name(name), _size(size)
{
}


/**
* MapDataFile DESTRUCTION !
*/
MapDataFile::~MapDataFile()
{
}

/**
* gets the MapDataFile name (string)
* @return name
*/
std::string MapDataFile::getName()
{
	return _name;
}

/**
* gets the MapDataFile width
* @return size in number of records
*/
int MapDataFile::getSize()
{
	return _size;
}

