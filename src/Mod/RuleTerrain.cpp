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

#include "RuleTerrain.h"
#include "MapBlock.h"
#include "MapDataSet.h"
#include "../Engine/RNG.h"
#include "Mod.h"

namespace OpenXcom
{

/**
 * RuleTerrain construction.
 */
RuleTerrain::RuleTerrain(const std::string &name) : _name(name), _script("DEFAULT"), _minDepth(0), _maxDepth(0), _ambience(-1), _ambientVolume(0.5)
{
}

/**
 * Ruleterrain only holds mapblocks. Map datafiles are referenced.
 */
RuleTerrain::~RuleTerrain()
{
	for (std::vector<MapBlock*>::iterator i = _mapBlocks.begin(); i != _mapBlocks.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Loads the terrain from a YAML file.
 * @param node YAML node.
 * @param mod Mod for the terrain.
 */
void RuleTerrain::load(const YAML::Node &node, Mod *mod)
{
	if (const YAML::Node &map = node["mapDataSets"])
	{
		_mapDataSets.clear();
		for (YAML::const_iterator i = map.begin(); i != map.end(); ++i)
		{
			_mapDataSets.push_back(mod->getMapDataSet(i->as<std::string>()));
		}
	}
	if (const YAML::Node &map = node["mapBlocks"])
	{
		_mapBlocks.clear();
		for (YAML::const_iterator i = map.begin(); i != map.end(); ++i)
		{
			MapBlock *mapBlock = new MapBlock((*i)["name"].as<std::string>());
			mapBlock->load(*i);
			_mapBlocks.push_back(mapBlock);
		}
	}
	_name = node["name"].as<std::string>(_name);
	if (const YAML::Node &civs = node["civilianTypes"])
	{
		_civilianTypes = civs.as<std::vector<std::string> >(_civilianTypes);
	}
	else
	{
		_civilianTypes.push_back("MALE_CIVILIAN");
		_civilianTypes.push_back("FEMALE_CIVILIAN");
	}
	for (YAML::const_iterator i = node["music"].begin(); i != node["music"].end(); ++i)
	{
		_music.push_back((*i).as<std::string>(""));
	}
	if (node["depth"])
	{
		_minDepth = node["depth"][0].as<int>(_minDepth);
		_maxDepth = node["depth"][1].as<int>(_maxDepth);
	}
	if (node["ambience"])
	{
		_ambience = mod->getSoundOffset(node["ambience"].as<int>(_ambience), "BATTLE.CAT");
	}
	_ambientVolume = node["ambientVolume"].as<double>(_ambientVolume);
	_script = node["script"].as<std::string>(_script);
}

/**
 * Gets the array of mapblocks.
 * @return Pointer to the array of mapblocks.
 */
std::vector<MapBlock*> *RuleTerrain::getMapBlocks()
{
	return &_mapBlocks;
}

/**
 * Gets the array of mapdatafiles.
 * @return Pointer to the array of mapdatafiles.
 */
std::vector<MapDataSet*> *RuleTerrain::getMapDataSets()
{
	return &_mapDataSets;
}

/**
 * Gets the terrain name.
 * @return The terrain name.
 */
std::string RuleTerrain::getName() const
{
	return _name;
}

/**
 * Gets a random mapblock within the given constraints.
 * @param maxsize The maximum size of the mapblock (10 or 20 or 999 - don't care).
 * @param type Whether this must be a block of a certain type.
 * @param force Whether to enforce the max size.
 * @return Pointer to the mapblock.
 */
MapBlock* RuleTerrain::getRandomMapBlock(int maxSizeX, int maxSizeY, int group, bool force)
{
	std::vector<MapBlock*> compliantMapBlocks;

	for (std::vector<MapBlock*>::const_iterator i = _mapBlocks.begin(); i != _mapBlocks.end(); ++i)
	{
		if (((*i)->getSizeX() == maxSizeX ||
			(!force && (*i)->getSizeX() < maxSizeX)) &&
			((*i)->getSizeY() == maxSizeY ||
			(!force && (*i)->getSizeY() < maxSizeY)) &&
			(*i)->isInGroup(group))
		{
			compliantMapBlocks.push_back((*i));
		}
	}

	if (compliantMapBlocks.empty()) return 0;

	size_t n = RNG::generate(0, compliantMapBlocks.size() - 1);

	return compliantMapBlocks[n];
}

/**
 * Gets a mapblock with a given name.
 * @param name The name of the mapblock.
 * @return Pointer to mapblock.
 */
MapBlock* RuleTerrain::getMapBlock(const std::string &name)
{
	for (std::vector<MapBlock*>::const_iterator i = _mapBlocks.begin(); i != _mapBlocks.end(); ++i)
	{
		if ((*i)->getName() == name)
			return (*i);
	}
	return 0;
}

/**
 * Gets a mapdata object.
 * @param id The id in the terrain.
 * @param mapDataSetID The id of the map data set.
 * @return Pointer to MapData object.
 */
MapData *RuleTerrain::getMapData(unsigned int *id, int *mapDataSetID) const
{
	MapDataSet* mdf = 0;
	std::vector<MapDataSet*>::const_iterator i = _mapDataSets.begin();
	for (; i != _mapDataSets.end(); ++i)
	{
		mdf = *i;
		if (*id < mdf->getSize())
		{
			break;
		}
		*id -= mdf->getSize();
		(*mapDataSetID)++;
	}
	if (i == _mapDataSets.end())
	{
		// oops! someone at microprose made an error in the map!
		// set this broken tile reference to BLANKS 0.
		mdf = _mapDataSets.front();
		*id = 0;
		*mapDataSetID = 0;
	}
	return mdf->getObjects()->at(*id);
}

/**
 * Gets the list of civilian types to use on this terrain (default MALE_CIVILIAN and FEMALE_CIVILIAN)
 * @return list of civilian types to use.
 */
std::vector<std::string> RuleTerrain::getCivilianTypes() const
{
	return _civilianTypes;
}

/**
 * Gets the min depth.
 * @return The min depth.
 */
int RuleTerrain::getMinDepth() const
{
	return _minDepth;
}

/**
 * Gets the max depth.
 * @return max depth.
 */
int RuleTerrain::getMaxDepth() const
{
	return _maxDepth;
}

/**
 * Gets The ambient sound effect.
 * @return The ambient sound effect.
 */
int RuleTerrain::getAmbience() const
{
	return _ambience;
}

/**
 * Gets The generation script name.
 * @return The name of the script to use.
 */
std::string RuleTerrain::getScript() const
{
	return _script;
}

/**
 * Gets The list of musics this terrain has to choose from.
 * @return The list of track names.
 */
const std::vector<std::string> &RuleTerrain::getMusic() const
{
	return _music;
}


double RuleTerrain::getAmbientVolume() const
{
	return _ambientVolume;
}

}
