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
#include "../Engine/Exception.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/CrossPlatform.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{

MapData *MapDataSet::_blankTile = 0;
MapData *MapDataSet::_scourgedTile = 0;

/**
* MapDataSet construction.
*/
MapDataSet::MapDataSet(const std::string &name, int size) : _name(name), _size(size), _objects(), _surfaceSet(0), _loaded(false)
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
 * Loads the map data set from a YAML file.
 * @param node YAML node.
 */
void MapDataSet::load(const YAML::Node &node)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "name")
		{
			i.second() >> _name;
		}
		else if (key == "size")
		{
			i.second() >> _size;
		}
	}
}

/**
 * Saves the map data set to a YAML file.
 * @param out YAML emitter.
 */
void MapDataSet::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << _name;
	out << YAML::Key << "size" << YAML::Value << _size;
	out << YAML::EndMap;
}

/**
* Gets the MapDataSet name (string).
* @return name.
*/
std::string MapDataSet::getName() const
{
	return _name;
}

/**
* Gets the MapDataSet size.
* @return size in number of records.
*/
int MapDataSet::getSize() const
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
SurfaceSet *MapDataSet::getSurfaceset() const
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

	int objNumber = 0;

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
	unsigned char Armor;
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
	s << "TERRAIN/" << _name << ".MCD";

	// Load file
	std::ifstream mapFile (CrossPlatform::getDataFile(s.str()).c_str(), std::ios::in | std::ios::binary);
	if (!mapFile)
	{
		throw Exception("Failed to load MCD");
	}

	while (mapFile.read((char*)&mcd, sizeof(MCD)))
	{
		MapData *to = new MapData(this);
		_objects.push_back(to);

		// set all the terrainobject properties:
		for (int frame = 0; frame < 8; frame++)
		{
			to->setSprite(frame,(int)mcd.Frame[frame]);
		}
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
		to->setArmor((int)mcd.Armor);
		to->setFlammable((int)mcd.Flammable);
		to->setFuel((int)mcd.Fuel);
		to->setExplosive((int)mcd.HE_Strength);
		to->setMiniMapIndex (mcd.ScanG);

		for (int layer = 0; layer < 12; layer++)
		{
			int loft = (int)mcd.LOFT[layer];
			to->setLoftID(loft, layer);
		}

		// store the 2 tiles of blanks in a static - so they are accesible everywhere
		if (_name.compare("BLANKS") == 0)
		{
			if (objNumber == 0)
				MapDataSet::_blankTile = to;
			else if (objNumber == 1)
				MapDataSet::_scourgedTile = to;
		}
		objNumber++;
	}


	if (!mapFile.eof())
	{
		throw Exception("Invalid data from file");
	}

	mapFile.close();

	// process the mapdataset to put block values on floortiles (as we don't have em in UFO)
	for (std::vector<MapData*>::iterator i = _objects.begin(); i != _objects.end(); ++i)
	{
		if ((*i)->getObjectType() == MapData::O_FLOOR && (*i)->getBlock(DT_HE) == 0)
		{
			(*i)->setBlockValue(1,1,(*i)->getArmor(),1,1,1);
			if ((*i)->getDieMCD())
			{
				_objects.at((*i)->getDieMCD())->setBlockValue(1,1,(*i)->getArmor(),1,1,1);
			}
		}
	}


	// Load terrain sprites/surfaces/PCK files into a surfaceset
	std::stringstream s1,s2;
	s1 << "TERRAIN/" << _name << ".PCK";
	s2 << "TERRAIN/" << _name << ".TAB";
	_surfaceSet = new SurfaceSet(32, 40);
	_surfaceSet->loadPck(CrossPlatform::getDataFile(s1.str()), CrossPlatform::getDataFile(s2.str()));

}

void MapDataSet::unload()
{
	if (_loaded)
	{
		for (std::vector<MapData*>::iterator i = _objects.begin(); i != _objects.end(); ++i)
		{
			delete *i;
		}
		delete _surfaceSet;
	}
}

/**
* loadLOFTEMPS loads the LOFTEMPS.DAT into the ruleset voxeldata
* @param filename
* @param voxelData
*/
void MapDataSet::loadLOFTEMPS(const std::string &filename, std::vector<Uint16> *voxelData)
{
	// Load file
	std::ifstream mapFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!mapFile)
	{
		throw Exception("Failed to load DAT");
	}

	Uint16 value;

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		voxelData->push_back(value);
	}

	if (!mapFile.eof())
	{
		throw Exception("Invalid data from file");
	}

	mapFile.close();
}

MapData *MapDataSet::getBlankFloorTile()
{
	return MapDataSet::_blankTile;
}

MapData *MapDataSet::getScourgedEarthTile()
{
	return MapDataSet::_scourgedTile;
}

}
