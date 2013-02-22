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
#ifndef OPENXCOM_MAPBLOCK_H
#define OPENXCOM_MAPBLOCK_H

#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

enum MapBlockType {MT_UNDEFINED = -1, MT_DEFAULT, MT_LANDINGZONE, MT_EWROAD, MT_NSROAD, MT_CROSSING, MT_DIRT, MT_XCOMSPAWN, MT_UBASECOMM, MT_FINALCOMM };
class RuleTerrain;

/**
 * Represents a Terrain Map Block.
 * It contains constant info about this mapblock, like it's name, dimensions, attributes...
 * Map blocks are stored in RuleTerrain objects
 * @sa http://www.ufopaedia.org/index.php?title=MAPS_Terrain
 */
class MapBlock
{
private:
	RuleTerrain *_terrain;
	std::string _name;
	int _width, _length, _height;
	MapBlockType _type, _subType;
	int _frequency, _timesUsed, _maxCount;
public:
	MapBlock(RuleTerrain *terrain, std::string name, int width, int length, MapBlockType type);
	~MapBlock();
	/// Loads the map block from YAML.
	void load(const YAML::Node& node);
	/// Saves the map block to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the mapblock's name (used for MAP generation).
	std::string getName() const;
	/// Gets the mapblock's width.
	int getWidth() const;
	/// Get the mapblock's length.
	int getLength() const;
	/// Get the mapblock's height.
	int getHeight() const;
	/// Set the mapblock's height.
	void setHeight(int height);
	/// Returns whether this mapblock is a landingzone.
	MapBlockType getType() const;
	/// Returns whether this mapblock is a landingzone.
	MapBlockType getSubType() const;
	int getRemainingUses();
	void markUsed();

};

}

#endif
