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
#ifndef OPENXCOM_TERRAINMODIFIER_H
#define OPENXCOM_TERRAINMODIFIER_H

#include <vector>
#include "Position.h"
#include "../Ruleset/MapData.h"
#include "SDL.h"

namespace OpenXcom
{

class SavedBattleGame;
class BattleUnit;
class BattleItem;
class Tile;

/**
 * A utility class that modifies tile properties on a battlescape map. This includes lighting, destruction, smoke, fire, fog of war.
 * Note that this function does not handle any sounds or animations.
 */
class TerrainModifier
{
private:
	SavedBattleGame *_save;
	std::vector<Uint16> *_voxelData;
	void addLight(const Position &center, int power, int layer);
	int blockage(Tile *tile, const int part, ItemDamageType type);
	int horizontalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type);
	int verticalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type);
	int vectorToDirection(const Position &vector);
	int voxelCheck(const Position& voxel);
public:
	/// Creates a new TerrainModifier class.
	TerrainModifier(SavedBattleGame *save, std::vector<Uint16> *voxelData);
	/// Cleans up the TerrainModifier.
	~TerrainModifier();
	/// Calculate sun shading of the whole map.
	void calculateSunShading();
	/// Calculate sun shading of a single tile.
	void calculateSunShading(Tile *tile);
	/// Calculate the visible tiles of a unit.
	void calculateFOV(BattleUnit *unit);
	/// Calculate the visible units of a unit.
	bool checkForVisibleUnits(BattleUnit *unit);
	/// Calculate the field of view within range of a certain position.
	void calculateFOV(const Position &position);
	/// Recalculate lighting of the battlescape.
	void calculateTerrainLighting();
	/// Recalculate lighting of the battlescape.
	void calculateUnitLighting();
	/// Explosions.
	void explode(const Position &center, int power, ItemDamageType type, int maxRadius);
	/// Unit opens door?
	int unitOpensDoor(BattleUnit *unit);
	/// Calculate line.
	int calculateLine(const Position& origin, const Position& target, bool storeTrajectory, std::vector<Position> *trajectory);
	/// Add item & affect with gravity.
	void spawnItem(const Position &position, BattleItem *item);
	/// New turn preparations.
	void prepareNewTurn();

};

}

#endif
