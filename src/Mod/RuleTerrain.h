#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include "MapBlock.h"

namespace OpenXcom
{

class MapBlock;
class MapDataSet;
class MapData;
class Mod;

/**
 * Represents a specific type of Battlescape Terrain.
 * - the names of the objectsets needed in this specific terrain.
 * - the mapblocks that can be used to build this terrain.
 * @sa http://www.ufopaedia.org/index.php?title=TERRAIN
 */
class RuleTerrain
{
private:
	std::vector<MapDataSet*> _mapDataSets;
	std::vector<MapBlock*> _mapBlocks;
	std::string _name, _script;
	std::vector<std::string> _civilianTypes, _music;
	int _minDepth, _maxDepth, _ambience;
	double _ambientVolume;
public:
	RuleTerrain(const std::string &name);
	~RuleTerrain();
	/// Loads the terrain from YAML.
	void load(const YAML::Node& node, Mod *mod);
	/// Gets the terrain's name (used for MAP generation).
	std::string getName() const;
	/// Gets the terrain's mapblocks.
	std::vector<MapBlock*> *getMapBlocks();
	/// Gets the terrain's mapdatafiles.
	std::vector<MapDataSet*> *getMapDataSets();
	/// Gets a random mapblock.
	MapBlock *getRandomMapBlock(int maxSizeX, int maxSizeY, int group, bool force = true);
	/// Gets a mapblock given its name.
	MapBlock *getMapBlock(const std::string &name);
	/// Gets the mapdata object.
	MapData *getMapData(unsigned int *id, int *mapDataSetID) const;
	/// Gets the civilian types to use.
	std::vector<std::string> getCivilianTypes() const;
	/// Gets the minimum depth.
	int getMinDepth() const;
	/// Gets the maximum depth.
	 int getMaxDepth() const;
	/// Gets the ambient sound effect.
	int getAmbience() const;
	/// Gets the generation script name.
	std::string getScript() const;
	/// Gets the list of music to pick from.
	const std::vector<std::string> &getMusic() const;
	double getAmbientVolume() const;
};

}
