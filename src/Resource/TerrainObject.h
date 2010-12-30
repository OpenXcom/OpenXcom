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

#include <string>

namespace OpenXcom
{

class Surface;

#define O_FLOOR 0
#define O_WESTWALL 1
#define O_NORTHWALL 2
#define O_OBJECT 3

enum SpecialTileType{TILE=0,
					START_POINT,
					ION_BEAM_ACCEL,
					DESTROY_OBJECTIVE,
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

enum MovementType{WALK=0, FLY, SLIDE};

/**
 * A TerrainObject is the smallest piece of a Battlescape terrain
 * @sa TerrainObjectSet
 */
class TerrainObject
{
private:
	std::string _mapDataFileName;
	Surface *_surfaces[8];
	TerrainObject *_dieObject;
	TerrainObject *_altObject;
	int _originalSpriteIndex[8];
	bool _isUfoDoor, _stopLOS, _isNoFloor, _isBigWall, _isGravLift, _isDoor, _blockFire, _blockSmoke;
	int _yOffset;
	SpecialTileType _specialType;
	int _TUWalk, _TUFly, _TUSlide;
	int _terrainLevel;
	int _footstepSound;
	int _dieMCD, _altMCD;
	int _objectType;
	int _block[6];
	int _lightSource;
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
	/// TU cost to walk over the object.
	int getTUCost(MovementType movementType);
	/// Can we walk over it.
	bool isNoFloor();
	/// Add this to the graphical Y offset of units or objects on this tile.
	int getTerrainLevel();
	/// Can we walk over it.
	bool isBigWall();
	/// Get the index to the footstep sound
	int getFootstepSound();
	/// Is a normal door.
	bool isDoor();
	/// Get the alternative object ID 0=death mcd, 1=alt mcd
	int getAltMCD(int type);
	/// Get the alternative object pointer.
	TerrainObject *getAltObject(int type);
	/// Set the alternative object pointer.
	void setAltObject(TerrainObject *obj, int type);
	/// Get the type of tile.
	int getObjectType();
	/// 
	int getBlock(int type);
	/// 
	int getLightSource();

	
	// below are setter functions for the properties
	void setFlags(bool isUfoDoor, bool stopLOS, bool isNoFloor, bool isBigWall, bool isGravLift, bool isDoor, bool blockFire, bool blockSmoke);
	void setYOffset(int value);
	void setOriginalSpriteIndex(int frameID, int value);
	void setSpecialType(int value, int otype);
	void setTUCosts(int walk, int fly, int slide);
	void setTerrainLevel(int value);
	void setFootstepSound(int value);
	void setAltMCD(int value);
	void setDieMCD(int value);
	void setBlockValue(int lightBlock, int visionBlock, int HEBlock, int smokeBlock, int fireBlock, int gasBlock);
	void setLightSource(int value);
};

}

#endif
