/*
 * Copyright 2010 OpenXcom Developers
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
#include "Terrain.h"
#include <fstream>
#include <sstream>
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/RNG.h"
#include "../Resource/ResourcePack.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Battlescape/Terraindata.h"
#include "MapBlock.h"

/**
* Terrain construction
*/
Terrain::Terrain(std::string name)
{
	_name = name;
}


/**
* Terrain DESTRUCTION !
*/
Terrain::~Terrain()
{
	for (std::vector<MapBlock*>::iterator i = _mapblocks.begin(); i != _mapblocks.end(); i++)
	{
		delete *i;
	}
}

/**
* gets a pointer to the array of sheetnames(strings)
* @return pointer to the array of sheetnames(strings)
*/
std::vector<std::string*> *Terrain::getSheetnames()
{
	return &_sheetnames;
}

/**
* gets a pointer to the array of mapblock
* @return pointer to the array of mapblocks
*/
std::vector<MapBlock*> *Terrain::getMapBlocks()
{
	return &_mapblocks;
}


/**
* gets the terrain name (string)
* @return name
*/
std::string Terrain::getName()
{
	return _name;
}


MapBlock* Terrain::getRandomMapBlock(int maxsize, bool landingzone)
{
	MapBlock* mb = NULL;

	while (mb==NULL)
	{
		int n = RNG::generate(0,_mapblocks.size()-1);
		mb = _mapblocks[n];
		if (landingzone && !mb->isLandingZone())
		{
			mb = NULL;
		}else if (maxsize < mb->getWidth())
		{
			mb = NULL;
		}
	}
	
	return mb;
}