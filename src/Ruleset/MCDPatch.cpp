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
			_bigWalls.push_back(std::make_pair(MCDIndex, bigWall));
		}
		if ((*i)["TUWalk"])
		{
			int TUWalk = (*i)["TUWalk"].as<int>();
			_TUWalks.push_back(std::make_pair(MCDIndex, TUWalk));
		}
		if ((*i)["TUFly"])
		{
			int TUFly = (*i)["TUFly"].as<int>();
			_TUFlys.push_back(std::make_pair(MCDIndex, TUFly));
		}
		if ((*i)["TUSlide"])
		{
			int TUSlide = (*i)["TUSlide"].as<int>();
			_TUSlides.push_back(std::make_pair(MCDIndex, TUSlide));
		}
		if ((*i)["deathTile"])
		{
			int deathTile = (*i)["deathTile"].as<int>();
			_deathTiles.push_back(std::make_pair(MCDIndex, deathTile));
		}
		if ((*i)["terrainHeight"])
		{
			int terrainHeight = (*i)["terrainHeight"].as<int>();
			_terrainHeight.push_back(std::make_pair(MCDIndex, terrainHeight));
		}
	}
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
}

}
