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
#ifndef OPENXCOM_RULETERRAIN_H
#define OPENXCOM_RULETERRAIN_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include "MapBlock.h"

namespace OpenXcom
{

class MapBlock;
class MapDataSet;
class MapData;
class Ruleset;

/**
 * Represents a specific type of Battlescape Terrain.
 * - the names of the objectsets needed in this specific terrain
 * - the mapblocks that can be used to build this terrain
 * @sa http://www.ufopaedia.org/index.php?title=TERRAIN
 */
class RuleTerrain
{
private:
	std::vector<MapDataSet*> _mapDataSets;
	std::vector<MapBlock*> _mapBlocks;
	std::string _name;
	int _largeBlockLimit;
public:
	RuleTerrain(const std::string &name);
	~RuleTerrain();
	/// Loads the terrain from YAML.
	void load(const YAML::Node& node, Ruleset *ruleset);
	/// Saves the terrain to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the terrain's name (used for MAP generation).
	std::string getName() const;
	/// Gets the terrain's mapblocks.
	std::vector<MapBlock*> *getMapBlocks();
	/// Gets the terrain's mapdatafiles.
	std::vector<MapDataSet*> *getMapDataSets();
	/// Gets a random mapblock.
	MapBlock *getRandomMapBlock(int maxsize, MapBlockType type, bool force = false);
	/// Get a mapblock given it's name.
	MapBlock* getMapBlock(const std::string &name);
	/// Get the mapdata object.
	MapData *getMapData(int *id, int *mapDataSetID) const;
	int getLargeBlockLimit() const;
	void resetMapBlocks();
};

}

#endif
