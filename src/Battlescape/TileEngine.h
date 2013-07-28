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
	static const int MAX_VOXEL_VIEW_DISTANCE = MAX_VIEW_DISTANCE * 16;
	static const int MAX_DARKNESS_TO_SEE_UNITS = 9;
	SavedBattleGame *_save;
	std::vector<Uint16> *_voxelData;
	static const int heightFromCenter[11];
	void addLight(const Position &center, int power, int layer);
	int blockage(Tile *tile, const int part, ItemDamageType type, int direction = -1);
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
	bool checkReactionFire(BattleUnit *unit);
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
	int unitOpensDoor(BattleUnit *unit, bool rClick = false, int dir = -1);
	/// Close ufo doors.
	int closeUfoDoors();
	/// Calculate line.
	int calculateLine(const Position& origin, const Position& target, bool storeTrajectory, std::vector<Position> *trajectory, BattleUnit *excludeUnit, bool doVoxelCheck = true, bool onlyVisible = false, BattleUnit *excludeAllBut = 0);
	/// Calculate a parabola trajectory.
	int calculateParabola(const Position& origin, const Position& target, bool storeTrajectory, std::vector<Position> *trajectory, BattleUnit *excludeUnit, double curvature, double accuracy);
	/// Find all the soldiers that would see queryingUnit at tile (aka tilePos) and collect some statistics for AI.
	bool surveyXComThreatToTile(Tile *tile, Position &tilePos, BattleUnit *hypotheticalUnit);	
	/// Get the origin voxel of a unit's eyesight
	Position getSightOriginVoxel(BattleUnit *currentUnit);
	/// Check visibility of a unit on this tile
	bool visible(BattleUnit *currentUnit, Tile *tile);
	/// turn xcom soldier's personal lighting on or off
	void togglePersonalLighting();
	/// check the distance between two positions
	int distance(const Position &pos1, const Position &pos2) const;
	/// check the distance squared between two positions
	int distanceSq(const Position &pos1, const Position &pos2, bool considerZ = true) const;	
	/// check the horizontal blockage of a tile
	int horizontalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type);
	/// check the vertical blockage of a tile
	int verticalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type);
	/// can someone on our team see this position?
	bool inTeamFOV(const Position &pos, UnitFaction team);
	/// attempt a panic or mind control action
	bool psiAttack(BattleAction *action);
	/// apply gravity to anything that occupy this tile.
	Tile *applyGravity(Tile *t);
	/// return melee validity between two units
	bool validMeleeRange(BattleUnit *attacker, BattleUnit *target, int dir);
	/// return validity of a melee attack from a given position
	bool validMeleeRange(Position pos, int direction, BattleUnit *attacker, BattleUnit *target);
	/// get the ai to look through a window
	int faceWindow(const Position &position);
	/// get the exposure % of a unit on a tile
	int checkVoxelExposure(Position *originVoxel, Tile *tile, BattleUnit *excludeUnit, BattleUnit *excludeAllBut);
	/// check validity for targetting a unit
	bool canTargetUnit(Position *originVoxel, Tile *tile, Position *scanVoxel, BattleUnit *excludeUnit);
	/// check validity for targetting a tile
	bool canTargetTile(Position *originVoxel, Tile *tile, int part, Position *scanVoxel, BattleUnit *excludeUnit);
	/// calculate te z voxel for shadows
	int castedShade(const Position& voxel);
	/// check the visibility of a given voxel
	bool isVoxelVisible(const Position& voxel);
	/// check what type of voxel occupies this space
	int voxelCheck(const Position& voxel, BattleUnit *excludeUnit, bool excludeAllUnits = false, bool onlyVisible = false, BattleUnit *excludeAllBut = 0);
	/// blow this tile up
	bool detonate(Tile* tile);
	/// validate a throwing action
	bool validateThrow(BattleAction *action);
	/// open any doors this door is connected to.
	void checkAdjacentDoors(Position pos, int part);
	/// create a vector of units that can spot this unit.
	std::vector<BattleUnit *> getSpottingUnits(BattleUnit* unit);
	/// given a vector of spotters, and a unit, this will pick the one with the highest reaction score.
	BattleUnit* getReactor(std::vector<BattleUnit *> spotters, BattleUnit *unit);
	/// check validity of a snap shot to this position.
	bool canMakeSnap(BattleUnit *unit, BattleUnit *target);
	/// try to perform a reaction snap shot to this location.
	bool tryReactionSnap(BattleUnit *unit, BattleUnit *target);
	void recalculateFOV();
};

}

#endif
