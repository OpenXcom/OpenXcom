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
#include "TerrainObject.h"

namespace OpenXcom
{

/**
*
*/
TerrainObject::TerrainObject()
{
	_dieObject = 0;
	_altObject = 0;
}

/**
*
*/
TerrainObject::~TerrainObject()
{

}

/**
* set the sprite offset
* @param surface pointer to surface
* @param frameID frame 0-7
*/
void TerrainObject::setSprite(Surface *surface, int frameID)
{
	_surfaces[frameID] = surface;

}

/**
* get the sprite index
* @param frameID frame 0-7
* @return the original sprite index
*/
int TerrainObject::getOriginalSpriteIndex(int frameID)
{
	return _originalSpriteIndex[frameID];
}

/**
* get the sprite 
* @param frameID frame 0-7
* @return pointer to surface
*/
Surface *TerrainObject::getSprite(int frameID)
{
	return _surfaces[frameID];
}

/**
  * whether this is an animated ufo door
  * @return bool 
  */
bool TerrainObject::isUFODoor()
{
	return _isUfoDoor;
}

/**
  * gets the Y offset for drawing
  * @return int height in pixels
  */
int TerrainObject::getYOffset()
{
	return _yOffset;
}

/**
  * gets the Y offset for drawing
  * @return int height in pixels
  */
SpecialTileType TerrainObject::getSpecialType()
{
	return _specialType;
}

/*
 * Gets the TU cost to walk over the object.
 * @return TU cost
 */
int TerrainObject::getTUCost(MovementType movementType)
{
	switch (movementType)
	{
	case WALK:
		return _TUWalk;
		break;
	case FLY:
		return _TUFly;
		break;
	case SLIDE:
		return _TUSlide;
		break;
	}
	return 0;
}

/**
  * whether this is a floor
  * @return bool 
  */
bool TerrainObject::isNoFloor()
{
	return _isNoFloor;
}

/**
  * whether this is a big wall, which blocks all surrounding paths
  * @return bool 
  */
bool TerrainObject::isBigWall()
{
	if (_terrainLevel < 0) return false; // this is a hack for eg. Skyranger Ramps
	return _isBigWall;
}

/**
  * Add this to the graphical Y offset of units or objects on this tile.
  * @return Y offset
  */
int TerrainObject::getTerrainLevel()
{
	return _terrainLevel;
}

/**
  * Get index to the footstep sound.
  * @return sound ID
  */
int TerrainObject::getFootstepSound()
{
	return _footstepSound;
}

/**
  * whether this is a normal door
  * @return bool 
  */
bool TerrainObject::isDoor()
{
	return _isDoor;
}

/**
  * Get the alternative object ID 
  * @param type 0=death mcd, 1=alt mcd
  * @return object ID 
  */
int TerrainObject::getAltMCD(int type)
{
	return type==0?_dieMCD:_altMCD;
}

/**
  * Get the alternative object pointer
  * @param type 0=death mcd, 1=alt mcd
  * @return object pointer 
  */
TerrainObject *TerrainObject::getAltObject(int type)
{
	return type==0?_dieObject:_altObject;
}

/**
  * Set the alternative object pointer
  * @param obj pointer 
  * @param type 0=death mcd, 1=alt mcd
  */
void TerrainObject::setAltObject(TerrainObject *obj, int type)
{
	if (type == 0)
	{
		_dieObject = obj;
	}else
	{
		_altObject = obj;
	}
}

/**
  * Get the type of object
  * @return 0-3
  */
int TerrainObject::getObjectType()
{
	return _objectType;
}

/**
  * Get the lightblock of object.
  * @param type
  * @return lightblock
  */
int TerrainObject::getBlock(int type)
{
	return _block[type]; 
}

/**
  * Get the lightsource of object.
  * @return lightsource
  */
int TerrainObject::getLightSource()
{
	return _lightSource;
}

/**
* setter
*/
void TerrainObject::setOriginalSpriteIndex(int frameID, int value)
{
	_originalSpriteIndex[frameID] = value;
}

/**
  * setter
  */
void TerrainObject::setFlags(bool isUfoDoor, bool stopLOS, bool isNoFloor, bool isBigWall, bool isGravLift, bool isDoor, bool blockFire, bool blockSmoke)
{
	_isUfoDoor = isUfoDoor;
	_stopLOS = stopLOS;
	_isNoFloor = isNoFloor;
	_isBigWall = isBigWall;
	_isGravLift = isGravLift;
	_isDoor = isDoor;
	_blockFire = blockFire;
	_blockSmoke = blockSmoke;
}

/**
  * setter
  */
void TerrainObject::setYOffset(int value)
{
	_yOffset = value;
}

/**
  * setter
  */
void TerrainObject::setSpecialType(int value, int otype)
{
	_specialType = (SpecialTileType)value;
	_objectType = otype;
}

/**
  * setter
  */
void TerrainObject::setTUCosts(int walk, int fly, int slide)
{
	_TUWalk = walk;
	_TUFly = fly;
	_TUSlide = slide;
}

/**
  * setter
  */
void TerrainObject::setTerrainLevel(int value)
{
	_terrainLevel = value;
}

/**
  * setter
  */
void TerrainObject::setFootstepSound(int value)
{
	_footstepSound = value;
}

/**
  * setter
  */
void TerrainObject::setAltMCD(int value)
{
	_altMCD = value;
}

/**
  * setter
  */
void TerrainObject::setDieMCD(int value)
{
	_dieMCD = value;
}

/**
  * setter
  */
void TerrainObject::setBlockValue(int lightBlock, int visionBlock, int HEBlock, int smokeBlock, int fireBlock, int gasBlock)
{
	_block[0] = lightBlock==10?16:lightBlock; // small tweak to make sure 10 blocks all light...
	_block[1] = visionBlock==1?255:0;
	_block[2] = HEBlock;
	_block[3] = smokeBlock==1?255:0;
	_block[4] = fireBlock==1?255:0;
	_block[5] = gasBlock==1?255:0;
}

/**
  * setter
  */
void TerrainObject::setLightSource(int value)
{
	_lightSource = value;
}

}
