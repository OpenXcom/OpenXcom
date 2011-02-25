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
	// recalculate checksum each time?
	_checksum = fletcher32(_voxels, (_width/16) * _length * _height);
}

/**
* Get a single MapModel voxel state. Voxels are either filled or not.
* We store voxels in a 16 bit unsigned int. Hence the bitshifting and tricks.
* Also: Y is reversed...
* @param x
* @param y
* @param z
* @return bool whether voxel is filled.
*/
bool MapModel::getVoxel(int x, int y, int z) const
{
	return (_voxels[z * _length + (15-y)] & (1 << x))==(1 << x);
}

/**
* The equals function. Compares two checksums of mapmodel objects with their checksum.
* @param mapModel
* @return bool
*/
bool MapModel::equals (const MapModel *mapModel) const
{
	return _checksum == mapModel->_checksum;
}

/**
* Calculates a checksum. Used to compare two models.
* @param data Pointer to the data.
* @param len Length of the data.
* @return Uint32 a 32bit checksum.
*/
Uint32 MapModel::fletcher32( Uint16 *data, size_t len )
{
        Uint32 sum1 = 0xffff, sum2 = 0xffff;
 
        while (len) {
                unsigned tlen = len > 360 ? 360 : len;
                len -= tlen;
                do {
                        sum1 += *data++;
                        sum2 += sum1;
                } while (--tlen);
                sum1 = (sum1 & 0xffff) + (sum1 >> 16);
                sum2 = (sum2 & 0xffff) + (sum2 >> 16);
        }
        /* Second reduction step to reduce sums to 16 bits */
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
        return sum2 << 16 | sum1;
}

}
