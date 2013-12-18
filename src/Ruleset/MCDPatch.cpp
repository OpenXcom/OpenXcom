/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "MCDPatch.h"
#include "MapDataSet.h"
#include "MapData.h"
#include <algorithm>

namespace OpenXcom
{

/**
 * Initializes an MCD Patch.
 */
MCDPatch::MCDPatch()
{
}

/**
 *
 */
MCDPatch::~MCDPatch()
{
}

struct PairFirstComp {
	template<typename Second>
	bool operator()(const std::pair<size_t, Second>& lhs, const std::pair<size_t, Second>& rhs)
	{
		return lhs.first < rhs.first;
	}
};
// Inserts value or replaces the pair with the same first item if ait exists
template<typename Container, typename Second>
static typename Container::iterator insert_replace(Container& container, const std::pair<size_t, Second>& value)
{
	typename Container::iterator end = container.end();
	typename Container::iterator lower = std::lower_bound(container.begin(), end, value, PairFirstComp());
	typename Container::iterator upper = std::upper_bound(lower, end, value, PairFirstComp());
	
	if(lower != upper)
	{
		*lower = value;
		return lower;
	}
	return container.insert(upper, value);
}

/**
 * Loads the MCD Patch from a YAML file.
 * TODO: fill this out with more data.
 * @param node YAML node.
 */
void MCDPatch::load(const YAML::Node &node)
{
	YAML::Node data = node["data"];
	for (YAML::const_iterator i = data.begin(); i != data.end(); ++i)
	{
		size_t MCDIndex = (*i)["MCDIndex"].as<size_t>();
		if ((*i)["bigWall"])
		{
			int bigWall = (*i)["bigWall"].as<int>();
			insert_replace(_bigWalls, std::make_pair(MCDIndex, bigWall));
		}
		if ((*i)["TUWalk"])
		{
			int TUWalk = (*i)["TUWalk"].as<int>();
			insert_replace(_TUWalks, std::make_pair(MCDIndex, TUWalk));
		}
		if ((*i)["TUFly"])
		{
			int TUFly = (*i)["TUFly"].as<int>();
			insert_replace(_TUFlys, std::make_pair(MCDIndex, TUFly));
		}
		if ((*i)["TUSlide"])
		{
			int TUSlide = (*i)["TUSlide"].as<int>();
			insert_replace(_TUSlides, std::make_pair(MCDIndex, TUSlide));
		}
		if ((*i)["deathTile"])
		{
			int deathTile = (*i)["deathTile"].as<int>();
			insert_replace(_deathTiles, std::make_pair(MCDIndex, deathTile));
		}
		if ((*i)["terrainHeight"])
		{
			int terrainHeight = (*i)["terrainHeight"].as<int>();
			insert_replace(_terrainHeight, std::make_pair(MCDIndex, terrainHeight));
		}
		if ((*i)["specialType"])
		{
			int specialType = (*i)["specialType"].as<int>();
			insert_replace(_specialTypes, std::make_pair(MCDIndex, specialType));
		}
		if ((*i)["LOFTS"])
		{
			std::vector<int> lofts = (*i)["LOFTS"].as< std::vector<int> >();
			insert_replace(_LOFTS, std::make_pair(MCDIndex, lofts));
		}
	}
}

/**
 * Saves the region type to a YAML file.
 * @return YAML node.
 */
YAML::Node MCDPatch::save() const
{
	YAML::Node node;
	YAML::Node data;
	size_t i_bigWalls = 0, i_TUWalks = 0, i_TUFlys = 0, i_TUSlides = 0, i_deathTiles = 0, i_terrainHeight = 0, i_specialTypes = 0;
	bool stop = false;
	for (size_t i = 0; !stop; ++i)
	{
		YAML::Node item;
		stop = true;
		if (i_bigWalls < _bigWalls.size())
		{
			stop = false;
			if (_bigWalls[i_bigWalls].first == i)
			{
				item["bigWall"] = _bigWalls[i_bigWalls++].second;
			}
		}
		if (i_TUWalks < _TUWalks.size())
		{
			stop = false;
			if (_TUWalks[i_TUWalks].first == i)
			{
				item["TUWalk"] = _TUWalks[i_TUWalks++].second;
			}
		}
		if (i_TUFlys < _TUFlys.size())
		{
			stop = false;
			if (_TUFlys[i_TUFlys].first == i)
			{
				item["TUFly"] = _TUWalks[i_TUFlys++].second;
			}
		}
		if (i_TUSlides < _TUSlides.size())
		{
			stop = false;
			if (_TUSlides[i_TUSlides].first == i)
			{
				item["TUSlide"] = _TUSlides[i_TUSlides++].second;
			}
		}
		if (i_deathTiles < _deathTiles.size())
		{
			stop = false;
			if (_deathTiles[i_deathTiles].first == i)
			{
				item["TUSlide"] = _deathTiles[i_deathTiles++].second;
			}
		}
		if (item.size())
		{
			data[i] = item;
		}
	}
	node["data"] = data;
	return node;
}

/**
 * Saves the region type to a YAML file.
 * @return YAML node.
 */
YAML::Node MCDPatch::save(const std::string& type) const
{
	YAML::Node node = save();
	node["type"] = type;
	return node;
}

/**
 * Applies an MCD patch to a mapDataSet.
 * @param dataSet The MapDataSet we want to modify.
 */
void MCDPatch::modifyData(MapDataSet *dataSet) const
{
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _bigWalls.begin(); i != _bigWalls.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setBigWall(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _TUWalks.begin(); i != _TUWalks.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setTUWalk(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _TUFlys.begin(); i != _TUFlys.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setTUFly(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _TUSlides.begin(); i != _TUSlides.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setTUSlide(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _deathTiles.begin(); i != _deathTiles.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setDieMCD(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _terrainHeight.begin(); i != _terrainHeight.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setTerrainLevel(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _specialTypes.begin(); i != _specialTypes.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setSpecialType(i->second, dataSet->getObjects()->at(i->first)->getObjectType());
	}
	for (std::vector<std::pair<size_t, std::vector<int> > >::const_iterator i = _LOFTS.begin(); i != _LOFTS.end(); ++i)
	{
		int layer = 0;
		for (std::vector<int>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
		{
			dataSet->getObjects()->at(i->first)->setLoftID(*j, layer);
			++layer;
		}
	}
}

}
