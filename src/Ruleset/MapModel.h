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
#ifndef OPENXCOM_MAPMODEL_H
#define OPENXCOM_MAPMODEL_H

#include <string>
#include <vector>
#include "SDL.h"

namespace OpenXcom
{

class RuleTerrain;

/**
 * Represents a 3D voxel model. We assume a width of 16 voxels, with a possible extension to 32 in the future?
 * @sa http://www.ufopaedia.org/index.php?title=LOFTEMPS.DAT
 */
class MapModel
{
private:
	int _width, _length, _height;
	Uint16 *_voxels;
	Uint32 fletcher32( Uint16 *data, size_t len );
	Uint32 _checksum;
public:
	MapModel(int width = 16, int length = 16, int height = 24);
	~MapModel();
	/// Loads voxeldata from a DAT file.
	static void loadLOFTEMPS(const std::string &filename, std::vector<Uint16> *voxelData);
	/// Gets the model's width.
	int getWidth() const;
	/// Get the model's length.
	int getLength() const;
	/// Get the model's height.
	int getHeight() const;
	/// Set a row of 16 voxels.
	void set16Voxels(Uint16 voxels, int y, int z);
	/// Get a single voxel.
	bool getVoxel(int x, int y, int z) const;
	/// Equals function.
    bool equals (const MapModel *mapModel) const;

};

}

#endif
