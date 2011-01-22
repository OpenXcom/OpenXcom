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
#ifndef OPENXCOM_MAPDATA_H
#define OPENXCOM_MAPDATA_H

#include <string>

namespace OpenXcom
{

class MapDataSet;

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

enum Affector { AFFECT_LIGHT, AFFECT_VISION, AFFECT_HE, AFFECT_SMOKE, AFFECT_FIRE, AFFECT_GAS };

/**
 * MapData is the smallest piece of a Battlescape terrain, holding info about a certain object, wall, floor, ...
 * @sa MapDataSet
 */
class MapData
{
private:
	MapDataSet *_dataset;
	SpecialTileType _specialType;
	bool _isUfoDoor, _stopLOS, _isNoFloor, _isBigWall, _isGravLift, _isDoor, _blockFire, _blockSmoke;
	int _yOffset, _TUWalk, _TUFly, _TUSlide, _terrainLevel, _footstepSound, _dieMCD, _altMCD, _objectType, _lightSource;
	int _sprite[8];
	int _block[6];
public:
	MapData(MapDataSet *dataset);
	~MapData();
	/// Get the dataset this object belongs to.
	MapDataSet *getDataset();
	/// Get the sprite index for a certain frame.
	int getSprite(int frameID);
	/// Set the sprite index for a certain frame.
	void setSprite(int frameID, int value);
	/// Get whether this is an animated ufo door.
	bool isUFODoor();
	/// Can we walk over it.
	bool isNoFloor();
	/// Can we walk over it.
	bool isBigWall();
	/// Is a normal door.
	bool isDoor();
	/// Sets all kinds of flags.
	void setFlags(bool isUfoDoor, bool stopLOS, bool isNoFloor, bool isBigWall, bool isGravLift, bool isDoor, bool blockFire, bool blockSmoke);
	/// Get the amount of blockage of a certain type.
	int getBlock(Affector type);
	/// Sets the amount of blockage for all types.
	void setBlockValue(int lightBlock, int visionBlock, int HEBlock, int smokeBlock, int fireBlock, int gasBlock);
	/// Gets the offset on the Y axis when drawing this object.
	int getYOffset();
	/// Sets the offset on the Y axis for drawing this object.
	void setYOffset(int value);
	/// Gets info about special tile types
	SpecialTileType getSpecialType();
	/// Get the type of tile.
	int getObjectType();
	/// Sets a special tile type and object type.
	void setSpecialType(int value, int otype);
	/// Get TU cost to move over the object.
	int getTUCost(MovementType movementType);
	/// Set TU cost to move over the object.
	void setTUCosts(int walk, int fly, int slide);
	/// Add this to the graphical Y offset of units or objects on this tile.
	int getTerrainLevel();
	/// Sets Y offset for units/objects on this tile.
	void setTerrainLevel(int value);
	/// Get the index to the footstep sound.
	int getFootstepSound();
	/// Set the index to the footstep sound.
	void setFootstepSound(int value);
	/// Get the alternative object ID
	int getAltMCD();
	/// Set the alternative object ID
	void setAltMCD(int value);
	/// Get the dead object ID
	int getDieMCD();
	/// Set the dead object ID
	void setDieMCD(int value);
	/// Get the amount of light the object is emitting.
	int getLightSource();
	/// Set the amount of light the object is emitting.
	void setLightSource(int value);
};

}

#endif
