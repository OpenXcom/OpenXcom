/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include <sstream>
#include "../Engine/Exception.h"

namespace OpenXcom
{

/**
 * MapBlock construction.
 */
MapBlock::MapBlock(const std::string &name):_name(name), _size_x(10), _size_y(10), _size_z(4)
{
	_groups.push_back(0);
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
	if ((_size_x % 10) != 0 || (_size_y % 10) != 0)
	{
		std::stringstream ss;
		ss << "Error: MapBlock " << _name << ": Size must be divisible by ten";
		throw Exception(ss.str());
	}
	if (const YAML::Node &map = node["groups"])
	{
		_groups.clear();
		if (map.Type() == YAML::NodeType::Sequence)
		{
			_groups = map.as<std::vector<int> >(_groups);
		}
		else
		{
			_groups.push_back(map.as<int>(0));
		}
	}
	if (const YAML::Node &map = node["revealedFloors"])
	{
		_revealedFloors.clear();
		if (map.Type() == YAML::NodeType::Sequence)
		{
			_revealedFloors = map.as<std::vector<int> >(_revealedFloors);
		}
		else
		{
			_revealedFloors.push_back(map.as<int>(0));
		}
	}
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
bool MapBlock::isInGroup(int group)
{
	return std::find(_groups.begin(), _groups.end(), group) != _groups.end();
}

/**
 * Gets if this floor should be revealed or not.
 */
bool MapBlock::isFloorRevealed(int floor)
{
	return std::find(_revealedFloors.begin(), _revealedFloors.end(), floor) != _revealedFloors.end();
}

/**
 * Gets the items and their positioning for any items associated with this block.
 * @return the items and their positions.
 */
std::map<std::string, std::vector<Position> > *MapBlock::getItems()
{
	return &_items;
}

}
