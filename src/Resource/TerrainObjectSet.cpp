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
#include "TerrainObjectSet.h"
#include "TerrainObject.h"
#include "../Engine/SurfaceSet.h"
#include <fstream>

/**
*
*/
TerrainObjectSet::TerrainObjectSet(const std::string &name) : _name(name)
{
}

/**
*
*/
TerrainObjectSet::~TerrainObjectSet()
{
	for (std::vector<TerrainObject*>::iterator i = _terrainObjects.begin(); i != _terrainObjects.end(); i++)
	{
		delete *i;
	}

}

/**
* @return pointer to an array of MCD objects
*/
std::vector<TerrainObject*> *TerrainObjectSet::getTerrainObjects()
{
	return &_terrainObjects;
}

/**
 * Loads terraindata in X-Com format directly into a
 * @param filename Filename of the MAP file.
 * @param tiles Pointer to the tile set.
 * @param xoff mapblock offset in X direction
 * @param yoff mapblock offset in Y direction
 * @sa http://www.ufopaedia.org/index.php?title=MCD
 */
void TerrainObjectSet::load(const std::string &filename)
{
	// the struct below helps to read the xcom file format
	struct MCD
	{
    unsigned char Frame[8];
    unsigned char LOFT[12];      
    unsigned short ScanG;
    unsigned char u23;
    unsigned char u24;
    unsigned char u25;
    unsigned char u26;
    unsigned char u27;
    unsigned char u28;
    unsigned char u29;
    unsigned char u30;
    unsigned char UFO_Door;
    unsigned char Stop_LOS;
    unsigned char No_Floor;
    unsigned char Big_Wall;      
    unsigned char Gravlift;
    unsigned char Door;
    unsigned char Block_Fire;
    unsigned char Block_Smoke;
    unsigned char u39;
    unsigned char TU_Walk;
    unsigned char TU_Fly;
    unsigned char TU_Slide;
    unsigned char Armour;
    unsigned char HE_Block;
    unsigned char Die_MCD;
    unsigned char Flammable;
    unsigned char Alt_MCD;       
    unsigned char u48;
    signed char T_Level;      
    unsigned char P_Level;
    unsigned char u51;
    unsigned char Light_Block;     
    unsigned char Footstep;         
    unsigned char Tile_Type;
    unsigned char HE_Type;         
    unsigned char HE_Strength;
    unsigned char Smoke_Blockage;      
    unsigned char Fuel;
    unsigned char Light_Source;
    unsigned char Target_Type;
    unsigned char u61;
    unsigned char u62;
	};

	MCD mcd;

	// Load file
	std::ifstream mapFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!mapFile)
	{
		throw "Failed to load MCD";
	}
	
	while (mapFile.read((char*)&mcd, sizeof(MCD)))
	{
		TerrainObject *to = new TerrainObject();
		_terrainObjects.push_back(to);

		// set all the terrainobject properties:
		for (int frame = 0; frame < 8; frame++)
			to->setOriginalSpriteIndex(frame,(int)mcd.Frame[frame]);
		to->setUfoDoor(mcd.UFO_Door == 1);
		to->setYOffset((int)mcd.P_Level);
		to->setSpecialType((int)mcd.Tile_Type);

	}

	if (!mapFile.eof())
	{
		throw "Invalid data from file";
	}

	mapFile.close();
}

/**
 * Links the terrain objects with corresponding sprites
 * @param sprites pointer to the surfaceset to link with
 */
void TerrainObjectSet::linkSprites(SurfaceSet *sprites)
{
	for (std::vector<TerrainObject*>::iterator i = _terrainObjects.begin(); i != _terrainObjects.end(); i++)
	{
		TerrainObject *tob = (TerrainObject*)*i;
		for (int frame = 0; frame < 8; frame++)
		{
			tob->setSprite(sprites->getFrame(tob->getOriginalSpriteIndex(frame)),frame);
		}
	}
}
