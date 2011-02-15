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
	bool _discovered; // tile has been seen in the past
	int _light[LIGHTLAYERS], _lastLight[LIGHTLAYERS], _smoke, _fire, _sessionID, _explosive;
	Position _pos;
	bool _cached;
	BattleUnit *_unit; // unit on this tile
	std::vector<BattleItem *> _inventory; // items on this tile
	int _animationOffset; //Animation frame offset. Set at random, to break tile pattern in cloud of smoke. Ranges from 0 to 3. 
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
	void setDiscovered(bool flag);
	bool isDiscovered();
	void resetLight(int layer);
	void addLight(int light, int sessionID, int layer);
	void isSeenBy(BattleUnit *unit, int sessionID);
	void setLight(int layer);
	bool isChecked(int sessionID);
	/// Get the shade amount.
	int getShade();
	/// Destroy a tile part.
	void destroy(int part);
	/// Destroy a tile part.
	void setExplosive(int power, int sessionID);
	/// Destroy a tile part.
	void detonate();
	/// Animated the tile parts.
	void animate();
	/// Get surfaces.
	Surface *getSprite(int part);
	void setUnit(BattleUnit *unit);
	BattleUnit *getUnit();
	/// Set fire.
	void setFire(int fire);
	/// Get fire.
	int getFire();
	/// Set smoke.
	void setSmoke(int smoke);
	/// Get smoke.
	int getSmoke();
	/// Get flammability.
	int getFlammability();
	void ignite();
};

}

#endif
