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
#ifndef OPENXCOM_MAPDATA_H
#define OPENXCOM_MAPDATA_H

#include "RuleItem.h"

namespace OpenXcom
{

class MapDataSet;

enum SpecialTileType{TILE=0,
					START_POINT,
					UFO_POWER_SOURCE,
					UFO_NAVIGATION,
					UFO_CONSTRUCTION,
					ALIEN_FOOD,
					ALIEN_REPRODUCTION,
					ALIEN_ENTERTAINMENT,
					ALIEN_SURGERY,
					EXAM_ROOM,
					ALIEN_ALLOYS,
					ALIEN_HABITAT,
					DEAD_TILE,
					END_POINT,
					MUST_DESTROY};

enum MovementType{ MT_WALK, MT_FLY, MT_SLIDE};

/**
 * MapData is the smallest piece of a Battlescape terrain, holding info about a certain object, wall, floor, ...
 * @sa MapDataSet
 */
class MapData
{
private:
	MapDataSet *_dataset;
	SpecialTileType _specialType;
	bool _isUfoDoor, _stopLOS, _isNoFloor, _isGravLift, _isDoor, _blockFire, _blockSmoke;
	int _yOffset, _TUWalk, _TUFly, _TUSlide, _terrainLevel, _footstepSound, _dieMCD, _altMCD, _objectType, _lightSource;
	int _armor, _flammable, _fuel, _explosive, _bigWall;
	int _sprite[8];
	int _block[6];
	int _loftID[12];
	unsigned short _miniMapIndex;
public:
	static const int O_FLOOR = 0;
	static const int O_WESTWALL = 1;
	static const int O_NORTHWALL = 2;
	static const int O_OBJECT = 3;
	MapData(MapDataSet *dataset);
	~MapData();
	/// Get the dataset this object belongs to.
	MapDataSet *getDataset() const;
	/// Get the sprite index for a certain frame.
	int getSprite(int frameID) const;
	/// Set the sprite index for a certain frame.
	void setSprite(int frameID, int value);
	/// Get whether this is an animated ufo door.
	bool isUFODoor() const;
	/// Can we walk over it.
	bool isNoFloor() const;
	/// Can we walk over it.
	int getBigWall() const;
	/// Is a normal door.
	bool isDoor() const;
	/// Is a grav lift.
	bool isGravLift() const;
	/// Sets all kinds of flags.
	void setFlags(bool isUfoDoor, bool stopLOS, bool isNoFloor, int bigWall, bool isGravLift, bool isDoor, bool blockFire, bool blockSmoke);
	/// Get the amount of blockage of a certain type.
	int getBlock(ItemDamageType type) const;
	/// Sets the amount of blockage for all types.
	void setBlockValue(int lightBlock, int visionBlock, int HEBlock, int smokeBlock, int fireBlock, int gasBlock);
	/// Gets the offset on the Y axis when drawing this object.
	int getYOffset() const;
	/// Sets the offset on the Y axis for drawing this object.
	void setYOffset(int value);
	/// Gets info about special tile types
	SpecialTileType getSpecialType() const;
	/// Get the type of tile.
	int getObjectType() const;
	/// Sets a special tile type and object type.
	void setSpecialType(int value, int otype);
	/// Get TU cost to move over the object.
	int getTUCost(MovementType movementType) const;
	/// Set TU cost to move over the object.
	void setTUCosts(int walk, int fly, int slide);
	/// Add this to the graphical Y offset of units or objects on this tile.
	int getTerrainLevel() const;
	/// Sets Y offset for units/objects on this tile.
	void setTerrainLevel(int value);
	/// Get the index to the footstep sound.
	int getFootstepSound() const;
	/// Set the index to the footstep sound.
	void setFootstepSound(int value);
	/// Get the alternative object ID
	int getAltMCD() const;
	/// Set the alternative object ID
	void setAltMCD(int value);
	/// Get the dead object ID
	int getDieMCD() const;
	/// Set the dead object ID
	void setDieMCD(int value);
	/// Get the amount of light the object is emitting.
	int getLightSource() const;
	/// Set the amount of light the object is emitting.
	void setLightSource(int value);
	/// Get the amount of armor.
	int getArmor() const;
	/// Set the amount of armor.
	void setArmor(int value);
	/// Get the amount of flammable.
	int getFlammable() const;
	/// Set the amount of flammable.
	void setFlammable(int value);
	/// Get the amount of fuel.
	int getFuel() const;
	/// Set the amount of fuel.
	void setFuel(int value);
	/// Get the loft index for a certain layer.
	int getLoftID(int layer) const;
	/// Set the loft index for a certain layer.
	void setLoftID(int loft, int layer);
	/// Get the amount of explosive.
	int getExplosive() const;
	/// Set the amount of explosive.
	void setExplosive(int value);
	/// Set the MiniMap index
	void setMiniMapIndex(unsigned short i);
	/// Get the MiniMap index
	unsigned short getMiniMapIndex() const;
	/// set the bigwall int
	void setBigWall(const int bigWall);
	/// set the TU values individually
	void setTUWalk(const int TUWalk);
	void setTUFly(const int TUFly);
	void setTUSlide(const int TUSlide);
};

}

#endif
