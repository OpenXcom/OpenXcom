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
#include "MapData.h"

namespace OpenXcom
{

/**
*  TODO: Daiky to organise and document the functions here
*/
MapData::MapData(MapDataSet *dataset) : _dataset(dataset)
{

}

/**
*
*/
MapData::~MapData()
{

}

/// Get the dataset this object belongs to.
MapDataSet *MapData::getDataset()
{
	return _dataset;
}

/**
* get the sprite index
* @param frameID frame 0-7
* @return the original sprite index
*/
int MapData::getSprite(int frameID)
{
	return _sprite[frameID];
}

/**
  * whether this is an animated ufo door
  * @return bool 
  */
bool MapData::isUFODoor()
{
	return _isUfoDoor;
}

/**
  * gets the Y offset for drawing
  * @return int height in pixels
  */
int MapData::getYOffset()
{
	return _yOffset;
}

/**
  * gets the Y offset for drawing
  * @return int height in pixels
  */
SpecialTileType MapData::getSpecialType()
{
	return _specialType;
}

/*
 * Gets the TU cost to walk over the object.
 * @return TU cost
 */
int MapData::getTUCost(MovementType movementType)
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
bool MapData::isNoFloor()
{
	return _isNoFloor;
}

/**
  * whether this is a big wall, which blocks all surrounding paths
  * @return bool 
  */
bool MapData::isBigWall()
{
	if (_terrainLevel < 0) return false; // this is a hack for eg. Skyranger Ramps
	return _isBigWall;
}

/**
  * Add this to the graphical Y offset of units or objects on this tile.
  * @return Y offset
  */
int MapData::getTerrainLevel()
{
	return _terrainLevel;
}

/**
  * Get index to the footstep sound.
  * @return sound ID
  */
int MapData::getFootstepSound()
{
	return _footstepSound;
}

/**
  * whether this is a normal door
  * @return bool 
  */
bool MapData::isDoor()
{
	return _isDoor;
}

/**
  * Get the alternative object ID 
  * @return object ID 
  */
int MapData::getAltMCD()
{
	return _altMCD;
}

/**
  * Get the dead object ID 
  * @return object ID 
  */
int MapData::getDieMCD()
{
	return _dieMCD;
}

/**
  * Get the type of object
  * @return 0-3
  */
int MapData::getObjectType()
{
	return _objectType;
}

/**
  * Get the amount of blockage of a certain type.
  * @param type
  * @return blockage (0-255)
  */
int MapData::getBlock(Affector type)
{
	return _block[(int)type]; 
}

/**
  * Get the lightsource of object.
  * @return lightsource
  */
int MapData::getLightSource()
{
	return _lightSource;
}

/**
* setter
*/
void MapData::setSprite(int frameID, int value)
{
	_sprite[frameID] = value;
}

/**
  * setter
  */
void MapData::setFlags(bool isUfoDoor, bool stopLOS, bool isNoFloor, bool isBigWall, bool isGravLift, bool isDoor, bool blockFire, bool blockSmoke)
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
void MapData::setYOffset(int value)
{
	_yOffset = value;
}

/**
  * setter
  */
void MapData::setSpecialType(int value, int otype)
{
	_specialType = (SpecialTileType)value;
	_objectType = otype;
}

/**
  * setter
  */
void MapData::setTUCosts(int walk, int fly, int slide)
{
	_TUWalk = walk;
	_TUFly = fly;
	_TUSlide = slide;
}

/**
  * setter
  */
void MapData::setTerrainLevel(int value)
{
	_terrainLevel = value;
}

/**
  * setter
  */
void MapData::setFootstepSound(int value)
{
	_footstepSound = value;
}

/**
  * setter
  */
void MapData::setAltMCD(int value)
{
	_altMCD = value;
}

/**
  * setter
  */
void MapData::setDieMCD(int value)
{
	_dieMCD = value;
}

/**
  * setter
  */
void MapData::setBlockValue(int lightBlock, int visionBlock, int HEBlock, int smokeBlock, int fireBlock, int gasBlock)
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
void MapData::setLightSource(int value)
{
	_lightSource = value;
}

}
