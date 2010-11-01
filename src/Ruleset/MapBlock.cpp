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
#include "MapBlock.h"
#include <fstream>
#include <sstream>
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"

/**
* MapBlock construction
*/
MapBlock::MapBlock(std::string name, int width, int length, bool landingZone):_name(name),_width(width),_length(length),_landingZone(landingZone)
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
 * @param SavedBattleGame* pointer to the current battle game
 * @param TerrainObjectIDoff offset of the TerrainObjectID values (loading of craft)
 * @sa http://www.ufopaedia.org/index.php?title=MAPS
 * @return the size of the map loaded 1 or 2
 * NOTE that Y-axis is in reverse order
 */
void MapBlock::loadMAP(int xoff, int yoff, SavedBattleGame* save, int TerrainObjectIDoff)
{
	int width,length,height;
	int x=xoff,y=yoff,z=0;
	char size[3];
	char value[4];
	int mcd[4];
	std::stringstream filename;
	filename << "./DATA/" << "MAPS/" << _name << ".MAP";


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
	z+=height-1;
	y+=length-1;

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		for (int p=0;p<4;p++)
		{
			mcd[p] = (int)value[p];
			if (mcd[p]>0 && TerrainObjectIDoff>0)
				mcd[p] += TerrainObjectIDoff;
		}

		// get the original floor tile back
		// so craft placements don't overwrite it
		if (mcd[0]==0)
			mcd[0] = save->getTile(x,y,z)->getTerrainObjectID(0);

		save->getTile(x,y,z)->setTerrainObjectID(mcd);

		x++;

		if (x==(width+xoff))
		{
			x=xoff;
			y--;
		}
		if (y==yoff-1)
		{
			y=length-1+yoff;
			z--;
		}
	}

	if (!mapFile.eof())
	{
		throw "Invalid data from file";
	}

	mapFile.close();
}