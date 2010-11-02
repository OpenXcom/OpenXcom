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
#include "MapBlock.h"
#include "RuleTerrain.h"
#include <fstream>
#include <sstream>
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/TerrainObject.h"
#include "../Resource/TerrainObjectSet.h"

/**
* MapBlock construction
*/
MapBlock::MapBlock(RuleTerrain *terrain, std::string name, int width, int length, bool landingZone):_terrain(terrain), _name(name), _width(width), _length(length), _landingZone(landingZone)
{
}

/**
* MapBlock DESTRUCTION !
*/
MapBlock::~MapBlock()
{
}

/**
* gets the MapBlock name (string)
* @return name
*/
std::string MapBlock::getName()
{
	return _name;
}

/**
* gets the MapBlock width
* @return width in tiles
*/
int MapBlock::getWidth()
{
	return _width;
}

/**
* gets the MapBlock length
* @return length in tiles
*/
int MapBlock::getLength()
{
	return _length;
}

/**
* Is this mapblock usable as a landingzone?
* You don't want to land your Skyranger into a house,
*  it tends to upset civilians
* @return bool whether you want to land here or not
*/
bool MapBlock::isLandingZone()
{
	return _landingZone;
}

/**
 * Loads a X-Com format MAP file into the tiles of the battlegame
 * @param xoff mapblock offset in X direction
 * @param yoff mapblock offset in Y direction
 * @param save pointer to the current battle game
 * @param tos pointer to the terrain object set to get the terrain object pointers from
 * @return int Height of the loaded mapblock (this is needed for spawpoint calculation...)
 * @sa http://www.ufopaedia.org/index.php?title=MAPS
 * NOTE that Y-axis is in reverse order
 */
int MapBlock::loadMAP(int xoff, int yoff, SavedBattleGame *save, RuleTerrain *terrain)
{
	int width, length, height;
	int x = xoff, y = yoff, z = 0;
	char size[3];
	char value[4];
	std::stringstream filename;
	filename << "./DATA/" << "MAPS/" << _name << ".MAP";
	std::string mapDataFileName;
	int terrainObjectID;

	// Load file
	std::ifstream mapFile (filename.str().c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw "Failed to load MAP";
	}
	
	mapFile.read((char*)&size, sizeof(size));
	length = (int)size[0];
	width = (int)size[1];
	height = (int)size[2];
	z += height - 1;
	y += length - 1;

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		for (int part = 0; part < 4; part++)
		{
			terrainObjectID = (int)value[part];
			if (terrainObjectID>0)
			{
				terrain->getMapDatafile(&terrainObjectID, &mapDataFileName);
				save->getTile(x, y, z)->setTerrainObjectID(terrainObjectID,part);
				save->getTile(x, y, z)->setMapDataFileName(mapDataFileName,part);
			}
		}

		x++;

		if (x == (width + xoff))
		{
			x = xoff;
			y--;
		}
		if (y == yoff - 1)
		{
			y = length - 1 + yoff;
			z--;
		}
	}

	if (!mapFile.eof())
	{
		throw "Invalid data from file";
	}

	mapFile.close();

	return height;
}

/**
 *  UNDER CONSTRUCTION
 *
 * Loads a X-Com format RMP file into the spawnpoints of the battlegame
 * @param xoff mapblock offset in X direction
 * @param yoff mapblock offset in Y direction
 * @param SavedBattleGame* pointer to the current battle game
 * @sa http://www.ufopaedia.org/index.php?title=ROUTES
 */
void MapBlock::loadRMP(int xoff, int yoff, int height, SavedBattleGame* save)
{
	//char value[24];
	std::stringstream filename;
	filename << "./DATA/" << "ROUTES/" << _name << ".RMP";

	// Load file
	std::ifstream mapFile (filename.str().c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw "Failed to load RMP";
	}
	
/*	while (mapFile.read((char*)&value, sizeof(value)))
	{

	}*/

	if (!mapFile.eof())
	{
		throw "Invalid data from file";
	}

	mapFile.close();
}
