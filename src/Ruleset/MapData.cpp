/*
 * Copyright 2010-2013 OpenXcom Developers.
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
*  Creates a new Map Data Object.
* @param dataset The dataset this object belongs to.
*/
MapData::MapData(MapDataSet *dataset) : _dataset(dataset)
{

}

/**
* Destroy the object.
*/
MapData::~MapData()
{

}

/**
* Get the dataset this object belongs to.
* @return Pointer to MapDataSet.
*/
MapDataSet *MapData::getDataset() const
{
	return _dataset;
}

/**
* Get the sprite index.
* @param frameID Animation frame 0-7
* @return the original sprite index
*/
int MapData::getSprite(int frameID) const
{
	return _sprite[frameID];
}

/**
* Set the sprite index for a certain frame.
* @param frameID Animation frame
* @param value The sprite index in the surfaceset of the mapdataset.
*/
void MapData::setSprite(int frameID, int value)
{
	_sprite[frameID] = value;
}

/**
  * Get whether this is an animated ufo door.
  * @return bool
  */
bool MapData::isUFODoor() const
{
	return _isUfoDoor;
}

/**
  * Get whether this is a floor.
  * @return bool
  */
bool MapData::isNoFloor() const
{
	return _isNoFloor;
}

/**
  * Get whether this is a big wall, which blocks all surrounding paths.
  * return value key:
  * 0: not a bigWall
  * 1: regular bigWall
  * 2: allows movement in ne/sw direction
  * 3: allows movement in nw/se direction
  * 4: acts as a west wall
  * 5: acts as a north wall
  * 6: acts as an east wall
  * 7: acts as a south wall
  * 8: acts as a south and east wall.
  * @return an integer representing what kind of bigwall this is.
  */
int MapData::getBigWall() const
{
	if (_terrainLevel < 0) return 0; // this is a hack for eg. Skyranger Ramps
	return _bigWall;
}

/**
  * Get whether this is a normal door.
  * @return bool
  */
bool MapData::isDoor() const
{
	return _isDoor;
}

/**
  * Get whether this is a grav lift.
  * @return bool
  */
bool MapData::isGravLift() const
{
	return _isGravLift;
}

/**
  * Set all kinds of flags.
  * @param isUfoDoor
  * @param stopLOS
  * @param isNoFloor
  * @param bigWall
  * @param isGravLift
  * @param isDoor
  * @param blockFire
  * @param blockSmoke
  */
void MapData::setFlags(bool isUfoDoor, bool stopLOS, bool isNoFloor, int bigWall, bool isGravLift, bool isDoor, bool blockFire, bool blockSmoke)
{
	_isUfoDoor = isUfoDoor;
	_stopLOS = stopLOS;
	_isNoFloor = isNoFloor;
	_bigWall = bigWall;
	_isGravLift = isGravLift;
	_isDoor = isDoor;
	_blockFire = blockFire;
	_blockSmoke = blockSmoke;
}

/**
  * Get the amount of blockage of a certain type.
  * @param type
  * @return blockage (0-255)
  */
int MapData::getBlock(ItemDamageType type) const
{
	switch (type)
	{
	case DT_NONE:
		return _block[1];
	case DT_HE:
		return _block[2];
	case DT_SMOKE:
		return _block[3];
	case DT_IN:
		return _block[4];
	case DT_STUN:
		return _block[5];
	default:
		break;
	}

	return 0;
}

/**
  * Sets the amount of blockage for all types.
  * @param lightBlock
  * @param visionBlock
  * @param HEBlock
  * @param smokeBlock
  * @param fireBlock
  * @param gasBlock
  */
void MapData::setBlockValue(int lightBlock, int visionBlock, int HEBlock, int smokeBlock, int fireBlock, int gasBlock)
{
	_block[0] = lightBlock; // not used...
	_block[1] = visionBlock==1?255:0;
	_block[2] = HEBlock;
	_block[3] = smokeBlock==1?255:0;
	_block[4] = fireBlock==1?255:0;
	_block[5] = gasBlock==1?255:0;
}

/**
  * Get the Y offset for drawing.
  * @return int height in pixels
  */
int MapData::getYOffset() const
{
	return _yOffset;
}

/**
  * Sets the offset on the Y axis for drawing this object.
  * @param value
  */
void MapData::setYOffset(int value)
{
	_yOffset = value;
}

/**
  * Gets the Y offset for drawing.
  * @return int height in pixels
  */
SpecialTileType MapData::getSpecialType() const
{
	return _specialType;
}

/**
  * Get the type of object.
  * @return 0-3
  */
int MapData::getObjectType() const
{
	return _objectType;
}

/**
  * Sets a special tile type and object type.
  * @param value Special tile type.
  * @param otype Object type.
  */
