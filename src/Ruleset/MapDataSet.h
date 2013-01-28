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
 * Which corresponds to an Xcom MCD & PCK file.
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
	/// Saves the map data set to YAML.
	void save(YAML::Emitter& out) const;
	/// Loads voxeldata from a DAT file.
	static void loadLOFTEMPS(const std::string &filename, std::vector<Uint16> *voxelData);
	/// Get the dataset name (used for MAP generation).
	std::string getName() const;
	/// Get the dataset size.
	int getSize() const;
	/// Get objects in this dataset.
	std::vector<MapData*> *getObjects();
	/// Get surfaces in this dataset.
	SurfaceSet *getSurfaceset() const;
	/// Load the objects from an MCD file.
	void loadData();
	///	Unload to free memory.
	void unloadData();
	///
	static MapData *getBlankFloorTile();
	static MapData *getScorchedEarthTile();
};

}

#endif
