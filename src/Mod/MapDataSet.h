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
#ifndef OPENXCOM_MAPDATASET_H
#define OPENXCOM_MAPDATASET_H

#include <string>
#include <vector>
#include <SDL.h>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class MapData;
class SurfaceSet;
class ResourcePack;

/**
 * Represents a Terrain Map Datafile.
 * Which corresponds to an XCom MCD & PCK file.
 * The list of map datafiles is stored in RuleSet, but referenced in RuleTerrain.
 * @sa http://www.ufopaedia.org/index.php?title=MCD
 */
class MapDataSet
{
private:
	std::string _name;
	std::vector<MapData*> _objects;
	SurfaceSet *_surfaceSet;
	bool _loaded;
	static MapData *_blankTile;
	static MapData *_scorchedTile;
public:
	MapDataSet(const std::string &name);
	~MapDataSet();
	/// Loads the map data set from YAML.
	void load(const YAML::Node& node);
	/// Loads voxeldata from a DAT file.
	static void loadLOFTEMPS(const std::string &filename, std::vector<Uint16> *voxelData);
	/// Gets the dataset name (used for MAP generation).
	std::string getName() const;
	/// Gets the dataset size.
	size_t getSize() const;
	/// Gets the objects in this dataset.
	std::vector<MapData*> *getObjects();
	/// Gets the surfaces in this dataset.
	SurfaceSet *getSurfaceset() const;
	/// Loads the objects from an MCD file.
	void loadData();
	///	Unloads to free memory.
	void unloadData();
	/// Gets a blank floor tile.
	static MapData *getBlankFloorTile();
	/// Gets a scorched earth tile.
	static MapData *getScorchedEarthTile();
};

}

#endif
