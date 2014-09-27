/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "../Battlescape/Position.h"

namespace OpenXcom
{

/**
 * MapBlock construction.
 */
MapBlock::MapBlock(const std::string &name, int size_x, int size_y, MapBlockType type):_name(name), _size_x(size_x), _size_y(size_y), _size_z(0), _type(type), _subType(MT_UNDEFINED), _frequency(1), _timesUsed(0), _maxCount(-1)
{
}

/**
 * MapBlock desctruction.
 */
MapBlock::~MapBlock()
{
}

/**
 * Loads the map block from a YAML file.
 * @param node YAML node.
 */
void MapBlock::load(const YAML::Node &node)
{
	_name = node["name"].as<std::string>(_name);
	_size_x = node["width"].as<int>(_size_x);
	_size_y = node["length"].as<int>(_size_y);
	_size_z = node["height"].as<int>(_size_z);
	_type = (MapBlockType)node["type"].as<int>(_type);
	if (_subType == MT_UNDEFINED)
		_subType = (MapBlockType)node["type"].as<int>(_type);
	_subType = (MapBlockType)node["subType"].as<int>(_subType);
	_frequency = node["frequency"].as<int>(_frequency);
	_maxCount = node["maxCount"].as<int>(_maxCount);
	_items = node["items"].as<std::map<std::string, std::vector<Position> > >(_items);
}

/**
 * Gets the MapBlock name (string).
 * @return The name.
 */
std::string MapBlock::getName() const
{
	return _name;
}

/**
 * Gets the MapBlock size x.
 * @return The size x in tiles.
 */
int MapBlock::getSizeX() const
{
	return _size_x;
}

/**
 * Gets the MapBlock size y.
 * @return The size y in tiles.
 */
int MapBlock::getSizeY() const
{
	return _size_y;
}

/**
 * Sets the MapBlock size z.
 * @param size_z The size z.
 */
void MapBlock::setSizeZ(int size_z)
{
	_size_z = size_z;
}

/**
 * Gets the MapBlock size z.
 * @return The size z.
 */
int MapBlock::getSizeZ() const
{
	return _size_z;
}

/**
 * Gets the type of mapblock.
 * @return The mapblock's type.
 */
MapBlockType MapBlock::getType() const
{
	return _type;
}

/**
 * Gets the secondary type of the mapblock, if the primary type is occupied.
 * @return The mapblock's secondary type.
 */
MapBlockType MapBlock::getSubType() const
{
	return _subType;
}

/**
 * Gets either the remaining uses of the mapblock OR THE FREQUENCY!
 * Remaining limits the number of times a mapblock occurs.
 * Frequency increases the odds of a mapblock occuring.
 * @return int
 */
int MapBlock::getRemainingUses()
{
	if (_maxCount == -1)
	{
		return _frequency;
	}
	return _maxCount - _timesUsed;
}

/**
 * Decreases the remaining uses of a mapblock for this session.
 */
void MapBlock::markUsed()
{
	if (_maxCount == -1)
	{
		return;
	}
	_timesUsed++;
	if (_timesUsed >= _maxCount)
	{
		_timesUsed = _maxCount;
	}
}

/**
 * Resets the remaining uses of a mapblock for this session.
 */
void MapBlock::reset()
{
	_timesUsed = 0;
}
std::map<std::string, std::vector<Position> > *MapBlock::getItems()
{
	return &_items;
}

}
