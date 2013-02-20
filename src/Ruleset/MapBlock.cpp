/*
 * Copyright 2010-2012 OpenXcom Developers.
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
MapBlock::MapBlock(RuleTerrain *terrain, std::string name, int width, int length, MapBlockType type):_terrain(terrain), _name(name), _width(width), _length(length), _height(0), _type(type), _subType(type), _frequency(1), _timesUsed(0), _maxCount(-1)
{
}

/**
* MapBlock DESTRUCTION.
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
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "name")
		{
			i.second() >> _name;
		}
		else if (key == "width")
		{
			i.second() >> _width;
		}
		else if (key == "length")
		{
			i.second() >> _length;
		}
		else if (key == "height")
		{
			i.second() >> _height;
		}
		else if (key == "type")
		{
			int a;
			i.second() >> a;
			_type = (MapBlockType)a;
		}
		else if (key == "subType")
		{
			int a;
			i.second() >> a;
			_subType = (MapBlockType)a;
		}
		else if (key == "frequency")
		{
			i.second() >> _frequency;
		}
		else if (key == "maxCount")
		{
			i.second() >> _maxCount;
		}
	}
}

/**
 * Saves the map block to a YAML file.
 * @param out YAML emitter.
 */
void MapBlock::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << _name;
	out << YAML::Key << "width" << YAML::Value << _width;
	out << YAML::Key << "length" << YAML::Value << _length;
	out << YAML::Key << "height" << YAML::Value << _height;
	out << YAML::Key << "type" << YAML::Value << (int)_type;
	out << YAML::Key << "subType" << YAML::Value << (int)_subType;
	out << YAML::Key << "frequency" << YAML::Value << _frequency;
	out << YAML::Key << "maxCount" << YAML::Value << _maxCount;
	out << YAML::EndMap;
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
* Get the type of mapblock.
* @return type
*/
MapBlockType MapBlock::getType() const
{
	return _type;
}

MapBlockType MapBlock::getSubType() const
{
	return _subType;
}

int MapBlock::getRemainingUses()
{
	if (_maxCount == -1)
	{
		return _frequency;
	}
	return _maxCount - _timesUsed;
}

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

}
