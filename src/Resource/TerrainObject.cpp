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

/**
*
*/
TerrainObject::TerrainObject()
{
	
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
void TerrainObject::setSpecialType(int value)
{
	_specialType = (SpecialTileType)value;
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