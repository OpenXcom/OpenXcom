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
#ifndef OPENXCOM_TILEENGINE_H
#define OPENXCOM_TILEENGINE_H

#include <vector>
#include "Position.h"
#include "../Ruleset/MapData.h"
#include <SDL.h>
#include "BattlescapeGame.h"
#include "../Savegame/BattleUnit.h"

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
class TileEngine
{
private:
	static const int MAX_VIEW_DISTANCE = 20;
	static const int MAX_DARKNESS_TO_SEE_UNITS = 9;
	SavedBattleGame *_save;
	std::vector<Uint16> *_voxelData;
	void addLight(const Position &center, int power, int layer);
	int blockage(Tile *tile, const int part, ItemDamageType type);
	int vectorToDirection(const Position &vector);
	int voxelCheck(const Position& voxel, BattleUnit *excludeUnit, bool excludeAllUnits = false);
	bool _personalLighting;
public:
	/// Creates a new TileEngine class.
	TileEngine(SavedBattleGame *save, std::vector<Uint16> *voxelData);
	/// Cleans up the TileEngine.
	~TileEngine();
	/// Calculate sun shading of the whole map.
	void calculateSunShading();
	/// Calculate sun shading of a single tile.
	void calculateSunShading(Tile *tile);
	/// Calculate the field of view from a units view point.
	bool calculateFOV(BattleUnit *unit);
	/// Calculate the field of view within range of a certain position.
	void calculateFOV(const Position &position);
	/// Check reaction fire.
	bool checkReactionFire(BattleUnit *unit, BattleAction *action, BattleUnit *potentialVictim = 0, bool recalculateFOV = true);
	/// Recalculate lighting of the battlescape.
	void calculateTerrainLighting();
	/// Recalculate lighting of the battlescape.
	void calculateUnitLighting();
	/// Explosions.
	BattleUnit *hit(const Position &center, int power, ItemDamageType type, BattleUnit *unit);
	void explode(const Position &center, int power, ItemDamageType type, int maxRadius, BattleUnit *unit = 0);
	/// Check if a destroyed tile starts an explosion.
	Tile *checkForTerrainExplosions();
	/// Unit opens door?
	int unitOpensDoor(BattleUnit *unit);
	/// Close ufo doors.
	int closeUfoDoors();
	/// Calculate line.
	int calculateLine(const Position& origin, const Position& target, bool storeTrajectory, std::vector<Position> *trajectory, BattleUnit *excludeUnit, bool doVoxelCheck = true);
	/// Calculate a parabola trajectory.
	int calculateParabola(const Position& origin, const Position& target, bool storeTrajectory, std::vector<Position> *trajectory, BattleUnit *excludeUnit, double curvature, double accuracy);
	bool visible(BattleUnit *currentUnit, Tile *tile);
	void togglePersonalLighting();
	int distance(const Position &pos1, const Position &pos2) const;
	int horizontalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type);
	int verticalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type);
	bool inTeamFOV(const Position &pos, UnitFaction team);
	bool psiAttack(BattleAction *action);
	Tile *applyItemGravity(Tile *t);
};

}

#endif
