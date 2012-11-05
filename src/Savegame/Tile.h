/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "BattleUnit.h"


namespace OpenXcom
{

class Surface;
class MapData;
class BattleUnit;
class BattleItem;
class RuleInventory;

/**
 * Basic element of which a battle map is build.
 * @sa http://www.ufopaedia.org/index.php?title=MAPS
 */
class Tile
{
protected:
	static const int LIGHTLAYERS = 3;
	MapData *_objects[4];
	int _mapDataID[4];
	int _mapDataSetID[4];
	int _currentFrame[4];
	bool _discovered[3];
	int _light[LIGHTLAYERS], _lastLight[LIGHTLAYERS];
	int _smoke;
	int _fire;
	int _explosive;
	Position _pos;
	BattleUnit *_unit;
	std::vector<BattleItem *> _inventory;
	int _animationOffset;
	int _markerColor;
	int _visible;
public:
	/// Creates a tile.
	Tile(const Position& pos);
	/// Cleans up a tile.
	~Tile();
	/// Load the tile to yaml
	void load(const YAML::Node &node);
	/// Saves the tile to yaml
	void save(YAML::Emitter &out) const;
	/// Gets a pointer to the mapdata for a specific part of the tile.
	MapData *getMapData(int part) const;
	/// Sets the pointer to the mapdata for a specific part of the tile
	void setMapData(MapData *dat, int mapDataID, int mapDataSetID, int part);
	/// Gets the IDs to the mapdata for a specific part of the tile
	void getMapData(int *mapDataID, int *mapDataSetID, int part) const;
	/// Gets whether this tile has no objects
	bool isVoid() const;
	/// Get the TU cost to walk over a certain part of the tile.
	int getTUCost(int part, MovementType movementType) const;
	/// Checks if this tile has a floor.
	bool hasNoFloor() const;
	/// Checks if this tile is a big wall.
	bool isBigWall() const;
	/// Get terrain level.
	int getTerrainLevel() const;
	/// Gets the tile's position.
	const Position& getPosition() const;
	/// Gets the floor object footstep sound.
	int getFootstepSound() const;
	/// Open a door, returns the ID, 0(normal), 1(ufo) or -1 if no door opened.
	int openDoor(int part);
	/// Check if ufo door is open.
	bool isUfoDoorOpen(int part) const;
	/// Close ufo door.
	int closeUfoDoor();
	/// Sets the black fog of war status of this tile.
	void setDiscovered(bool flag, int part);
	/// Gets the black fog of war status of this tile.
	bool isDiscovered(int part) const;
	/// Reset light to zero for this tile.
	void resetLight(int layer);
	/// Add light to this tile.
	void addLight(int light, int layer);
	/// Get the shade amount.
	int getShade() const;
	/// Destroy a tile part.
	void destroy(int part);
	/// Damage a tile part.
	void damage(int part, int power);
	/// Set a "virtual" explosive on this tile, to detonate later.
	void setExplosive(int power);
	/// Get explosive power of this tile.
	int getExplosive() const;
	/// Apply the explosive power to the tile parts.
	void detonate();
	/// Animated the tile parts.
	void animate();
	/// Get object sprites.
	Surface *getSprite(int part) const;
	/// Set a unit on this tile.
	void setUnit(BattleUnit *unit);
	/// Get the (alive) unit on this tile.
	BattleUnit *getUnit() const;
	/// Set fire.
	void setFire(int fire);
	/// Get fire.
	int getFire() const;
	/// Set smoke.
	void addSmoke(int smoke);
	/// Get smoke.
	int getSmoke() const;
	/// Get flammability.
	int getFlammability() const;
	/// Sets the objects on the tile on fire.
	void ignite();
	/// Get fire and smoke animation offset.
	int getAnimationOffset() const;
	/// Add item
	void addItem(BattleItem *item, RuleInventory *ground);
	/// Remove item
	void removeItem(BattleItem *item);
	/// Get top-most item
	int getTopItemSprite();
	/// Decrease fire and smoke timers.
	void prepareNewTurn();
	/// Get inventory on this tile.
	std::vector<BattleItem *> *getInventory();
	/// Set the tile marker color.
	void setMarkerColor(int color);
	/// Get the tile marker color.
	int getMarkerColor();
	/// Set the tile visible flag.
	void setVisible(int visibility);
	/// Get the tile visible flag.
	int getVisible();

};

}

#endif
