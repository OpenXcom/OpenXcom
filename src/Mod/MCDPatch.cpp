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
		if ((*i)["specialType"])
		{
			int specialType = (*i)["specialType"].as<int>();
			_specialTypes.push_back(std::make_pair(MCDIndex, specialType));
		}
		if ((*i)["explosive"])
		{
			int explosive = (*i)["explosive"].as<int>();
			_explosives.push_back(std::make_pair(MCDIndex, explosive));
		}
		if ((*i)["armor"])
		{
			int armor = (*i)["armor"].as<int>();
			_armors.push_back(std::make_pair(MCDIndex, armor));
		}
		if ((*i)["flammability"])
		{
			int flammability = (*i)["flammability"].as<int>();
			_flammabilities.push_back(std::make_pair(MCDIndex, flammability));
		}
		if ((*i)["fuel"])
		{
			int fuel = (*i)["fuel"].as<int>();
			_fuels.push_back(std::make_pair(MCDIndex, fuel));
		}
		if ((*i)["footstepSound"])
		{
			int footstepSound = (*i)["footstepSound"].as<int>();
			_footstepSounds.push_back(std::make_pair(MCDIndex, footstepSound));
		}
		if ((*i)["HEBlock"])
		{
			int HEBlock = (*i)["HEBlock"].as<int>();
			_HEBlocks.push_back(std::make_pair(MCDIndex, HEBlock));
		}
		if ((*i)["noFloor"])
		{
			bool noFloor = (*i)["noFloor"].as<bool>();
			_noFloors.push_back(std::make_pair(MCDIndex, noFloor));
		}
		if ((*i)["LOFTS"])
		{
			std::vector<int> lofts = (*i)["LOFTS"].as< std::vector<int> >();
			_LOFTS.push_back(std::make_pair(MCDIndex, lofts));
		}
		if ((*i)["stopLOS"])
		{
			bool stopLOS = (*i)["stopLOS"].as<bool>();
			_stopLOSses.push_back(std::make_pair(MCDIndex, stopLOS));
		}
		if ((*i)["objectType"])
		{
			int objectType = (*i)["objectType"].as<int>();
			_objectTypes.push_back(std::make_pair(MCDIndex, objectType));
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
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _specialTypes.begin(); i != _specialTypes.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setSpecialType(i->second, dataSet->getObjects()->at(i->first)->getObjectType());
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _explosives.begin(); i != _explosives.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setExplosive(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _armors.begin(); i != _armors.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setArmor(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _flammabilities.begin(); i != _flammabilities.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setFlammable(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _fuels.begin(); i != _fuels.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setFuel(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _HEBlocks.begin(); i != _HEBlocks.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setHEBlock(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _footstepSounds.begin(); i != _footstepSounds.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setFootstepSound(i->second);
	}
	for (std::vector<std::pair<size_t, int> >::const_iterator i = _objectTypes.begin(); i != _objectTypes.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setObjectType((TilePart)i->second);
	}
	for (std::vector<std::pair<size_t, bool> >::const_iterator i = _noFloors.begin(); i != _noFloors.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setNoFloor(i->second);
	}
	for (std::vector<std::pair<size_t, bool> >::const_iterator i = _stopLOSses.begin(); i != _stopLOSses.end(); ++i)
	{
		dataSet->getObjects()->at(i->first)->setStopLOS(i->second);
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
