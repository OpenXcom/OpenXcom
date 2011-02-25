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

#include "MapModel.h"
#include <fstream>
#include "../Engine/Exception.h"


namespace OpenXcom
{

/**
* MapModel construction
* @param width default 16
* @param length default 16
* @param height default 24
*/
MapModel::MapModel(int width, int length, int height): _width(width), _length(length), _height(height)
{
	_voxels = new Uint16[(_width/16) * _length * _height];
	for (int i = 0; i < (_width/16) * _length * _height; i++)
		_voxels[i] = 0;
}

/**
* MapModel DESTRUCTION.
*/
MapModel::~MapModel()
{
}

/**
* loadLOFTEMPS loads the LOFTEMPS.DAT into the ruleset voxeldata
* @param filename
* @param voxelData
*/
void MapModel::loadLOFTEMPS(const std::string &filename, std::vector<Uint16> *voxelData)
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
/**
* Gets the MapModel width.
* @return width in voxels.
*/
int MapModel::getWidth() const
{
	return _width;
}

/**
* Gets the MapModel length.
* @return length in voxels.
*/
int MapModel::getLength() const
{
	return _length;
}

/**
* Gets the MapModel height.
* @return height in voxels.
*/
int MapModel::getHeight() const
{
	return _height;
}

/**
* Sets 16 voxels using an unsigned 16 bit variable. It also recalculates the checksum.
* @param voxels
* @param y this is the column
* @param z this is the layer
*/
void MapModel::set16Voxels(Uint16 voxels, int y, int z)
{
	_voxels[z * _length + y] = voxels;
}

/**
* Get a single MapModel voxel state. Voxels are either filled or not.
* We store voxels in a 16 bit unsigned int. Hence the bitshifting and tricks.
* @param x
* @param y
* @param z
* @return bool whether voxel is filled.
*/
bool MapModel::getVoxel(int x, int y, int z) const
{
	// coords are reversed
	x = 15 - x;
	y = 15 - y;
	return (_voxels[z * _length + y] & (1 << x))==(1 << x);
}

/**
* The equals function.
* @param mapModel
* @return bool
*/
bool MapModel::equals (const MapModel *mapModel)
{
	for (int i=0; i < (_width/16) * _length * _height; i++)
	{
		if (this->_voxels[i] != mapModel->_voxels[i])
			return false;
	}
	return true;
}

}
