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

#include <sstream>
#include <iostream>
#include "RuleTerrain.h"
#include "MapBlock.h"
#include "MapDataSet.h"
#include "../Engine/RNG.h"

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
	for (std::vector<MapBlock*>::iterator i = _mapBlocks.begin(); i != _mapBlocks.end(); i++)
	{
		delete *i;
	}
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
	return &_mapDataFiles;
}

/**
* gets the terrain name (string)
* @return name
*/
std::string RuleTerrain::getName()
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
	}
	
	return mb;
}

/**
* Gets a mapdata object.
* @param id the id in the terrain
* @return pointer to object
*/
MapData *RuleTerrain::getMapData(int id)
{
	MapDataSet* mdf = 0;
	int relativeID = id;
	
	for (std::vector<MapDataSet*>::iterator i = _mapDataFiles.begin(); i != _mapDataFiles.end(); i++)
	{
		mdf = *i;
		if (relativeID- mdf->getSize() < 0)
		{
			break;
		}
		relativeID -= mdf->getSize();
	}

	return mdf->getObjects()->at(relativeID);
}

}
