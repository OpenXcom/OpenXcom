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
#ifndef OPENXCOM_TERRAINOBJECT_H
#define OPENXCOM_TERRAINOBJECT_H

class Surface;
#include <string>

enum SpecialTileType{TILE=0,
					START_POINT,
					ION_BEAM_ACCEL,
					DESTORY_OBJECTIVE,
					MAGNETIC_NAV,
					ALIEN_CRYO,
					ALIEN_CLON,
					ALIEN_LEARN,
					ALIEN_IMPLANT,
					UKNOWN09,
					ALIEN_PLASTICS,
					EXAM_ROOM,
					DEAD_TILE,
					END_POINT,
					MUST_DESTROY};
/**
 * A TerrainObject is the smallest piece of a Battlescape terrain
 * @sa TerrainObject
 */
class TerrainObject
{
private:
	std::string _mapDataFileName;
	Surface *_surfaces[8];
	TerrainObject *deadObject;
	TerrainObject *alternativeObject;
	int _originalSpriteIndex[8];
	bool _isUfoDoor;
	int _yOffset;
	SpecialTileType _specialType;
public:
	TerrainObject();
	~TerrainObject();
	/// set the sprite id for a certain frame
	void setSprite(Surface *surface, int frameID);
	/// get the sprite for a certain frame
	Surface *getSprite(int frameID);
	/// get the sprite index for a certain frame
	int getOriginalSpriteIndex(int frameID);
	/// whether this is an animated ufo door
	bool isUFODoor();
	/// a offset on the Y axis when drawing this object
	int getYOffset();
	/// info about special tile types
	SpecialTileType getSpecialType();
	
	// below are setter functions for the properties
	void setUfoDoor(bool flag);
	void setYOffset(int value);
	void setOriginalSpriteIndex(int frameID, int value);
	void setSpecialType(int value);
};

#endif
