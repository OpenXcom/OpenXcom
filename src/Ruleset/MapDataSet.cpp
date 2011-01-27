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
#include "MapDataSet.h"
#include "MapData.h"
#include <fstream>
#include <sstream>
#include "../Engine/SurfaceSet.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{

/**
* MapDataSet construction.
*/
MapDataSet::MapDataSet(std::string name, int size):_name(name), _size(size), _objects(), _loaded(false)
{
}


/**
* MapDataSet DESTRUCTION.
*/
MapDataSet::~MapDataSet()
{
	unload();
}

/**
* Gets the MapDataSet name (string).
* @return name.
*/
std::string MapDataSet::getName()
{
	return _name;
}

/**
* Gets the MapDataSet size.
* @return size in number of records.
*/
int MapDataSet::getSize()
{
	return _size;
}


/**
* @return pointer to the objects
*/
std::vector<MapData*> *MapDataSet::getObjects()
{
	return &_objects;
}

/**
* @return pointer to the surfaceset
*/
SurfaceSet *MapDataSet::getSurfaceset()
{
	return _surfaceSet;
}

/**
 * Loads terraindata in X-Com format (MCD & PCK files)
 * @param res The resourcepack.
 * @sa http://www.ufopaedia.org/index.php?title=MCD
 */
void MapDataSet::load(ResourcePack *res)
{
	// prevents loading twice
	if (_loaded) return;
	_loaded = true;

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


	// Load Terrain Data from MCD file
	std::stringstream s;
	s << res->getFolder() << "TERRAIN/" << _name << ".MCD";

	// Load file
	std::ifstream mapFile (ResourcePack::insensitive(s.str()).c_str(), std::ios::in | std::ios::binary);
	if (!mapFile)
	{
		throw "Failed to load MCD";
	}

	while (mapFile.read((char*)&mcd, sizeof(MCD)))
	{
		MapData *to = new MapData(this);
		_objects.push_back(to);

		// set all the terrainobject properties:
		for (int frame = 0; frame < 8; frame++)
			to->setSprite(frame,(int)mcd.Frame[frame]);
		to->setYOffset((int)mcd.P_Level);
		to->setSpecialType((int)mcd.Target_Type, (int)mcd.Tile_Type);
		to->setTUCosts((int)mcd.TU_Walk, (int)mcd.TU_Fly, (int)mcd.TU_Slide);
		to->setFlags(mcd.UFO_Door == 1, mcd.Stop_LOS == 1, mcd.No_Floor == 1, mcd.Big_Wall == 1, mcd.Gravlift == 1, mcd.Door == 1, mcd.Block_Fire == 1, mcd.Block_Smoke == 1);
		to->setTerrainLevel((int)mcd.T_Level);
		to->setFootstepSound((int)mcd.Footstep);
		to->setAltMCD((int)(mcd.Alt_MCD));
		to->setDieMCD((int)(mcd.Die_MCD));
		to->setBlockValue((int)mcd.Light_Block, (int)mcd.Stop_LOS, (int)mcd.HE_Block, (int)mcd.Block_Smoke, (int)mcd.Block_Fire, (int)mcd.Block_Smoke);
		to->setLightSource((int)mcd.Light_Source);
	}

	if (!mapFile.eof())
	{
		throw "Invalid data from file";
	}

	mapFile.close();

	// Load terrain sprites/surfaces/PCK files into a surfaceset
	std::stringstream s1,s2;
	s1 << res->getFolder() << "TERRAIN/" << _name << ".PCK";
	s2 << res->getFolder() << "TERRAIN/" << _name << ".TAB";
	_surfaceSet = new SurfaceSet(32, 40);
	_surfaceSet->loadPck(ResourcePack::insensitive(s1.str()), ResourcePack::insensitive(s2.str()));

}

void MapDataSet::unload()
{
	if (_loaded)
	{
		for (std::vector<MapData*>::iterator i = _objects.begin(); i != _objects.end(); i++)
		{
			delete *i;
		}
		delete _surfaceSet;
	}
}

}
