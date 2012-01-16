/*
 * Copyright 2010 OpenXcom Developers.
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
#include "Ruleset.h"

namespace OpenXcom
{

/**
* RuleTerrain construction
*/
RuleTerrain::RuleTerrain(const std::string &name) : _name(name)
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
 * @param rule Ruleset for the terrain.
 */
void RuleTerrain::load(const YAML::Node &node, const Ruleset *ruleset)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "name")
		{
			i.second() >> _name;
		}
		else if (key == "mapDataSets")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string name;
				j.second() >> name;
				_mapDataSets.push_back(ruleset->getMapDataSet(name));
			}
		}
		else if (key == "mapBlocks")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				std::string name;
				j.second()["name"] >> name;
				MapBlock *map = new MapBlock(this, name, 0, 0, false);
				map->load(j.second());
				_mapBlocks.push_back(map);
			}
		}
	}
}

/**
 * Saves the terrain to a YAML file.
 * @param out YAML emitter.
 */
void RuleTerrain::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << _name;
	out << YAML::Key << "mapDataSets" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<MapDataSet*>::const_iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		out << (*i)->getName();
	}
	out << YAML::EndSeq;
	out << YAML::Key << "mapBlocks" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<MapBlock*>::const_iterator i = _mapBlocks.begin(); i != _mapBlocks.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::EndMap;
}

/**
* gets a pointer to the array of mapblock
* @return pointer to the array of mapblocks
*/
std::vector<MapBlock*> *RuleTerrain::getMapBlocks()
{
	return &_mapBlocks;
}

/**
* gets a pointer to the array of mapdatafiles
* @return pointer to the array of mapdatafiles
*/
std::vector<MapDataSet*> *RuleTerrain::getMapDataSets()
{
	return &_mapDataSets;
}

/**
* gets the terrain name (string)
* @return name
*/
std::string RuleTerrain::getName() const
{
	return _name;
}

/**
* gets a random mapblock within the given constraints
* @param maxsize maximum size of the mapblock (1 or 2)
* @param landingzone whether this must be a landingzone (true) or don't care (false)
* @return pointer to mapblock
*/
MapBlock* RuleTerrain::getRandomMapBlock(int maxsize, bool landingzone)
{
	MapBlock* mb = 0;

	while (mb == 0)
	{
		int n = RNG::generate(0, _mapBlocks.size() - 1);
		mb = _mapBlocks[n];
		if (landingzone && !mb->isLandingZone())
		{
			mb = 0;
		}
		else if (maxsize < mb->getWidth())
		{
			mb = 0;
		}
		else if (mb->getName() == "URBAN00" || mb->getName() == "URBAN01" || mb->getName() == "URBAN02")
		{
			mb = 0;
		}
	}

	return mb;
}

/**
* Gets a mapdata object.
* @param id the id in the terrain
* @return pointer to object
*/
MapData *RuleTerrain::getMapData(int *id, int *mapDataSetID) const
{
	MapDataSet* mdf = 0;

	for (std::vector<MapDataSet*>::const_iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		mdf = *i;
		if (*id - mdf->getSize() < 0)
		{
			break;
		}
		*id -= mdf->getSize();
		(*mapDataSetID)++;
	}

	return mdf->getObjects()->at(*id);
}

}
