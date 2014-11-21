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
#ifndef OPENXCOM_MAPBLOCK_H
#define OPENXCOM_MAPBLOCK_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

enum MapBlockType {MT_UNDEFINED = -1, MT_DEFAULT, MT_LANDINGZONE, MT_EWROAD, MT_NSROAD, MT_CROSSING};
class RuleTerrain;
class Position;

/**
 * Represents a Terrain Map Block.
 * It contains constant info about this mapblock, like its name, dimensions, attributes...
 * Map blocks are stored in RuleTerrain objects.
 * @sa http://www.ufopaedia.org/index.php?title=MAPS_Terrain
 */
class MapBlock
{
private:
	std::string _name;
	int _size_x, _size_y, _size_z;
	std::vector<int> _groups;
	std::map<std::string, std::vector<Position> > _items;
public:
	MapBlock(const std::string &name);
	~MapBlock();
	/// Loads the map block from YAML.
	void load(const YAML::Node& node);
	/// Gets the mapblock's name (used for MAP generation).
	std::string getName() const;
	/// Gets the mapblock's x size.
	int getSizeX() const;
	/// Gets the mapblock's y size.
	int getSizeY() const;
	/// Gets the mapblock's z size.
	int getSizeZ() const;
	/// Sets the mapblock's z size.
	void setSizeZ(int size_z);
	/// Returns if this mapblock is from the group specified.
	bool isInGroup(int group);
	/// Gets the layout for any items that belong in this map block.
	std::map<std::string, std::vector<Position> > *getItems();

};

}

#endif