void MapData::setSpecialType(int value, int otype)
{
	_specialType = (SpecialTileType)value;
	_objectType = otype;
}

/*
 * Gets the TU cost to walk over the object.
 * @param movementType
 * @return TU cost
 */
int MapData::getTUCost(MovementType movementType) const
{
	switch (movementType)
	{
	case MT_WALK:
		return _TUWalk;
		break;
	case MT_FLY:
		return _TUFly;
		break;
	case MT_SLIDE:
		return _TUSlide;
		break;
	}
	return 0;
}

/**
  * Set TU cost to move over the object.
  * @param walk
  * @param fly
  * @param slide
  */
void MapData::setTUCosts(int walk, int fly, int slide)
{
	_TUWalk = walk;
	_TUFly = fly;
	_TUSlide = slide;
}

/**
  * Add this to the graphical Y offset of units or objects on this tile.
  * @return Y offset
  */
int MapData::getTerrainLevel() const
{
	return _terrainLevel;
}

/**
  * Sets Y offset for units/objects on this tile.
  * @param value
  */
void MapData::setTerrainLevel(int value)
{
	_terrainLevel = value;
}

/**
  * Get index to the footstep sound.
  * @return sound ID
  */
int MapData::getFootstepSound() const
{
	return _footstepSound;
}

/**
  * Set the index to the footstep sound.
  * @param value
  */
void MapData::setFootstepSound(int value)
{
	_footstepSound = value;
}

/**
  * Get the alternative object ID.
  * @return object ID
  */
int MapData::getAltMCD() const
{
	return _altMCD;
}

/**
  * Set the alternative object ID.
  * @param value
  */
void MapData::setAltMCD(int value)
{
	_altMCD = value;
}

/**
  * Get the dead object ID.
  * @return object ID
  */
int MapData::getDieMCD() const
{
	return _dieMCD;
}

/**
  * Set the dead object ID.
  * @param value
  */
void MapData::setDieMCD(int value)
{
	_dieMCD = value;
}

/**
  * Get the amount of light the object is emitting.
  * @return lightsource
  */
int MapData::getLightSource() const
{
	// lamp posts have 1, but they should emit more light
	if (_lightSource == 1)
		return 15;
	else
		return _lightSource - 1;
}

/**
  * Set the amount of light the object is emitting.
  * @param value
  */
void MapData::setLightSource(int value)
{
	_lightSource = value;
}

/**
  * Get the amount of armor.
  * @return armor
  */
int MapData::getArmor() const
{
	return _armor;
}

/**
  * Set the amount of armor.
  * @param value armor
  */
void MapData::setArmor(int value)
{
	_armor = value;
}

/**
  * Get the amount of flammable.
  * @return armor
  */
int MapData::getFlammable() const
{
	return _flammable;
}

/**
  * Set the amount of flammable.
  * @param value armor
  */
void MapData::setFlammable(int value)
{
	_flammable = value;
}

/**
  * Get the amount of fuel.
  * @return fuel
  */
int MapData::getFuel() const
{
	return _fuel;
}

/**
  * Set the amount of fuel.
  * @param value fuel
  */
void MapData::setFuel(int value)
{
	_fuel = value;
}

/// Get the loft index for a certain layer.
int MapData::getLoftID(int layer) const
{
	return _loftID[layer];
}

/// Set the loft index for a certain layer.
void MapData::setLoftID(int loft, int layer)
{
	_loftID[layer] = loft;
}

/**
  * Get the amount of explosive.
  * @return armor
  */
int MapData::getExplosive() const
{
	return _explosive;
}

/**
  * Set the amount of explosive.
  * @param value armor
  */
void MapData::setExplosive(int value)
{
	_explosive = value;
}

/**
* Set the SCANG.DAT index for minimap.
* @param i the minimap index
*/
void MapData::setMiniMapIndex(unsigned short i)
{
	_miniMapIndex = i;
}

/**
* Get the SCANG.DAT index for minimap.
* @return the minimap index
*/
unsigned short MapData::getMiniMapIndex() const
{
	return _miniMapIndex;
}

/**
* Sets the bigWall value.
* @param value the new bigWall value.
*/
void MapData::setBigWall(const int bigWall)
{
	_bigWall = bigWall;
}

/**
* Sets the TUWalk value.
* @param value the new TUWalk value.
*/
void MapData::setTUWalk(const int TUWalk)
{
	_TUWalk = TUWalk;
}

/**
* Sets the TUFly value.
* @param value the new TUFly value.
*/
void MapData::setTUFly(const int TUFly)
{
	_TUFly = TUFly;
}

/**
* Sets the TUSlide value.
* @param value the new TUSlide value.
*/
void MapData::setTUSlide(const int TUSlide)
{
	_TUSlide = TUSlide;
}
}
