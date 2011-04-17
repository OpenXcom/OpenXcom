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
#ifndef OPENXCOM_TILE_H
#define OPENXCOM_TILE_H

#include <string>
#include <vector>
#include "../Battlescape/Position.h"
#include "../Ruleset/MapData.h"

#define LIGHTLAYERS 3

namespace OpenXcom
{

class Surface;
class MapData;
class BattleUnit;
class BattleItem;

/**
 * Basic element of which a battle map is build.
 * @sa http://www.ufopaedia.org/index.php?title=MAPS
 */
class Tile
{
protected:
	MapData *_objects[4];
	int _currentFrame[4];
	bool _discovered, _checked;
	int _light[LIGHTLAYERS];
	int _lastLight[LIGHTLAYERS];
	int _smoke;
	int _fire;
	int _explosive;
	Position _pos;
	bool _cached;
	BattleUnit *_unit;
	std::vector<BattleItem *> _inventory;
	int _animationOffset;
public:
	/// Creates a tile.
	Tile(const Position& pos);
	/// Cleans up a tile.
	~Tile();
	/// Gets a pointer to the mapdata for a specific part of the tile.
	MapData *getMapData(int part);
	/// Sets the pointer to the mapdata for a specific part of the tile
	void setMapData(MapData *dat, int part);
	/// Gets wether this tile has no objects
	bool isVoid();
	/// Get the TU cost to walk over a certain part of the tile.
	int getTUCost(int part, MovementType movementType);
	/// Checks if this tile has a floor.
	bool hasNoFloor();
	/// Checks if this tile is a big wall.
	bool isBigWall();
	/// Get terrain level.
	int getTerrainLevel();
	/// Gets the tile's position.
	const Position& getPosition() const;
	/// Gets the floor object footstep sound.
	int getFootstepSound();
	/// Open a door, returns the ID, 0(normal), 1(ufo) or -1 if no door opened.
	int openDoor(int part);
	/// Check if ufo door is open.
	bool isUfoDoorOpen(int part);
	/// Set the cached flag.
	void setCached(bool cached);
	/// Check if tile is cached.
	bool isCached();
	/// Sets the black fog of war status of this tile.
	void setDiscovered(bool flag);
	/// Gets the black fog of war status of this tile.
	bool isDiscovered();
	/// Reset light to zero for this tile.
	void resetLight(int layer);
	/// Add light to this tile.
	void addLight(int light, int layer);
	/// Checks if the light on this tile has changed.
	void checkForChangedLight(int layer);
	/// Get the shade amount.
	int getShade();
	/// Destroy a tile part.
	void destroy(int part);
	/// Damage a tile part.
	void damage(int part, int power);
	/// Set a "virtual" explosive on this tile, to detonate later.
	void setExplosive(int power);
	/// Apply the explosive power to the tile parts.
	void detonate();
	/// Animated the tile parts.
	void animate();
	/// Get object sprites.
	Surface *getSprite(int part);
	/// Set a unit on this tile.
	void setUnit(BattleUnit *unit);
	/// Get the unit on this tile.
	BattleUnit *getUnit();
	/// Set fire.
	void setFire(int fire);
	/// Get fire.
	int getFire();
	/// Set smoke.
	void addSmoke(int smoke);
	/// Get smoke.
	int getSmoke();
	/// Get flammability.
	int getFlammability();
	/// Sets the objects on the tile on fire.
	void ignite();
	/// Get fire and smoke animation offset.
	int getAnimationOffset();
	/// Add item
	void addItem(BattleItem *item);
	/// Get top-most item
	int getTopItemSprite();
	/// Decrease fire and smoke timers.
	void prepareNewTurn();
	/// Set whether we checked this tile.
	void setChecked(bool flag);
	/// Get whether we checked this tile.
	bool getChecked();

};

}

#endif
