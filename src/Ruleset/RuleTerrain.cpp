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
#include "MapDataFile.h"
#include "../Engine/RNG.h"

/**
* RuleTerrain construction
*/
RuleTerrain::RuleTerrain(const std::string &name) : _name(name)
{
}

/**
* RuleTerrain DESTRUCTION !
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
* gets a pointer to the array of mapblock
* @return pointer to the array of mapblocks
*/
std::vector<MapDataFile*> *RuleTerrain::getMapDataFiles()
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
* Generates a corresponding unique terrain object name, consisting of:
* the MCD filename + the relative position in that file
* eg "CULTA.MCD:8"
* @param absoluteID the absolute ID as found in the MAP file
* @return unique terrain object name
*/
std::string RuleTerrain::getTerrainObjectName(int absoluteID)
{
	MapDataFile* mdf = 0;
	int relativeID = absoluteID;
	std::stringstream name;
	
	for (std::vector<MapDataFile*>::iterator i = _mapDataFiles.begin(); i != _mapDataFiles.end(); i++)
	{
		mdf = *i;
		if (relativeID- mdf->getSize() < 0)
		{
			break;
		}
		relativeID -= mdf->getSize();
	}

	name << mdf->getName();
	name << ":";
	name << relativeID;

	return name.str();
}

/**
* Parses the terrain object name to get the datafilename and the relative ID in that file
* eg "CULTA.MCD:8" -> file CULTA.MCD , position 8
* @param objectName the name of the object
* @param dataFileName pointer to the datafile where the object can be found
* @param relativeID pointer to the relative ID where the object is inside the file
*/
void RuleTerrain::parseTerrainObjectName(std::string objectName, std::string *dataFileName, int *relativeID)
{
	size_t  pos = objectName.find(":", 0);			//store the position of the delimiter
	*dataFileName = objectName.substr(0, pos);      //get the filename
	objectName.erase(0, pos + 1);					//erase it from the source 
	std::istringstream issID(objectName);			// convert leftover to istringstream
	issID >> *relativeID;							// to int
}