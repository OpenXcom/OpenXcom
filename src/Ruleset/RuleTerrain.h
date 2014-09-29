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

struct LandingSite
{
	int x, y, sizeX, sizeY;
};
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
	std::string _name;
	int _largeBlockLimit;
	std::vector<int> _textures, _roadTypeOdds;
	std::vector<std::string> _civilianTypes;
	int _hemisphere, _minDepth, _maxDepth, _ambience;
	std::vector<LandingSite*> _ufoPositions, _craftPositions;
public:
	RuleTerrain(const std::string &name);
	~RuleTerrain();
	/// Loads the terrain from YAML.
	void load(const YAML::Node& node, Ruleset *ruleset);
	/// Gets the terrain's name (used for MAP generation).
	std::string getName() const;
	/// Gets the terrain's mapblocks.
	std::vector<MapBlock*> *getMapBlocks();
	/// Gets the terrain's mapdatafiles.
	std::vector<MapDataSet*> *getMapDataSets();
	/// Gets a random mapblock.
	MapBlock *getRandomMapBlock(int maxsize, MapBlockType type, bool force = false);
	/// Gets a mapblock given its name.
	MapBlock *getMapBlock(const std::string &name);
	/// Gets the mapdata object.
	MapData *getMapData(unsigned int *id, int *mapDataSetID) const;
	/// Gets the maximum amount of large blocks in this terrain.
	int getLargeBlockLimit() const;
	void resetMapBlocks();
	std::vector<int> *getTextures();
	int getHemisphere() const;
	/// Gets teh civilian types to use.
	std::vector<std::string> getCivilianTypes() const;
	/// Gets road type odds.
	std::vector<int> getRoadTypeOdds() const;
	/// get the minimum depth.
	const int getMinDepth() const;
	/// get the maximum depth.
	const int getMaxDepth() const;
	/// get the ambient sound effect.
	const int getAmbience() const;
	/// get a list of potential UFO landing zones for this map.
	const std::vector<LandingSite*> *getUfoPositions();
	/// get a list of potential xcom craft landing zones for this map.
	const std::vector<LandingSite*> *getCraftPositions();
};

}

#endif
