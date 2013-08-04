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
#define _USE_MATH_DEFINES
#include <assert.h>
#include <cmath>
#include <climits>
#include <set>
#include <functional>
#include "TileEngine.h"
#include <SDL.h>
#include "BattleAIState.h"
#include "AggroBAIState.h"
#include "UnitTurnBState.h"
#include "Map.h"
#include "Camera.h"
#include "../Savegame/SavedBattleGame.h"
#include "ExplosionBState.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Soldier.h"
#include "../Engine/RNG.h"
#include "BattlescapeState.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/MapData.h"
#include "../Ruleset/Unit.h"
#include "../Ruleset/RuleSoldier.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/Ruleset.h"
#include "../Resource/ResourcePack.h"
#include "Pathfinding.h"
#include "../Engine/Options.h"
#include "ProjectileFlyBState.h"
#include "../Engine/Logger.h"
#include "../aresame.h"

namespace OpenXcom
{

const int TileEngine::heightFromCenter[11] = {0,-2,+2,-4,+4,-6,+6,-8,+8,-12,+12};

/**
 * Sets up a TileEngine.
 * @param save pointer to SavedBattleGame object.
 */
TileEngine::TileEngine(SavedBattleGame *save, std::vector<Uint16> *voxelData) : _save(save), _voxelData(voxelData), _personalLighting(true)
{
}

/**
 * Deletes the TileEngine.
 */
TileEngine::~TileEngine()
{

}


/**
  * Calculate sun shading for the whole terrain.
  */
void TileEngine::calculateSunShading()
{
	const int layer = 0; // Ambient lighting layer.

	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
	{
		_save->getTiles()[i]->resetLight(layer);
		calculateSunShading(_save->getTiles()[i]);
	}
}

/**
  * Calculate sun shading for 1 tile. Sun comes from above and is blocked by floors or objects.
  * @param tile The tile to calculate sun shading for.
  */
void TileEngine::calculateSunShading(Tile *tile)
{
	const int layer = 0; // Ambient lighting layer.

	int power = 15 - _save->getGlobalShade();

	// At night/dusk sun isn't dropping shades blocked by roofs
	if (_save->getGlobalShade() <= 4)
	{
		if (verticalBlockage(_save->getTile(Position(tile->getPosition().x, tile->getPosition().y, _save->getMapSizeZ() - 1)), tile, DT_NONE))
		{
			power -= 2;
		}
	}

	tile->addLight(power, layer);
}

/**
  * Recalculate lighting for the terrain: objects,items,fire.
  */
void TileEngine::calculateTerrainLighting()
{
	const int layer = 1; // Static lighting layer.
	const int fireLightPower = 15; // amount of light a fire generates

	// reset all light to 0 first
	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
	{
		_save->getTiles()[i]->resetLight(layer);
	}

	// add lighting of terrain
	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
	{
		// only floors and objects can light up
		if (_save->getTiles()[i]->getMapData(MapData::O_FLOOR)
			&& _save->getTiles()[i]->getMapData(MapData::O_FLOOR)->getLightSource())
		{
			addLight(_save->getTiles()[i]->getPosition(), _save->getTiles()[i]->getMapData(MapData::O_FLOOR)->getLightSource(), layer);
		}
		if (_save->getTiles()[i]->getMapData(MapData::O_OBJECT)
			&& _save->getTiles()[i]->getMapData(MapData::O_OBJECT)->getLightSource())
		{
			addLight(_save->getTiles()[i]->getPosition(), _save->getTiles()[i]->getMapData(MapData::O_OBJECT)->getLightSource(), layer);
		}

		// fires
		if (_save->getTiles()[i]->getFire())
		{
			addLight(_save->getTiles()[i]->getPosition(), fireLightPower, layer);
		}

		for (std::vector<BattleItem*>::iterator it = _save->getTiles()[i]->getInventory()->begin(); it != _save->getTiles()[i]->getInventory()->end(); ++it)
		{
			if ((*it)->getRules()->getBattleType() == BT_FLARE)
			{
				addLight(_save->getTiles()[i]->getPosition(), (*it)->getRules()->getPower(), layer);
			}
		}

	}

}

/**
  * Recalculate lighting for the units.
  */
void TileEngine::calculateUnitLighting()
{
	const int layer = 2; // Dynamic lighting layer.
	const int personalLightPower = 15; // amount of light a unit generates
	const int fireLightPower = 15; // amount of light a fire generates

	// reset all light to 0 first
	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
	{
		_save->getTiles()[i]->resetLight(layer);
	}

	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		// add lighting of soldiers
		if (_personalLighting && (*i)->getFaction() == FACTION_PLAYER && !(*i)->isOut())
		{
			addLight((*i)->getPosition(), personalLightPower, layer);
		}
		// add lighting of units on fire
		if ((*i)->getFire())
		{
			addLight((*i)->getPosition(), fireLightPower, layer);
		}
	}
}

/**
 * Adds circular light pattern starting from center and losing power with distance travelled.
 * @param center
 * @param power
 * @param layer Light is seperated in 3 layers: Ambient, Static and Dynamic.
 */
void TileEngine::addLight(const Position &center, int power, int layer)
{
	// only loop through the positive quadrant.
	for (int x = 0; x <= power; ++x)
	{
		for (int y = 0; y <= power; ++y)
		{
			for (int z = 0; z < _save->getMapSizeZ(); z++)
			{
				int distance = int(floor(sqrt(float(x*x + y*y)) + 0.5));

				if (_save->getTile(Position(center.x + x,center.y + y, z)))
					_save->getTile(Position(center.x + x,center.y + y, z))->addLight(power - distance, layer);

				if (_save->getTile(Position(center.x - x,center.y - y, z)))
					_save->getTile(Position(center.x - x,center.y - y, z))->addLight(power - distance, layer);

				if (_save->getTile(Position(center.x - x,center.y + y, z)))
					_save->getTile(Position(center.x - x,center.y + y, z))->addLight(power - distance, layer);

				if (_save->getTile(Position(center.x + x,center.y - y, z)))
					_save->getTile(Position(center.x + x,center.y - y, z))->addLight(power - distance, layer);
			}
		}
	}
}

/**
 * Calculates line of sight of a soldier.
 * @param unit
 * @return true when new aliens spotted
 */
bool TileEngine::calculateFOV(BattleUnit *unit)
{
	size_t oldNumVisibleUnits = unit->getUnitsSpottedThisTurn().size();
	Position center = unit->getPosition();
	Position test;
	int direction;
	bool swap;
	std::vector<Position> _trajectory;
	if (_save->getStrafeSetting() && (unit->getTurretType() > -1)) {
		direction = unit->getTurretDirection();
	}
	else
	{
		direction = unit->getDirection();
	}
	swap = (direction==0 || direction==4);
	int signX[8] = { +1, +1, +1, +1, -1, -1, -1, -1 };
	int signY[8] = { -1, -1, -1, +1, +1, +1, -1, -1 };
	int y1, y2;

	unit->clearVisibleUnits();
	unit->clearVisibleTiles();

	if (unit->isOut())
		return false;
	Position pos = unit->getPosition();

	if ((unit->getHeight() + unit->getFloatHeight() + -_save->getTile(unit->getPosition())->getTerrainLevel()) >= 24 + 4)
	{
		++pos.z;
	}
	for (int x = 0; x <= MAX_VIEW_DISTANCE; ++x)
	{
		if (direction%2)
		{
			y1 = 0;
			y2 = MAX_VIEW_DISTANCE;
		}
		else
		{
			y1 = -x;
			y2 = x;
		}
		for (int y = y1; y <= y2; ++y)
		{
			for (int z = 0; z < _save->getMapSizeZ(); z++)
			{
				const int distanceSqr = x*x + y*y;
				test.z = z;
				if (distanceSqr <= MAX_VIEW_DISTANCE*MAX_VIEW_DISTANCE)
				{
					test.x = center.x + signX[direction]*(swap?y:x);
					test.y = center.y + signY[direction]*(swap?x:y);
					if (_save->getTile(test))
					{
						BattleUnit *visibleUnit = _save->getTile(test)->getUnit();
						if (visibleUnit && !visibleUnit->isOut() && visible(unit, _save->getTile(test)))
						{
							if (unit->getFaction() == FACTION_PLAYER)
							{
								visibleUnit->getTile()->setVisible(+1);
								visibleUnit->setVisible(true);
							}
							if ((visibleUnit->getFaction() == FACTION_HOSTILE && unit->getFaction() != FACTION_HOSTILE)
								|| (visibleUnit->getFaction() != FACTION_HOSTILE && unit->getFaction() == FACTION_HOSTILE))
							{
								unit->addToVisibleUnits(visibleUnit);
								unit->addToVisibleTiles(visibleUnit->getTile());

								if (unit->getFaction() == FACTION_HOSTILE && visibleUnit->getFaction() == FACTION_PLAYER)
								{
									visibleUnit->setTurnsExposed(0);
								}
							}
						}

						if (unit->getFaction() == FACTION_PLAYER)
						{
							// this sets tiles to discovered if they are in LOS - tile visibility is not calculated in voxelspace but in tilespace
							// large units have "4 pair of eyes"
							int size = unit->getArmor()->getSize();
							for (int xo = 0; xo < size; xo++)
							{
								for (int yo = 0; yo < size; yo++)
								{
									Position poso = pos + Position(xo,yo,0);
									_trajectory.clear();
									int tst = calculateLine(poso, test, true, &_trajectory, unit, false);
									unsigned int tsize = _trajectory.size();
									if (tst>127) --tsize; //last tile is blocked thus must be cropped
									for (unsigned int i = 0; i < tsize; i++)
									{
										Position posi = _trajectory.at(i); 
										//mark every tile of line as visible (as in original)
										//this is needed because of bresenham narrow stroke. 
										_save->getTile(posi)->setVisible(+1);
										_save->getTile(posi)->setDiscovered(true, 2);
										// walls to the east or south of a visible tile, we see that too
										Tile* t = _save->getTile(Position(posi.x + 1, posi.y, posi.z));
										if (t) t->setDiscovered(true, 0);
										t = _save->getTile(Position(posi.x, posi.y + 1, posi.z));
										if (t) t->setDiscovered(true, 1);
									}

								}
							}
						}
					}
				}
			}
		}
	}

	// we only react when there are at least the same amount of visible units as before AND the checksum is different
	// this way we stop if there are the same amount of visible units, but a different unit is seen
	// or we stop if there are more visible units seen
	if (unit->getUnitsSpottedThisTurn().size() > oldNumVisibleUnits && unit->getVisibleUnits()->size() > 0)
	{
		// a hostile unit will aggro on the new unit if it sees one - it will not start walking
		if (unit->getFaction() == FACTION_HOSTILE)
		{
			AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(unit->getCurrentAIState());
			if (aggro == 0)
			{
				aggro = new AggroBAIState(_save, unit);
				unit->setAIState(aggro);
				unit->_hidingForTurn = false; // something new happened, react at will...
			}
			aggro->setAggroTarget(unit->getVisibleUnits()->at(0)); // just pick the first one - maybe we need to prioritize on distance to unit or other parameters?
		}

		return true;
	}

	return false;

}



/**
 * @brief Find all the soldiers that would see queryingUnit at tile (aka tilePos) and collect some statistics for AI.
 * @param tile the tile to check
 * @param tilePos the position of the tile to check, redundantly
 * @param queryingUnit the unit to temporarily place at tilePos for calculations
 * @return false if the unit couldn't possibly be placed at tile (i.e., something's blocking it), true otherwise
 */
bool TileEngine::surveyXComThreatToTile(Tile *tile, Position &tilePos, BattleUnit *queryingUnit)
{
	if (tile->soldiersVisible != -1) return true; // already calculated this turn
	
	if (!_save->setUnitPosition(queryingUnit, tilePos, true)) 
	{
		return false;
	}

	tile->soldiersVisible = 0; // we're actually not updating the other three tiles of a 2x2 unit because the AI code is going to ignore them anyway for now
	tile->closestSoldierDSqr = INT_MAX;
	tile->closestAlienDSqr = INT_MAX;
	
	int dsqrTotal = 0;
	
	Position targetVoxel = tile->getPosition() * Position(16, 16, 24) + Position(8, 8, 8 - tile->getTerrainLevel());
	
	for (std::vector<BattleUnit*>::const_iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if ((*i)->isOut()) continue;
		
		int dsqr = distanceSq(tilePos, (*i)->getPosition());
		if (dsqr < 1) dsqr = 1; // sanity buffer against dividing by 0

		if (dsqr > (MAX_VIEW_DISTANCE * MAX_VIEW_DISTANCE)) continue; // can't even see that far, yo
		
		Position originVoxel = getSightOriginVoxel(*i);
		int target = calculateLine(originVoxel, targetVoxel, false, 0, *i, true, false, *i);
		if ((*i)->getFaction() == FACTION_PLAYER && target == -1)
		{
			++tile->soldiersVisible;

			if (dsqr < tile->closestSoldierDSqr)
			{
				tile->closestSoldierDSqr = dsqr;
			}
			
			dsqrTotal += dsqr;
		}

		if ((*i)->getFaction() == FACTION_HOSTILE && dsqr < tile->closestAlienDSqr) tile->closestAlienDSqr = dsqr;
	}

	if (tile->soldiersVisible == 0)
	{
		tile->closestSoldierDSqr = -1; 
	}
	
	return true;
}

/**
 * Get the origin voxel of a unit's eyesight (from just one eye or something? Why is it x+7??
 * @param currentUnit the watcher
 * @return approximately an eyeball voxel
 */
Position TileEngine::getSightOriginVoxel(BattleUnit *currentUnit)
{
	// determine the origin and target voxels for the raytrace
	Position originVoxel;
	originVoxel = Position((currentUnit->getPosition().x * 16) + 7, (currentUnit->getPosition().y * 16) + 8, currentUnit->getPosition().z*24);
	originVoxel.z += -_save->getTile(currentUnit->getPosition())->getTerrainLevel();
	originVoxel.z += currentUnit->getHeight() + currentUnit->getFloatHeight() - 1; //one voxel lower (eye level)
	Tile *tileAbove = _save->getTile(currentUnit->getPosition() + Position(0,0,1));
	if (currentUnit->getArmor()->getSize() > 1)
	{
		originVoxel.x += 8;
		originVoxel.y += 8;
		originVoxel.z += 1; //topmost voxel
	}
	if (originVoxel.z >= (currentUnit->getPosition().z + 1)*24 && (!tileAbove || !tileAbove->hasNoFloor(0)))
	{
		while (originVoxel.z >= (currentUnit->getPosition().z + 1)*24)
		{
			originVoxel.z--;
		}
	}

	return originVoxel;
}



/**
 * Check for an opposing unit on this tile
 * @param currentUnit the watcher
 * @param tile the tile to check for
 * @return true/false
 */
bool TileEngine::visible(BattleUnit *currentUnit, Tile *tile)
{
	// if there is no tile or no unit, we can't see it
	if (!tile || !tile->getUnit())
	{
		return false;
	}

	// aliens can see in the dark, xcom can see at a distance of 9 or less, further if there's enough light.
	if (currentUnit->getFaction() == FACTION_PLAYER &&
		distance(currentUnit->getPosition(), tile->getPosition()) > 9 &&
		tile->getShade() > MAX_DARKNESS_TO_SEE_UNITS)
	{
		return false;
	}

	if (currentUnit->getFaction() == tile->getUnit()->getFaction()) return true; //friendlies are always seen

	Position originVoxel = getSightOriginVoxel(currentUnit);

	bool unitSeen = false;
	// for large units origin voxel is in the middle

	Position scanVoxel;
	std::vector<Position> _trajectory;
	unitSeen = canTargetUnit(&originVoxel, tile, &scanVoxel, currentUnit);

	if (unitSeen)
	{
		// now check if we really see it taking into account smoke tiles
		// initial smoke "density" of a smoke grenade is around 15 per tile
		// we do density/3 to get the decay of visibility
		// so in fresh smoke we should only have 4 tiles of visibility
		// this is traced in voxel space, with smoke affecting visibility every step of the way
		_trajectory.clear();
		calculateLine(originVoxel, scanVoxel, true, &_trajectory, currentUnit);
		Tile *t = _save->getTile(currentUnit->getPosition());
		int visibleDistance = _trajectory.size();
		for (unsigned int i = 0; i < _trajectory.size(); i++)
		{
			if (t != _save->getTile(Position(_trajectory.at(i).x/16,_trajectory.at(i).y/16, _trajectory.at(i).z/24)))
			{
				t = _save->getTile(Position(_trajectory.at(i).x/16,_trajectory.at(i).y/16, _trajectory.at(i).z/24));
			}
			if (t->getFire() == 0)
			{
				visibleDistance += t->getSmoke() / 3;
			}
			if (visibleDistance > MAX_VOXEL_VIEW_DISTANCE)
			{
				unitSeen = false;
				break;
			}
		}
	}
	return unitSeen;
}


/**
 * Check for how exposed unit for another unit
 * @param originVoxel voxel of trace origin (eye or gun's barrel)
 * @param tile the tile to check for
 * @param excludeUnit is self (not to hit self)
 * @param excludeAllBut [optional] is unit which is the only one to be considered for ray hits
 * @return degree of exposure (as percent)
 */
int TileEngine::checkVoxelExposure(Position *originVoxel, Tile *tile, BattleUnit *excludeUnit, BattleUnit *excludeAllBut)
{
	Position targetVoxel = Position((tile->getPosition().x * 16) + 7, (tile->getPosition().y * 16) + 8, tile->getPosition().z * 24);
	Position scanVoxel;
	std::vector<Position> _trajectory;
	BattleUnit *otherUnit = tile->getUnit();
	if (otherUnit == 0) return 0; //no unit in this tile, even if it elevated and appearing in it.
	if (otherUnit == excludeUnit) return 0; //skip self

	int targetMinHeight = targetVoxel.z - tile->getTerrainLevel();
	if (otherUnit)
		 targetMinHeight += otherUnit->getFloatHeight();

	// if there is an other unit on target tile, we assume we want to check against this unit's height
	int heightRange;
 
	int unitRadius = otherUnit->getLoftemps(); //width == loft in default loftemps set
	if (otherUnit->getArmor()->getSize() > 1)
	{
		unitRadius = 3;
	}

	// vector manipulation to make scan work in view-space
	Position relPos = targetVoxel - *originVoxel;
	float normal = unitRadius/sqrt((float)(relPos.x*relPos.x + relPos.y*relPos.y));
	int relX = floor(((float)relPos.y)*normal+0.5);
	int relY = floor(((float)-relPos.x)*normal+0.5);

	int sliceTargets[10]={0,0, relX,relY, -relX,-relY};
 
	if (!otherUnit->isOut())
	{
		heightRange = otherUnit->getHeight();
	}
	else
	{
		heightRange = 12;
	}


	int targetMaxHeight=targetMinHeight+heightRange;
	// scan ray from top to bottom  plus different parts of target cylinder
	int total=0;
	int visible=0;
	for (int i = heightRange; i >=0; i-=2)
	{
		++total;
		scanVoxel.z=targetMinHeight+i;
		for (int j = 0; j < 2; ++j)
		{
			scanVoxel.x=targetVoxel.x + sliceTargets[j*2];
			scanVoxel.y=targetVoxel.y + sliceTargets[j*2+1];
			_trajectory.clear();
			int test = calculateLine(*originVoxel, scanVoxel, false, &_trajectory, excludeUnit, true, false, excludeAllBut);
			if (test == 4)
			{
				//voxel of hit must be inside of scanned box
				if (_trajectory.at(0).x/16 == scanVoxel.x/16 &&
					_trajectory.at(0).y/16 == scanVoxel.y/16 &&
					_trajectory.at(0).z >= targetMinHeight &&
					_trajectory.at(0).z <= targetMaxHeight)
				{
					++visible;
				}
			}
		}
	}
	return (visible*100)/total;
}


/**
 * Check for an another unit is available for targeting and what particular voxel
 * @param originVoxel voxel of trace origin (eye or gun's barrel)
 * @param tile the tile to check for
 * @param scanVoxel is returned coordinate of hit
 * @param excludeUnit is self (not to hit self)
 * @return true/false
 */
bool TileEngine::canTargetUnit(Position *originVoxel, Tile *tile, Position *scanVoxel, BattleUnit *excludeUnit)
{
	Position targetVoxel = Position((tile->getPosition().x * 16) + 7, (tile->getPosition().y * 16) + 8, tile->getPosition().z * 24);
	std::vector<Position> _trajectory;
	BattleUnit *otherUnit = tile->getUnit();
	if (otherUnit == 0) return false; //no unit in this tile, even if it elevated and appearing in it.
	if (otherUnit == excludeUnit) return false; //skip self

	int targetMinHeight = targetVoxel.z - tile->getTerrainLevel();
	if (otherUnit)
		 targetMinHeight += otherUnit->getFloatHeight();

	int targetMaxHeight = targetMinHeight;
	int targetCenterHeight;
	// if there is an other unit on target tile, we assume we want to check against this unit's height
	int heightRange;
 
	int unitRadius = otherUnit->getLoftemps(); //width == loft in default loftemps set
	int targetSize = otherUnit->getArmor()->getSize() - 1;
	if (targetSize > 0)
	{
		unitRadius = 3;
	}

	// vector manipulation to make scan work in view-space
	Position relPos = targetVoxel - *originVoxel;
	float normal = unitRadius/sqrt((float)(relPos.x*relPos.x + relPos.y*relPos.y));
	int relX = floor(((float)relPos.y)*normal+0.5);
	int relY = floor(((float)-relPos.x)*normal+0.5);

	int sliceTargets[10]={0,0, relX,relY, -relX,-relY, relY,-relX, -relY,relX};
 
	if (!otherUnit->isOut())
	{
		heightRange = otherUnit->getHeight();
	}
	else
	{
		heightRange = 12;
	}

	targetMaxHeight += heightRange;
	targetCenterHeight=(targetMaxHeight+targetMinHeight)/2;
	heightRange/=2;
	if (heightRange>10) heightRange=10;
	if (heightRange<=0) heightRange=0;

	// scan ray from top to bottom  plus different parts of target cylinder
	for (int i = 0; i <= heightRange; ++i)
	{
		scanVoxel->z=targetCenterHeight+heightFromCenter[i];
		for (int j = 0; j < 5; ++j)
		{
			if (i < (heightRange-1) && j>2) break; //skip unnecessary checks
			scanVoxel->x=targetVoxel.x + sliceTargets[j*2];
			scanVoxel->y=targetVoxel.y + sliceTargets[j*2+1];
			_trajectory.clear();
			int test = calculateLine(*originVoxel, *scanVoxel, false, &_trajectory, excludeUnit, true);
			if (test == 4)
			{
				for (int x = 0; x <= targetSize; ++x)
				{
					for (int y = 0; y <= targetSize; ++y)
					{
						//voxel of hit must be inside of scanned box
						if (_trajectory.at(0).x/16 == (scanVoxel->x/16) + x &&
							_trajectory.at(0).y/16 == (scanVoxel->y/16) + y &&
							_trajectory.at(0).z >= targetMinHeight &&
							_trajectory.at(0).z <= targetMaxHeight)
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

/**
 * Check for a tile part available for targeting and what particular voxel
 * @param originVoxel voxel of trace origin (gun's barrel)
 * @param tile the tile to check for
 * @param part tile part to check for
 * @param scanVoxel is returned coordinate of hit
 * @param excludeUnit is self (not to hit self)
 * @return true/false
 */
bool TileEngine::canTargetTile(Position *originVoxel, Tile *tile, int part, Position *scanVoxel, BattleUnit *excludeUnit)
{
	static int sliceObjectSpiral[82] = {8,8, 8,6, 10,6, 10,8, 10,10, 8,10, 6,10, 6,8, 6,6, //first circle
		8,4, 10,4, 12,4, 12,6, 12,8, 12,10, 12,12, 10,12, 8,12, 6,12, 4,12, 4,10, 4,8, 4,6, 4,4, 6,4, //second circle
		8,1, 12,1, 15,1, 15,4, 15,8, 15,12, 15,15, 12,15, 8,15, 4,15, 1,15, 1,12, 1,8, 1,4, 1,1, 4,1}; //third circle
	static int westWallSpiral[14] = {0,7, 0,9, 0,6, 0,11, 0,4, 0,13, 0,2};
	static int northWallSpiral[14] = {7,0, 9,0, 6,0, 11,0, 4,0, 13,0, 2,0};

	Position targetVoxel = Position((tile->getPosition().x * 16), (tile->getPosition().y * 16), tile->getPosition().z * 24);
	std::vector<Position> _trajectory;
	
	int *spiralArray;
	int spiralCount;

	int minZ, maxZ;
	bool minZfound = false, maxZfound = false;

	if (part == MapData::O_OBJECT)
	{
		spiralArray = sliceObjectSpiral;
		spiralCount = 41;
	}
	else
	if (part == MapData::O_NORTHWALL)
	{
		spiralArray = northWallSpiral;
		spiralCount = 7;
	}
	else
	if (part == MapData::O_WESTWALL)
	{
		spiralArray = westWallSpiral;
		spiralCount = 7;
	}
	else if (part == MapData::O_FLOOR)
	{
		spiralArray = sliceObjectSpiral;
		spiralCount = 41;
		minZfound = true; minZ=0;
		maxZfound = true; maxZ=0;
	}
	else
	{
		return false;
	}

// find out height range

	if (!minZfound)
	{
		for (int j = 1; j < 12; ++j)
		{
			if (minZfound) break;
			for (int i = 0; i < spiralCount; ++i)
			{
				int tX = spiralArray[i*2];
				int tY = spiralArray[i*2+1];
				if (voxelCheck(Position(targetVoxel.x + tX, targetVoxel.y + tY, targetVoxel.z + j*2),0,true) == part) //bingo
				{
					if (!minZfound)
					{
						minZ = j*2;
						minZfound = true;
						break;
					}
				}
			}
		}
	}

	if (!minZfound) return false;//empty object!!!

	if (!maxZfound)
	{
		for (int j = 10; j >= 0; --j)
		{
			if (maxZfound) break;
			for (int i = 0; i < spiralCount; ++i)
			{
				int tX = spiralArray[i*2];
				int tY = spiralArray[i*2+1];
				if (voxelCheck(Position(targetVoxel.x + tX, targetVoxel.y + tY, targetVoxel.z + j*2),0,true) == part) //bingo
				{
					if (!maxZfound)
					{
						maxZ = j*2;
						maxZfound = true;
						break;
					}
				}
			}
		}
	}

	if (!maxZfound) return false;//it's impossible to get there

	if (minZ > maxZ) minZ = maxZ;
	int rangeZ = maxZ - minZ;
	int centerZ = (maxZ + minZ)/2;

	for (int j = 0; j <= rangeZ; ++j)
	{
		scanVoxel->z = targetVoxel.z + centerZ + heightFromCenter[j];
		for (int i = 0; i < spiralCount; ++i)
		{
			scanVoxel->x = targetVoxel.x + spiralArray[i*2];
			scanVoxel->y = targetVoxel.y + spiralArray[i*2+1];
			_trajectory.clear();
			int test = calculateLine(*originVoxel, *scanVoxel, false, &_trajectory, excludeUnit, true);
			if (test == part) //bingo
			{
				if (_trajectory.at(0).x/16 == scanVoxel->x/16 &&
					_trajectory.at(0).y/16 == scanVoxel->y/16 &&
					_trajectory.at(0).z/24 == scanVoxel->z/24)
				{
					return true;
				}
			}
		}
	}
	return false;
}


/**
 * Calculates line of sight of a soldiers within range of the Position
 * (used when terrain has changed, which can reveal new parts of terrain or units)
 * @param position Position of the changed terrain.
 */
void TileEngine::calculateFOV(const Position &position)
{
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if (distance(position, (*i)->getPosition()) < MAX_VIEW_DISTANCE)
		{
			calculateFOV(*i);
		}
	}
}

/**
 * Checks if of the opposing faction a sniper sees this unit. The unit with the highest reaction score will be compared with the current unit's reaction score.
 * If it's higher, a shot is fired when enough time units a weapon and ammo available.
 * @param unit the unit to check reaction fire upon
 * @return whether or not reaction fire took place.
 */
bool TileEngine::checkReactionFire(BattleUnit *unit)
{
	// reaction fire only triggered when the actioning unit is of the currently playing side
	if (unit->getFaction() != _save->getSide())
	{
		return false;
	}

	std::vector<BattleUnit *> spotters = getSpottingUnits(unit);
	bool result = false;

	// not mind controlled, or controlled by the player
	if (unit->getFaction() == unit->getOriginalFaction()
		|| unit->getFaction() != FACTION_HOSTILE)
	{
		BattleUnit *reactor = getReactor(spotters, unit);
		if (reactor != unit)
		{
			while (true)
			{
				if (!tryReactionSnap(reactor, unit))
					break;
				reactor = getReactor(spotters, unit);
				result = true;
				if (reactor == unit)
					break;
			}
		}
	}
	return result;
}

/*
 * create a vector of units that can spot this unit.
 * @param unit the unit to check for spotters of
 * @return a vector of units that can see this unit.
 */
std::vector<BattleUnit *> TileEngine::getSpottingUnits(BattleUnit* unit)
{
	std::vector<BattleUnit*> spotters;
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
			// not dead/unconscious
		if (!(*i)->isOut() &&
			// not a friend
			(*i)->getFaction() != _save->getSide() &&
			// closer than 20 tiles
			distance(unit->getPosition(), (*i)->getPosition()) <= MAX_VIEW_DISTANCE)
		{
			AggroBAIState *aggro = dynamic_cast<AggroBAIState*>((*i)->getCurrentAIState());
			bool gotHit = (aggro != 0 && aggro->getWasHit());
				// can actually see the target Tile, or we got hit
			if (((*i)->checkViewSector(unit->getPosition()) || gotHit) &&
				// can actually see the unit
				visible(*i, unit->getTile()))
			{
				if ((*i)->getFaction() == FACTION_PLAYER)
				{
					unit->setVisible(true);
				}
				(*i)->addToVisibleUnits(unit);
				// no reaction on civilian turn.
				if (_save->getSide() != FACTION_NEUTRAL &&
					canMakeSnap(*i, unit))
				{
					spotters.push_back(*i);
				}
			}
		}
	}
	return spotters;
}

/*
 * get the unit with the highest reaction score from the spotter vector.
 * @param spotters the vector of spotting units.
 * @param unit the unit to check scores against.
 * @return the unit with the highest reactions.
 */
BattleUnit* TileEngine::getReactor(std::vector<BattleUnit *> spotters, BattleUnit *unit)
{
	int bestScore = -1;
	BattleUnit *bu = 0;
	for (std::vector<BattleUnit *>::iterator i = spotters.begin(); i != spotters.end(); ++i)
	{
		if (!(*i)->isOut() &&
		canMakeSnap(*i, unit) &&
		(*i)->getReactionScore() > bestScore)
		{
			bestScore = (*i)->getReactionScore();
			bu = *i;
		}
	}
	if (unit->getReactionScore() <= bestScore)
	{
		if (bu->getOriginalFaction() == FACTION_PLAYER)
		{
			bu->addReactionExp();
		}
	}
	else
	{
		bu = unit;
	}
	return bu;
}

/*
 * check the validity of a snap shot performed here.
 * @param unit the unit to check sight from
 * @param target the unit to check sight TO
 * @return if the target is valid
 */
bool TileEngine::canMakeSnap(BattleUnit *unit, BattleUnit *target)
{
	BattleItem *weapon = unit->getMainHandWeapon();
	// has a weapon
	if (weapon && 
		// has a melee weapon and is in melee range
		((weapon->getRules()->getBattleType() == BT_MELEE &&
		validMeleeRange(unit, target, unit->getDirection()) &&
		unit->getTimeUnits() > unit->getActionTUs(BA_HIT, weapon)) ||
		// has a gun capable of snap shot with ammo
		(weapon->getRules()->getBattleType() != BT_MELEE &&
		weapon->getRules()->getTUSnap() &&
		weapon->getAmmoItem() &&
		unit->getTimeUnits() > unit->getActionTUs(BA_SNAPSHOT, weapon))))
	{
		Position origin = getSightOriginVoxel(unit);
		Position scanVoxel;
		if (canTargetUnit(&origin, target->getTile(), &scanVoxel, unit))
		{
			return true;
		}
	}
	return false;
}

/*
 * attempt to perform a reaction snap shot.
 * @param unit the unit to check sight from
 * @param target the unit to check sight TO
 * @return if the action should (theoretically) succeed
 */
bool TileEngine::tryReactionSnap(BattleUnit *unit, BattleUnit *target)
{
	BattleAction action;
	action.cameraPosition = _save->getBattleState()->getMap()->getCamera()->getMapOffset();
	action.actor = unit;
	action.weapon = unit->getMainHandWeapon();
	// reaction fire is ALWAYS snap shot.
	action.type = BA_SNAPSHOT;
	// unless we're a melee unit.
	if (action.weapon->getRules()->getBattleType() == BT_MELEE)
	{
		action.type = BA_HIT;
	}
	action.target = target->getPosition();
	action.TU = unit->getActionTUs(action.type, action.weapon);

	if (action.weapon && action.weapon->getAmmoItem() && action.weapon->getAmmoItem()->getAmmoQuantity() && unit->getTimeUnits() >= action.TU)
	{
		action.targeting = true;

		// hostile units will go into an "aggro" state when they react.
		if (unit->getFaction() == FACTION_HOSTILE)
		{
			AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(unit->getCurrentAIState());
			if (aggro == 0)
			{
				aggro = new AggroBAIState(_save, unit);
				unit->setAIState(aggro);
			}
			if (!target->isOut())
			{
				aggro->setAggroTarget(target);
			}

			if (action.weapon->getAmmoItem()->getRules()->getExplosionRadius() &&
				aggro->explosiveEfficacy(action.target, unit, action.weapon->getAmmoItem()->getRules()->getExplosionRadius(), -1) == false)
			{
				// don't shoot. it's too early in the game or we'll kill ourselves or someone we care about
				// this will cause the alien to NOT actually fire, but allow the loop to continue in case someone else CAN.
				// the unit won't get it's time units back, so it won't react again this turn
				action.targeting = false;
			}
		}

		if (unit->spendTimeUnits(unit->getActionTUs(action.type, action.weapon)) && action.targeting)
		{
			action.TU = 0;
			_save->getBattleState()->getBattleGame()->statePushBack(new UnitTurnBState(_save->getBattleState()->getBattleGame(), action));
			_save->getBattleState()->getBattleGame()->statePushBack(new ProjectileFlyBState(_save->getBattleState()->getBattleGame(), action));
		}
		return true;
	}
	return false;
}

/**
 * A bullet/weapon hits a voxel.
 * @param center Center of the explosion in voxelspace.
 * @param power Power of the explosion.
 * @param type The damage type of the explosion.
 * @param unit The unit that caused the explosion.
 * @return The Unit that got hit
 */
BattleUnit *TileEngine::hit(const Position &center, int power, ItemDamageType type, BattleUnit *unit)
{
	Tile *tile = _save->getTile(Position(center.x/16, center.y/16, center.z/24));
	BattleUnit *bu = tile->getUnit();
	int adjustedDamage = 0;
	const int part = voxelCheck(center, unit);
	if (part >= 0 && part <= 3)
	{
		// power 25% to 75%
		const int rndPower = RNG::generate(power/4, (power*3)/4); //RNG::boxMuller(power, power/6)
		if (tile->damage(part, rndPower))
			_save->setObjectiveDestroyed(true);
	}
	else if (part == 4)
	{
		// power 0 - 200%
		const int rndPower = RNG::generate(0, power*2); // RNG::boxMuller(power, power/3)
		int verticaloffset = 0;
		if (!bu)
		{
			// it's possible we have a unit below the actual tile, when he stands on a stairs and sticks his head out to the next tile
			Tile *below = _save->getTile(Position(center.x/16, center.y/16, (center.z/24)-1));
			if (below)
			{
				BattleUnit *buBelow = below->getUnit();
				if (buBelow)
				{
					bu = buBelow;
					verticaloffset = 24;
				}
			}
		}
		if (bu)
		{
			const int sz = bu->getArmor()->getSize() * 8;
			const Position target = bu->getPosition() * Position(16,16,24) + Position(sz,sz, bu->getFloatHeight() - tile->getTerrainLevel());
			const Position relative = (center - target) - Position(0,0,verticaloffset);

			adjustedDamage = bu->damage(relative, rndPower, type);

			const int bravery = (110 - bu->getStats()->bravery) / 10;
			const int modifier = bu->getFaction() == FACTION_PLAYER ? _save->getMoraleModifier() : 100;
			const int morale_loss = 100 * (adjustedDamage * bravery / 10) / modifier;

			bu->moraleChange(-morale_loss);

			if (bu->getSpecialAbility() == SPECAB_EXPLODEONDEATH && !bu->isOut() && (bu->getHealth() == 0 || bu->getStunlevel() >= bu->getHealth()))
			{
				if (type != DT_STUN && type != DT_HE)
				{
					Position p = Position(bu->getPosition().x * 16, bu->getPosition().y * 16, bu->getPosition().z * 24);
					_save->getBattleState()->getBattleGame()->statePushNext(new ExplosionBState(_save->getBattleState()->getBattleGame(), p, 0, bu, 0));
				}
			}

			if (bu->getOriginalFaction() == FACTION_HOSTILE && unit->getOriginalFaction() == FACTION_PLAYER && type != DT_NONE)
			{
				unit->addFiringExp();
			}
		}
	}
	applyGravity(tile);
	calculateSunShading(); // roofs could have been destroyed
	calculateTerrainLighting(); // fires could have been started
	calculateFOV(center / Position(16,16,24));
	return bu;
}

/**
 * HE, smoke and fire explodes in a circular pattern on 1 level only. HE however damages floor tiles of the above level. Not the units on it.
 * HE destroys an object if its armor is lower than the explosive power, then it's HE blockage is applied for further propagation.
 * See http://www.ufopaedia.org/index.php?title=Explosions for more info.
 * @param center Center of the explosion in voxelspace.
 * @param power Power of the explosion.
 * @param type The damage type of the explosion.
 * @param maxRadius The maximum radius othe explosion.
 * @param unit The unit that caused the explosion.
 */
void TileEngine::explode(const Position &center, int power, ItemDamageType type, int maxRadius, BattleUnit *unit)
{
	double centerZ = (int)(center.z / 24) + 0.5;
	double centerX = (int)(center.x / 16) + 0.5;
	double centerY = (int)(center.y / 16) + 0.5;
	int power_;
	std::set<Tile*> tilesAffected;
	std::pair<std::set<Tile*>::iterator,bool> ret;

	if (type == DT_IN)
	{
		power /= 2;
	}

	int exHeight = Options::getInt("battleExplosionHeight");
	int vertdec = 1000; //default flat explosion
	if (exHeight<0) exHeight = 0;
	if (exHeight>3) exHeight = 3;
	switch (exHeight)
	{
	case 1:
		vertdec = 30;
		break;
	case 2:
		vertdec = 10;
		break;
	case 3:
		vertdec = 5;
	}


	for (int fi = -90; fi <= 90; fi += 5)
//	for (int fi = 0; fi <= 0; fi += 10)
	{
		// raytrace every 3 degrees makes sure we cover all tiles in a circle.
		for (int te = 0; te <= 360; te += 3)
		{
			double cos_te = cos(te * M_PI / 180.0);
			double sin_te = sin(te * M_PI / 180.0);
			double sin_fi = sin(fi * M_PI / 180.0);
			double cos_fi = cos(fi * M_PI / 180.0);

			Tile *origin = _save->getTile(Position(centerX, centerY, centerZ));
			double l = 0;
			double vx, vy, vz;
			int tileX, tileY, tileZ;
			power_ = power + 1;

			while (power_ > 0 && l <= maxRadius)
			{
				vx = centerX + l * sin_te * cos_fi;
				vy = centerY + l * cos_te * cos_fi;
				vz = centerZ + l * sin_fi;

				tileZ = int(floor(vz));
				tileX = int(floor(vx));
				tileY = int(floor(vy));

				Tile *dest = _save->getTile(Position(tileX, tileY, tileZ));
				if (!dest) break; // out of map!


				// blockage by terrain is deducted from the explosion power
				if (std::abs(l) > 0) // no need to block epicentrum
				{
					power_ -= (horizontalBlockage(origin, dest, type) + verticalBlockage(origin, dest, type)) * 2;
					power_ -= 10; // explosive damage decreases by 10 per tile
					if (origin->getPosition().z != tileZ) power_ -= vertdec; //3d explosion factor
					if (type == DT_IN)
					{
						int dir;
						Pathfinding::vectorToDirection(origin->getPosition() - dest->getPosition(), dir);
						if (dir != -1 && dir %2) power_ -= 5; // diagonal movement costs an extra 50% for fire.
					}
				}

				if (power_ > 0)
				{
					if (type == DT_HE)
					{
						// explosives do 1/2 damage to terrain and 1/2 up to 3/2 random damage to units (the halving is handled elsewhere)
						dest->setExplosive(power_);
					}

					ret = tilesAffected.insert(dest); // check if we had this tile already
					if (ret.second)
					{
						if (type == DT_STUN)
						{
							// power 50 - 150%
							if (dest->getUnit())
							{
								dest->getUnit()->damage(Position(0, 0, 0), (int)(RNG::generate(power_/2.0, power_*1.5)), type);
							}
							for (std::vector<BattleItem*>::iterator it = dest->getInventory()->begin(); it != dest->getInventory()->end(); ++it)
							{
								if ((*it)->getUnit())
								{
									(*it)->getUnit()->damage(Position(0, 0, 0), (int)(RNG::generate(power_/2.0, power_*1.5)), type);
								}
							}
						}
						if (type == DT_HE)
						{
							// power 50 - 150%
							if (dest->getUnit())
							{
								dest->getUnit()->damage(Position(0, 0, 0), (int)(RNG::generate(power_/2.0, power_*1.5)), type);
							}
							bool done = false;
							while (!done)
							{
								done = dest->getInventory()->size() == 0;
								for (std::vector<BattleItem*>::iterator it = dest->getInventory()->begin(); it != dest->getInventory()->end(); )
								{
									if (power_ > (*it)->getRules()->getArmor())
									{
										if ((*it)->getUnit() && (*it)->getUnit()->getStatus() == STATUS_UNCONSCIOUS)
											(*it)->getUnit()->instaKill();
										_save->removeItem((*it));
										break;
									}
									else
									{
										++it;
										done = it == dest->getInventory()->end();
									}
								}
							}
						}

						if (type == DT_SMOKE)
						{
							// smoke from explosions always stay 6 to 14 turns - power of a smoke grenade is 60
							if (dest->getSmoke() < 10)
							{
								dest->setFire(0);
								dest->setSmoke(RNG::generate(7, 15));
							}
						}

						if (type == DT_IN && !dest->isVoid())
						{
							if (dest->getFire() == 0)
							{
								dest->setFire(dest->getFuel() + 1);
								dest->setSmoke(std::max(1, std::min(15 - (dest->getFlammability() / 10), 12)));
							}
							if (dest->getUnit())
							{
								float resistance = dest->getUnit()->getArmor()->getDamageModifier(DT_IN);
								if (resistance > 0.0)
								{
									dest->getUnit()->damage(Position(0, 0, 12-dest->getTerrainLevel()), RNG::generate(5, 10), DT_IN, true);
									int burnTime = RNG::generate(0, int(5 * resistance));
									if (dest->getUnit()->getFire() < burnTime)
									{
										dest->getUnit()->setFire(burnTime); // catch fire and burn
									}
								}
							}
						}

						if (unit && dest->getUnit() && dest->getUnit()->getFaction() != unit->getFaction())
						{
							unit->addFiringExp();
						}

					}
				}
				origin = dest;
				l++;
			}
		}
	}
	// now detonate the tiles affected with HE

	if (type == DT_HE)
	{
		for (std::set<Tile*>::iterator i = tilesAffected.begin(); i != tilesAffected.end(); ++i)
		{
			if (detonate(*i))
				_save->setObjectiveDestroyed(true);
			applyGravity(*i);
			Tile *j = _save->getTile((*i)->getPosition() + Position(0,0,1));
			if (j)
				applyGravity(j);
		}
	}

	calculateSunShading(); // roofs could have been destroyed
	calculateTerrainLighting(); // fires could have been started
	calculateFOV(center / Position(16,16,24));
}

/**
 * Apply the explosive power to the tile parts. This is where the actual destruction takes place.
 * Must affect on 7 objects (6 box sides and object inside)
 * @return bool Return true objective was destroyed
 */
bool TileEngine::detonate(Tile* tile)
{
	int explosive = tile->getExplosive();
	tile->setExplosive(0,true);
	bool objective = false;
	Tile* tiles[7];
	static const int parts[7]={0,1,2,0,1,2,3};
	Position pos = tile->getPosition();
	tiles[0] = _save->getTile(Position(pos.x, pos.y, pos.z+1)); //ceiling
	tiles[1] = _save->getTile(Position(pos.x+1, pos.y, pos.z)); //east wall
	tiles[2] = _save->getTile(Position(pos.x, pos.y+1, pos.z)); //south wall
	tiles[3] = tiles[4] = tiles[5] = tiles[6] = tile;
	if (explosive)
	{
		int remainingPower = explosive;
		int flam = tile->getFlammability();
		int fuel = tile->getFuel() + 1;
		// explosions create smoke which only stays 1 or 2 turns
		tile->setSmoke(std::max(1, std::min(tile->getSmoke() + RNG::generate(0,2), 15)));
		for (int i = 0; i < 7; ++i)
		{
			if(tiles[i] && tiles[i]->getMapData(parts[i]))
			{
				remainingPower = explosive;
				while (remainingPower >= 0 && tiles[i]->getMapData(parts[i]))
				{
					remainingPower -= 2 * tiles[i]->getMapData(parts[i])->getArmor();
					if (remainingPower >= 0)
					{
						int volume = 0;
						// get the volume of the object by checking it's loftemps objects.
						for (int j=0; j < 12; j++)
						{
							if (tiles[i]->getMapData(parts[i])->getLoftID(j) != 0)
								++volume;
						}

						if (i > 3)
						{
							tiles[i]->setFire(0);
							int smoke = RNG::generate(0, (volume / 2) + 2);
							smoke += (volume / 2) + 1;
							if (smoke > tiles[i]->getSmoke())
							{
								tiles[i]->setSmoke(std::max(0, std::min(smoke, 15)));
							}
						}
						if (tiles[i]->destroy(parts[i]))
						{
							objective = true;
						}
						if (tiles[i]->getMapData(parts[i]))
						{
							flam = tiles[i]->getFlammability();
							fuel = tiles[i]->getFuel() + 1;
						}
					}
				}
				if (2 * flam < remainingPower)
				{
					tile->setFire(fuel);
					tile->setSmoke(std::max(1, std::min(15 - (flam / 10), 12)));
				}
			}
		}
	}

	return objective;
}


/**
 * Chained explosions are explosions which occur after an explosive map object is destroyed.
 * May be due a direct hit, other explosion or fire.
 * @return tile on which a explosion occurred
 */
Tile *TileEngine::checkForTerrainExplosions()
{

	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
	{
		if (_save->getTiles()[i]->getExplosive())
		{
			return _save->getTiles()[i];
		}
	}
	return 0;
}

/**
 * The amount of power that is blocked going from one tile to another on a different level.
 * Can cross more than one level. Only floor tiles are taken into account.
 * @param startTile The tile where the power starts.
 * @param endTile The adjacent tile where the power ends.
 * @param type The type of power/damage.
 * @return Amount of blockage of this power.
 */
int TileEngine::verticalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type)
{
	int block = 0;

	// safety check
	if (startTile == 0 || endTile == 0) return 0;
	int direction = endTile->getPosition().z - startTile->getPosition().z;

	if (direction == 0 ) return 0;

	int x = startTile->getPosition().x;
	int y = startTile->getPosition().y;

	if (direction < 0) // down
	{
		for (int z = startTile->getPosition().z; z > endTile->getPosition().z; z--)
		{
			block += blockage(_save->getTile(Position(x, y, z)), MapData::O_FLOOR, type);
			block += blockage(_save->getTile(Position(x, y, z)), MapData::O_OBJECT, type);
		}
		if (x != endTile->getPosition().x || y != endTile->getPosition().y)
		{
			x = endTile->getPosition().x;
			y = endTile->getPosition().y;
			for (int z = startTile->getPosition().z; z > endTile->getPosition().z; z--)
			{
				block += blockage(_save->getTile(Position(x, y, z)), MapData::O_FLOOR, type);
				block += blockage(_save->getTile(Position(x, y, z)), MapData::O_OBJECT, type);
			}
		}
	}
	else if (direction > 0) // up
	{
		for (int z = startTile->getPosition().z + 1; z <= endTile->getPosition().z; z++)
		{
			block += blockage(_save->getTile(Position(x, y, z)), MapData::O_FLOOR, type);
			block += blockage(_save->getTile(Position(x, y, z)), MapData::O_OBJECT, type);
		}
		if (x != endTile->getPosition().x || y != endTile->getPosition().y)
		{
			x = endTile->getPosition().x;
			y = endTile->getPosition().y;
			for (int z = startTile->getPosition().z + 1; z <= endTile->getPosition().z; z++)
			{
				block += blockage(_save->getTile(Position(x, y, z)), MapData::O_FLOOR, type);
				block += blockage(_save->getTile(Position(x, y, z)), MapData::O_OBJECT, type);
			}
		}
	}

	return block;
}

/**
 * The amount of power that is blocked going from one tile to another on the same level.
 * @param startTile The tile where the power starts.
 * @param endTile The adjacent tile where the power ends.
 * @param type The type of power/damage.
 * @return amount of blockage
 */
int TileEngine::horizontalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type)
{
	static const Position oneTileNorth = Position(0, -1, 0);
	static const Position oneTileEast = Position(1, 0, 0);
	static const Position oneTileSouth = Position(0, 1, 0);
	static const Position oneTileWest = Position(-1, 0, 0);

	// safety check
	if (startTile == 0 || endTile == 0) return 0;
	if (startTile->getPosition().z != endTile->getPosition().z) return 0;

	int direction;
	Pathfinding::vectorToDirection(endTile->getPosition() - startTile->getPosition(), direction);
	if (direction == -1) return 0;
	int block = 0;

	switch(direction)
	{
	case 0:	// north
		block = blockage(startTile, MapData::O_NORTHWALL, type);
		break;
	case 1: // north east
		if (type == DT_NONE) //this is two-way diagonal visiblity check, used in original game
		{
			block = blockage(startTile, MapData::O_NORTHWALL, type) + blockage(endTile, MapData::O_WESTWALL, type); //up+right
			block += blockage(_save->getTile(startTile->getPosition() + oneTileNorth), MapData::O_OBJECT, type, 3);
			if (block == 0) break; //this way is opened
			block = blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_NORTHWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_WESTWALL, type); //right+up
			block += blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_OBJECT, type, 7);
		}
		else
		{
			block = (blockage(startTile,MapData::O_NORTHWALL, type) + blockage(endTile,MapData::O_WESTWALL, type))/2
				+ (blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_NORTHWALL, type))/2;
			if (type == DT_HE)
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type, 4) +
				blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type, 6))/2;
			else
			block = (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type, 4) +
				  blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type, 6)) < 510?0:255;
		}
		break;
	case 2: // east
		block = blockage(endTile,MapData::O_WESTWALL, type);
		break;
	case 3: // south east
		if (type == DT_NONE)
		{
			block = blockage(_save->getTile(startTile->getPosition() + oneTileSouth), MapData::O_NORTHWALL, type)
				+ blockage(endTile, MapData::O_WESTWALL, type); //down+right
			block += blockage(_save->getTile(startTile->getPosition() + oneTileSouth), MapData::O_OBJECT, type, 1);
			if (block == 0) break; //this way is opened
			block = blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_WESTWALL, type)
				+ blockage(endTile, MapData::O_NORTHWALL, type); //right+down
			block += blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_OBJECT, type, 5);
		}
		else
		{
			block = (blockage(endTile,MapData::O_WESTWALL, type) + blockage(endTile,MapData::O_NORTHWALL, type))/2
				+ (blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_NORTHWALL, type))/2;
			if (type == DT_HE)
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type, 0) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type, 6))/2;
			else
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type, 0) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type, 6)) < 510?0:255;
		}
		break;
	case 4: // south
		block = blockage(endTile,MapData::O_NORTHWALL, type);
		break;
	case 5: // south west
		if (type == DT_NONE)
		{
			block = blockage(_save->getTile(startTile->getPosition() + oneTileSouth), MapData::O_NORTHWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileSouth), MapData::O_WESTWALL, type); //down+left
			block += blockage(_save->getTile(startTile->getPosition() + oneTileSouth), MapData::O_OBJECT, type, 7);
			if (block == 0) break; //this way is opened
			block = blockage(startTile, MapData::O_WESTWALL, type) + blockage(endTile, MapData::O_NORTHWALL, type); //left+down
			block += blockage(_save->getTile(startTile->getPosition() + oneTileWest), MapData::O_OBJECT, type, 3);
		}
		else
		{
			block = (blockage(endTile,MapData::O_NORTHWALL, type) + blockage(startTile,MapData::O_WESTWALL, type))/2
				+ (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_NORTHWALL, type))/2;
			if (type == DT_HE)
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type, 0) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type, 2))/2;
			else
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type, 0) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type, 2)) < 510?0:255;
		}
		break;
	case 6: // west
		block = blockage(startTile,MapData::O_WESTWALL, type);
		break;
	case 7: // north west

		if (type == DT_NONE)
		{
			block = blockage(startTile, MapData::O_NORTHWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileNorth), MapData::O_WESTWALL, type); //up+left
			block += blockage(_save->getTile(startTile->getPosition() + oneTileNorth), MapData::O_OBJECT, type, 5);
			if (block == 0) break; //this way is opened
			block = blockage(startTile, MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileWest), MapData::O_NORTHWALL, type); //left+up
			block += blockage(_save->getTile(startTile->getPosition() + oneTileWest), MapData::O_OBJECT, type, 1);
		}
		else
		{
			block = (blockage(startTile,MapData::O_WESTWALL, type) + blockage(startTile,MapData::O_NORTHWALL, type))/2
				+ (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_NORTHWALL, type))/2;
			if (type == DT_HE)
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type, 4) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type, 2))/2;
			else
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type, 4) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type, 2)) < 510?0:255;
		}
		break;
	}

	if (type != DT_NONE)
	{
		direction += 4;
		if (direction > 7)
			direction -= 8;
		block += blockage(endTile,MapData::O_OBJECT, type, direction);
	}
	else
	{
        block += blockage(startTile,MapData::O_OBJECT, type, direction);
        if ( block <= 127 ) 
        {
            direction += 4;
            if (direction > 7)
                direction -= 8;
            if (blockage(endTile,MapData::O_OBJECT, type, direction) > 127){
                return -1; //hit bigwall, reveal bigwall tile
            }
        }
	}

	return block;
}




/*
 * The amount this certain wall or floor-part of the tile blocks.
 * @param startTile The tile where the power starts.
 * @param part The part of the tile the power needs to go through.
 * @param type The type of power/damage.
 * @return amount of blockage
 */
int TileEngine::blockage(Tile *tile, const int part, ItemDamageType type, int direction)
{
	int blockage = 0;

	if (tile == 0) return 0; // probably outside the map here
	if (tile->getMapData(part))
	{
		bool check = true;
		int wall = -1;
		if (direction != -1)
		{
			wall = tile->getMapData(MapData::O_OBJECT)->getBigWall();
			switch (direction)
			{
			case 0: // north
				if (wall == Pathfinding::BIGWALLWEST ||
					wall == Pathfinding::BIGWALLEAST ||
					wall == Pathfinding::BIGWALLSOUTH ||
					wall == Pathfinding::BIGWALLEASTANDSOUTH)
				{
					check = false;
				}
				break;
			case 1: // north east
				if (wall == Pathfinding::BIGWALLWEST ||
					wall == Pathfinding::BIGWALLSOUTH)
				{
					check = false;
				}
				break;
			case 2: // east
				if (wall == Pathfinding::BIGWALLNORTH ||
					wall == Pathfinding::BIGWALLSOUTH ||
					wall == Pathfinding::BIGWALLWEST)
				{
					check = false;
				}
				break;
			case 3: // south east
				if (wall == Pathfinding::BIGWALLNORTH ||
					wall == Pathfinding::BIGWALLWEST)
				{
					check = false;
				}
				break;
			case 4: // south
				if (wall == Pathfinding::BIGWALLWEST ||
					wall == Pathfinding::BIGWALLEAST ||
					wall == Pathfinding::BIGWALLNORTH)
				{
					check = false;
				}
				break;
			case 5: // south west
				if (wall == Pathfinding::BIGWALLNORTH ||
					wall == Pathfinding::BIGWALLEAST)
				{
					check = false;
				}
				break;
			case 6: // west
				if (wall == Pathfinding::BIGWALLNORTH ||
					wall == Pathfinding::BIGWALLSOUTH ||
					wall == Pathfinding::BIGWALLEAST ||
					wall == Pathfinding::BIGWALLEASTANDSOUTH)
				{
					check = false;
				}
				break;
			case 7: // north west
				if (wall == Pathfinding::BIGWALLSOUTH ||
					wall == Pathfinding::BIGWALLEAST ||
					wall == Pathfinding::BIGWALLEASTANDSOUTH)
				{
					check = false;
				}
				break;
			default:
				break;
			}
		}

		if (check)
		{
			if (type == DT_SMOKE && wall != 0)
			{
				return 255;
			}
			blockage += tile->getMapData(part)->getBlock(type);
		}
	}

	// open ufo doors are actually still closed behind the scenes
	// so a special trick is needed to see if they are open, if they are, they obviously don't block anything
	if (tile->isUfoDoorOpen(part))
		blockage = 0;

	return blockage;
}


/**
 * Soldier opens a door (if any) by rightclick, or by walking through it. The unit has to face in the right direction.
 * @param unit
 * @return -1 there is no door, you can walk through.
 *		  0 normal door opened, make a squeaky sound and you can walk through.
 *		  1 ufo door is starting to open, make a whoosh sound, don't walk through.
 *		  3 ufo door is still opening, don't walk through it yet. (have patience, futuristic technology...)
 */
int TileEngine::unitOpensDoor(BattleUnit *unit, bool rClick, int dir)
{
	int door = -1;
	int TUCost = 0;
	int size = unit->getArmor()->getSize();
	int z = unit->getTile()->getTerrainLevel() < -12 ? 1 : 0; // if we're standing on stairs, check the tile above instead.
	if (size > 1 && rClick)
		return door;
	if (dir == -1)
	{
		dir = unit->getDirection();
	}
	for (int x = 0; x < size && door == -1; x++)
	{
		for (int y = 0; y < size && door == -1; y++)
		{
			std::vector<std::pair<Position, int> > checkPositions;
			Tile *tile = _save->getTile(unit->getPosition() + Position(x,y,z));
			if (!tile) continue;

			switch (dir)
			{
			case 0: // north
				checkPositions.push_back(std::make_pair(Position(0, 0, 0), MapData::O_NORTHWALL)); // origin
				break;
			case 1: // north east
				checkPositions.push_back(std::make_pair(Position(0, 0, 0), MapData::O_NORTHWALL)); // origin
				checkPositions.push_back(std::make_pair(Position(1, -1, 0), MapData::O_WESTWALL)); // one tile north-east
				checkPositions.push_back(std::make_pair(Position(1, 0, 0), MapData::O_WESTWALL)); // one tile east
				checkPositions.push_back(std::make_pair(Position(1, 0, 0), MapData::O_NORTHWALL)); // one tile east
				break;
			case 2: // east
				checkPositions.push_back(std::make_pair(Position(1, 0, 0), MapData::O_WESTWALL)); // one tile east
				break;
			case 3: // south-east
				checkPositions.push_back(std::make_pair(Position(1, 0, 0), MapData::O_WESTWALL)); // one tile east
				checkPositions.push_back(std::make_pair(Position(0, 1, 0), MapData::O_NORTHWALL)); // one tile south
				checkPositions.push_back(std::make_pair(Position(1, 1, 0), MapData::O_WESTWALL)); // one tile south-east
				checkPositions.push_back(std::make_pair(Position(1, 1, 0), MapData::O_NORTHWALL)); // one tile south-east
				break;
			case 4: // south
				checkPositions.push_back(std::make_pair(Position(0, 1, 0), MapData::O_NORTHWALL)); // one tile south
				break;
			case 5: // south-west
				checkPositions.push_back(std::make_pair(Position(0, 0, 0), MapData::O_WESTWALL)); // origin
				checkPositions.push_back(std::make_pair(Position(0, 1, 0), MapData::O_WESTWALL)); // one tile south
				checkPositions.push_back(std::make_pair(Position(0, 1, 0), MapData::O_NORTHWALL)); // one tile south
				checkPositions.push_back(std::make_pair(Position(-1, 1, 0), MapData::O_NORTHWALL)); // one tile south-west
				break;
			case 6: // west
				checkPositions.push_back(std::make_pair(Position(0, 0, 0), MapData::O_WESTWALL)); // origin
				break;
			case 7: // north-west
				checkPositions.push_back(std::make_pair(Position(0, 0, 0), MapData::O_WESTWALL)); // origin
				checkPositions.push_back(std::make_pair(Position(0, 0, 0), MapData::O_NORTHWALL)); // origin
				checkPositions.push_back(std::make_pair(Position(0, -1, 0), MapData::O_WESTWALL)); // one tile north
				checkPositions.push_back(std::make_pair(Position(-1, 0, 0), MapData::O_NORTHWALL)); // one tile west
				break;
			default:
				break;
			}

			int part = 0;
			for (std::vector<std::pair<Position, int> >::const_iterator i = checkPositions.begin(); i != checkPositions.end() && door == -1; ++i)
			{
				tile = _save->getTile(unit->getPosition() + Position(x,y,z) + i->first);
				if (tile)
				{
					door = tile->openDoor(i->second, unit, _save->getDebugMode());
					if (door != -1)
					{
						part = i->second;
						if (door == 1)
						{
							checkAdjacentDoors(unit->getPosition() + Position(x,y,z) + i->first, i->second);
						}
					}
				}
			}

			if (door == 0 && rClick)
			{
				if (part == MapData::O_WESTWALL)
				{
					part = MapData::O_NORTHWALL;
				}
				else
				{
					part = MapData::O_WESTWALL;
				}
				TUCost = tile->getTUCost(part, unit->getArmor()->getMovementType());
			}
			else if (door == 1)
			{
				TUCost = tile->getTUCost(part, unit->getArmor()->getMovementType());
			}
		}
	}

	if (door == 0 || door == 1)
	{
		unit->spendTimeUnits(TUCost);
		calculateFOV(unit->getPosition());
		// look from the other side (may be need check reaction fire?)
		std::vector<BattleUnit*> *vunits = unit->getVisibleUnits();
		for (size_t i = 0; i < vunits->size(); ++i)
		{
			calculateFOV(vunits->at(i));
		}
	}

	return door;
}

/**
 * open any doors connected to this part at this position,
 * keeps processing til it hits a non-ufo-door.
 * @param pos the starting position
 * @param part the part to open, defines which direction to check.
 */
void TileEngine::checkAdjacentDoors(Position pos, int part)
{
	Position offset;
	bool westSide = (part == 1);
	for (int i = 1;; ++i)
	{
		offset = westSide ? Position(0,i,0):Position(i,0,0);
		Tile *tile = _save->getTile(pos + offset);
		if (tile && tile->getMapData(part) && tile->getMapData(part)->isUFODoor())
		{
			tile->openDoor(part);
		}
		else break;
	}
	for (int i = -1;; --i)
	{
		offset = westSide ? Position(0,i,0):Position(i,0,0);
		Tile *tile = _save->getTile(pos + offset);
		if (tile && tile->getMapData(part) && tile->getMapData(part)->isUFODoor())
		{
			tile->openDoor(part);
		}
		else break;
	}
}

/**
 * Close ufo doors.
 * @return whether doors are closed.
 */
int TileEngine::closeUfoDoors()
{
	int doorsclosed = 0;

	// prepare a list of tiles on fire/smoke & close any ufo doors
	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
	{
		if (_save->getTiles()[i]->getUnit() && _save->getTiles()[i]->getUnit()->getArmor()->getSize() > 1)
		{
			BattleUnit *bu = _save->getTiles()[i]->getUnit();
			Tile *tile = _save->getTiles()[i];
			Tile *oneTileNorth = _save->getTile(tile->getPosition() + Position(0, -1, 0));
			Tile *oneTileWest = _save->getTile(tile->getPosition() + Position(-1, 0, 0));
			if ((tile->isUfoDoorOpen(MapData::O_NORTHWALL) && oneTileNorth && oneTileNorth->getUnit() && oneTileNorth->getUnit() == bu) ||
				(tile->isUfoDoorOpen(MapData::O_WESTWALL) && oneTileWest && oneTileWest->getUnit() && oneTileWest->getUnit() == bu))
			{
				continue;
			}
		}
		doorsclosed += _save->getTiles()[i]->closeUfoDoor();
	}

	return doorsclosed;
}
/**
 * calculateLine. Using bresenham algorithm in 3D.
 * @param origin (voxel??)
 * @param target (also voxel??)
 * @param storeTrajectory true will store the whole trajectory - otherwise it just stores the last position.
 * @param trajectory A vector of positions in which the trajectory is stored.
 * @param excludeUnit Excludes this unit in the collision detection.
 * @param doVoxelCheck Check against voxel or tile blocking? (first one for units visibility and line of fire, second one for terrain visibility)
 * @param onlyVisible skip invisible units? used in FPS view
 * @param excludeAllBut [optional] the only unit to be considered for ray hits
 * @return the objectnumber(0-3) or unit(4) or out of map (5) or -1(hit nothing)
 */
int TileEngine::calculateLine(const Position& origin, const Position& target, bool storeTrajectory, std::vector<Position> *trajectory, BattleUnit *excludeUnit, bool doVoxelCheck, bool onlyVisible, BattleUnit *excludeAllBut)
{
	int x, x0, x1, delta_x, step_x;
	int y, y0, y1, delta_y, step_y;
	int z, z0, z1, delta_z, step_z;
	int swap_xy, swap_xz;
	int drift_xy, drift_xz;
	int cx, cy, cz;
	Position lastPoint(origin);
	int result;

	//start and end points
	x0 = origin.x;	 x1 = target.x;
	y0 = origin.y;	 y1 = target.y;
	z0 = origin.z;	 z1 = target.z;

	//'steep' xy Line, make longest delta x plane
	swap_xy = abs(y1 - y0) > abs(x1 - x0);
	if (swap_xy)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	//do same for xz
	swap_xz = abs(z1 - z0) > abs(x1 - x0);
	if (swap_xz)
	{
		std::swap(x0, z0);
		std::swap(x1, z1);
	}

	//delta is Length in each plane
	delta_x = abs(x1 - x0);
	delta_y = abs(y1 - y0);
	delta_z = abs(z1 - z0);

	//drift controls when to step in 'shallow' planes
	//starting value keeps Line centred
	drift_xy  = (delta_x / 2);
	drift_xz  = (delta_x / 2);

	//direction of line
	step_x = 1;  if (x0 > x1) {  step_x = -1; }
	step_y = 1;  if (y0 > y1) {  step_y = -1; }
	step_z = 1;  if (z0 > z1) {  step_z = -1; }

	//starting point
	y = y0;
	z = z0;

	//step through longest delta (which we have swapped to x)
	for (x = x0; x != (x1+step_x); x += step_x)
	{
		//copy position
		cx = x;	cy = y;	cz = z;

		//unswap (in reverse)
		if (swap_xz) std::swap(cx, cz);
		if (swap_xy) std::swap(cx, cy);

		if (storeTrajectory && trajectory)
		{
			trajectory->push_back(Position(cx, cy, cz));
		}
		//passes through this point?
		if (doVoxelCheck)
		{
			result = voxelCheck(Position(cx, cy, cz), excludeUnit, false, onlyVisible, excludeAllBut);
			if (result != -1)
			{
				if (trajectory)
				{ // store the position of impact
					trajectory->push_back(Position(cx, cy, cz));
				}
				return result;
			}
		}
		else
		{
            int temp_res = verticalBlockage(_save->getTile(lastPoint), _save->getTile(Position(cx, cy, cz)), DT_NONE);
			result = horizontalBlockage(_save->getTile(lastPoint), _save->getTile(Position(cx, cy, cz)), DT_NONE);
            if (result == -1)
            {
                if (temp_res > 127)
                {
                    result = 0;
                } else {
                return result; // We hit a big wall
                }
            }
            result += temp_res;
			if (result > 127)
			{
				return result;
			}

			lastPoint = Position(cx, cy, cz);
		}
		//update progress in other planes
		drift_xy = drift_xy - delta_y;
		drift_xz = drift_xz - delta_z;

		//step in y plane
		if (drift_xy < 0)
		{
			y = y + step_y;
			drift_xy = drift_xy + delta_x;

			//check for xy diagonal intermediate voxel step
			if (doVoxelCheck)
			{
				cx = x;	cz = z; cy = y;
				if (swap_xz) std::swap(cx, cz);
				if (swap_xy) std::swap(cx, cy);
				result = voxelCheck(Position(cx, cy, cz), excludeUnit, false, onlyVisible, excludeAllBut);
				if (result != -1)
				{
					if (trajectory != 0)
					{ // store the position of impact
						trajectory->push_back(Position(cx, cy, cz));
					}
					return result;
				}
			}
		}

		//same in z
		if (drift_xz < 0)
		{
			z = z + step_z;
			drift_xz = drift_xz + delta_x;

			//check for xz diagonal intermediate voxel step
			if (doVoxelCheck)
			{
				cx = x;	cz = z; cy = y;
				if (swap_xz) std::swap(cx, cz);
				if (swap_xy) std::swap(cx, cy);
				result = voxelCheck(Position(cx, cy, cz), excludeUnit, false, onlyVisible,  excludeAllBut);
				if (result != -1)
				{
					if (trajectory != 0)
					{ // store the position of impact
						trajectory->push_back(Position(cx, cy, cz));
					}
					return result;
				}
			}
		}
	}

	return -1;
}

/**
 * Calculate a parabola trajectory, used for throwing items.
 * @param origin in voxelspace
 * @param target in voxelspace
 * @param storeTrajectory true will store the whole trajectory - otherwise it just stores the last position.
 * @param excludeUnit makes sure the trajectory does not hit the shooter itself
 * @param curvature how high the parabola goes: 1.0 is almost straight throw, 3.0 is a very high throw, to throw over a fence for example
 * @param accuracy is the deviation of the angles it should take into account. 1.0 is perfection.
 * @return the objectnumber(0-3) or unit(4) or out of map (5) or -1(hit nothing)
 */
int TileEngine::calculateParabola(const Position& origin, const Position& target, bool storeTrajectory, std::vector<Position> *trajectory, BattleUnit *excludeUnit, double curvature, double accuracy)
{
	double ro = sqrt((double)((target.x - origin.x) * (target.x - origin.x) + (target.y - origin.y) * (target.y - origin.y) + (target.z - origin.z) * (target.z - origin.z)));

	double fi = acos((double)(target.z - origin.z) / ro);
	double te = atan2((double)(target.y - origin.y), (double)(target.x - origin.x));

	fi *= accuracy;
	te *= accuracy;

	double zA = sqrt(ro)*curvature;
	double zK = 4.0 * zA / ro / ro;

	int x = origin.x;
	int y = origin.y;
	int z = origin.z;
	int i = 8;

	while (z > 0) {
		x = (int)((double)origin.x + (double)i * cos(te) * sin(fi));
		y = (int)((double)origin.y + (double)i * sin(te) * sin(fi));
		z = (int)((double)origin.z + (double)i * cos(fi) - zK * ((double)i - ro / 2.0) * ((double)i - ro / 2.0) + zA);
		if (storeTrajectory && trajectory)
		{
			trajectory->push_back(Position(x, y, z));
		}
		//passes through this point?
		int result = voxelCheck(Position(x, y, z), excludeUnit);
		if (result != -1)
		{
			if (!storeTrajectory && trajectory != 0)
			{ // store the position of impact
				trajectory->push_back(Position(x, y, z));
			}
			return result;
		}
		++i;
	}
	if (!storeTrajectory && trajectory != 0)
	{ // store the position of impact
		trajectory->push_back(Position(x, y, z));
	}
	return -1;
}

/**
 * Calculate z "grounded" value for particular voxel (used for projectile shadow).
 * @param voxel The voxel to trace down.
 * @return z coord of "ground"
 */
int TileEngine::castedShade(const Position& voxel)
{
	int zstart = voxel.z;
	Position tmpVoxel = voxel;
	int z;
	for (z = zstart; z>0; z--)
	{
		tmpVoxel.z = z;
		if (voxelCheck(tmpVoxel, 0) != -1) break;
			
	}
    return z;
}

/**
 * Trace voxel visibility.
 * @param voxel coordinate.
 * @return visible or not
 */

bool TileEngine::isVoxelVisible(const Position& voxel)
{
	int zstart = voxel.z+3; // slight Z adjust
	if ((zstart/24)!=(voxel.z/24))
		return true; // visble!
	Position tmpVoxel = voxel;
	int zend = (zstart/24)*24 +24;
	for (int z = zstart; z<zend; z++)
	{
		tmpVoxel.z=z;
		// only OBJECT can cause additional occlusion (because of any shape)
		if (voxelCheck(tmpVoxel, 0) == MapData::O_OBJECT) return false;
		++tmpVoxel.x;
		if (voxelCheck(tmpVoxel, 0) == MapData::O_OBJECT) return false;
		++tmpVoxel.y;
		if (voxelCheck(tmpVoxel, 0) == MapData::O_OBJECT) return false;
	}
    return true;
}



/**
 * Check if we hit a voxel.
 * @param voxel The voxel to check.
 * @param excludeUnit Don't do checks on this unit.
 * @param excludeAllUnits Don't do checks on any unit.
 * @param excludeAllBut if set, the only unit to be considered for ray hits
 * @return the objectnumber(0-3) or unit(4) or out of map (5) or -1(hit nothing)
 */
int TileEngine::voxelCheck(const Position& voxel, BattleUnit *excludeUnit, bool excludeAllUnits, bool onlyVisible, BattleUnit *excludeAllBut)
{

	Tile *tile = _save->getTile(Position(voxel.x/16, voxel.y/16, voxel.z/24));
	// check if we are not out of the map
	if (tile == 0 || voxel.x < 0 || voxel.y < 0 || voxel.z < 0)
	{
		return 5;
	}
	
	if (voxel.z % 24 == 0 && tile->getMapData(MapData::O_FLOOR) && tile->getMapData(MapData::O_FLOOR)->isGravLift())
	{		
		Tile *tileBelow = _save->getTile(tile->getPosition() + Position(0,0,-1));
		if (tileBelow && tileBelow->getMapData(MapData::O_FLOOR) && !tileBelow->getMapData(MapData::O_FLOOR)->isGravLift())
			return 0;
	}

	// first we check terrain voxel data, not to allow 2x2 units stick through walls
	for (int i=0; i< 4; ++i)
	{
		MapData *mp = tile->getMapData(i);
		if (tile->isUfoDoorOpen(i))
			continue;
		if (mp != 0)
		{
			int x = 15 - voxel.x%16;
			int y = voxel.y%16;
			int idx = (mp->getLoftID((voxel.z%24)/2)*16) + y;
			if (_voxelData->at(idx) & (1 << x))
			{
				return i;
			}
		}
	}

	if (!excludeAllUnits)
	{
		BattleUnit *unit = tile->getUnit();
		// sometimes there is unit on the tile below, but sticks up to this tile with his head,
		// in this case we couldn't have unit standing at current tile.
		if (unit == 0 && tile->hasNoFloor(0))
		{
			tile = _save->getTile(Position(voxel.x/16, voxel.y/16, (voxel.z/24)-1)); //below
			if (tile) unit = tile->getUnit();
		}

		if (unit != 0 && unit != excludeUnit && (!excludeAllBut || unit == excludeAllBut) && (!onlyVisible || unit->getVisible() ) )
		{
			Position tilepos;
			Position unitpos = unit->getPosition();
			int tz = unitpos.z*24 + unit->getFloatHeight()+(-tile->getTerrainLevel());//bottom
			if ((voxel.z > tz) && (voxel.z <= tz + unit->getHeight()) )
			{
				int x = voxel.x%16;
				int y = voxel.y%16;
				int part = 0;
				if (unit->getArmor()->getSize() > 1)
				{
					tilepos = tile->getPosition();
					part = tilepos.x - unitpos.x + (tilepos.y - unitpos.y)*2;
				}
				int idx = (unit->getLoftemps(part) * 16) + y;
				if (_voxelData->at(idx) & (1 << x))
				{
					return 4;
				}
			}
		}
	}
	return -1;
}


/**
 * Toggles personal lighting on / off.
 */
void TileEngine::togglePersonalLighting()
{
	_personalLighting = !_personalLighting;
	calculateUnitLighting();
}

/**
 * Distance between 2 points. Rounded up to first INT.
 * @return distance
 */
int TileEngine::distance(const Position &pos1, const Position &pos2) const
{
	int x = pos1.x - pos2.x;
	int y = pos1.y - pos2.y;
	return int(floor(sqrt(float(x*x + y*y)) + 0.5));
}


/**
 * Distance squared between 2 points. No sqrt(), not floating point math, and sometimes it's all you need.
 * @return distance
 */
int TileEngine::distanceSq(const Position &pos1, const Position &pos2, bool considerZ) const
{
	int x = pos1.x - pos2.x;
	int y = pos1.y - pos2.y;
	int z = considerZ ? (pos1.z - pos2.z) : 0;
	return x*x + y*y + z*z;
}



/**
 * Psionic attack mechanism.
 * @param action
 * @return whether it failed or succeeded
 */
bool TileEngine::psiAttack(BattleAction *action)
{
	BattleUnit *victim = _save->getTile(action->target)->getUnit();
	double attackStrength = action->actor->getStats()->psiStrength * action->actor->getStats()->psiSkill / 50.0;
	double defenseStrength = victim->getStats()->psiStrength
		+ ((victim->getStats()->psiSkill > 0) ? 10.0 + victim->getStats()->psiSkill / 5.0 : 10.0);
	double d = distance(action->actor->getPosition(), action->target);
	attackStrength -= d;
	attackStrength += RNG::generate(0,55);

	if (action->type == BA_MINDCONTROL)
	{
		defenseStrength += 20;
	}

	action->actor->addPsiExp();
	if (attackStrength > defenseStrength)
	{
		action->actor->addPsiExp();
		action->actor->addPsiExp();
		if (action->type == BA_PANIC)
		{
			int moraleLoss = (110-_save->getTile(action->target)->getUnit()->getStats()->bravery);
			if (moraleLoss > 0)
			_save->getTile(action->target)->getUnit()->moraleChange(-moraleLoss);
		}
		else// if (action->type == BA_MINDCONTROL)
		{
			victim->convertToFaction(action->actor->getFaction());
			calculateFOV(victim->getPosition());
			calculateUnitLighting();
			victim->setTimeUnits(victim->getStats()->tu);
			victim->allowReselect();
			victim->abortTurn(); // resets unit status to STANDING
			// if all units from either faction are mind controlled - auto-end the mission.
			if (Options::getBool("battleAutoEnd") && Options::getBool("allowPsionicCapture"))
			{
				int liveAliens = 0;
				int liveSoldiers = 0;
				_save->getBattleState()->getBattleGame()->tallyUnits(liveAliens, liveSoldiers, false);
				if (liveAliens == 0 || liveSoldiers == 0)
				{
					_save->getBattleState()->getBattleGame()->requestEndTurn();
				}
			}
		}
		return true;
	}
	return false;
}

/**
 * Apply gravity to a tile. Causes items and units to drop.
 * @param t Tile
 * @return Tile where the items end up in eventually.
 */
Tile *TileEngine::applyGravity(Tile *t)
{
	if (t->getInventory()->size() == 0 && !t->getUnit()) return t; // skip this if there are no items
 
	Position p = t->getPosition();
	Tile *rt = t;
	Tile *rtb;
	BattleUnit *occupant = t->getUnit();
 
	if (occupant && (occupant->getArmor()->getMovementType() != MT_FLY || occupant->isOut()))
	{
		Position unitpos = occupant->getPosition();
		while (unitpos.z >= 0)
		{
			bool canFall = true;
			for (int y = 0; y < occupant->getArmor()->getSize() && canFall; ++y)
			{
				for (int x = 0; x < occupant->getArmor()->getSize() && canFall; ++x)
				{
					rt = _save->getTile(Position(unitpos.x+x, unitpos.y+y, unitpos.z));
					rtb = _save->getTile(Position(unitpos.x+x, unitpos.y+y, unitpos.z-1)); //below
					if (!rt->hasNoFloor(rtb))
					{
						canFall = false;
					}
				}
			}
			if (!canFall)
				break;
			unitpos.z--;
		}
		if (unitpos != occupant->getPosition())
		{
			if (occupant->getHealth() != 0 && occupant->getStunlevel() < occupant->getHealth())
			{
				occupant->startWalking(Pathfinding::DIR_DOWN, occupant->getPosition() + Position(0,0,-1),
					_save->getTile(occupant->getPosition() + Position(0,0,-1)), true);
				_save->addFallingUnit(occupant);
			}
			else
			{
				Position origin = occupant->getPosition();
				for (int y = occupant->getArmor()->getSize()-1; y >= 0; --y)
				{
					for (int x = occupant->getArmor()->getSize()-1; x >= 0; --x)
					{
						_save->getTile(origin + Position(x, y, 0))->setUnit(0);
					}
				}
				occupant->setPosition(unitpos);
			}
		}
	}
	rt = t;
	bool canFall = true;
	while (p.z >= 0 && canFall)
	{
		rt = _save->getTile(p);
		rtb = _save->getTile(Position(p.x, p.y, p.z-1)); //below
		if (!rt->hasNoFloor(rtb))
			canFall = false;
		p.z--;
	}
	
	for (std::vector<BattleItem*>::iterator it = t->getInventory()->begin(); it != t->getInventory()->end(); ++it)
	{
		if ((*it)->getUnit() && t->getPosition() == (*it)->getUnit()->getPosition())
		{
			(*it)->getUnit()->setPosition(rt->getPosition());
		}
		if (t != rt)
		{
			rt->addItem(*it, (*it)->getSlot());
		}
	}
	
	if (t != rt)
	{
		// clear tile
		t->getInventory()->clear();
	}

	return rt;
}
/*
 * Validate the melee range between two units.
 * @param *unit the attacking unit.
 * @param *target the unit we want to attack.
 * @param dir direction to check.
 * @return true when range is valid.
 */
bool TileEngine::validMeleeRange(BattleUnit *attacker, BattleUnit *target, int dir)
{
	return validMeleeRange(attacker->getPosition(), dir, attacker, target);
}

/*
 * Validate the melee range between a tile and a unit.
 * @param pos Position to check from.
 * @param direction direction to check.
 * @param size for large units, we have to do extra checks.
 * @param *target the unit we want to attack, 0 for any unit.
 * @return true when range is valid.
 */
bool TileEngine::validMeleeRange(Position pos, int direction, BattleUnit *attacker, BattleUnit *target)
{
	if (direction < 0 || direction > 7)
	{
		return false;
	}
	Position p;
	int size = attacker->getArmor()->getSize() - 1;
	Pathfinding::directionToVector(direction, &p);
	for (int x = 0; x <= size; ++x)
	{
		for (int y = 0; y <= size; ++y)
		{
			Tile *origin (_save->getTile(Position(pos + Position(x, y, 0))));
			Tile *targetTile (_save->getTile(Position(pos + Position(x, y, 0) + p)));
			Tile *aboveTargetTile (_save->getTile(Position(pos + Position(x, y, 1) + p)));

			if (targetTile && origin)
			{
				if (origin->getTerrainLevel() <= -16 && aboveTargetTile && !aboveTargetTile->hasNoFloor(targetTile))
				{
					targetTile = aboveTargetTile;
				}
				if (targetTile->getUnit())
				{
					if (target == 0 || targetTile->getUnit() == target)
					{
						Position originVoxel = Position(origin->getPosition() * Position(16,16,24))
							+ Position(8,8,attacker->getHeight() + attacker->getFloatHeight() - 4 -origin->getTerrainLevel());
						Position targetVoxel;
						if (canTargetUnit(&originVoxel, targetTile, &targetVoxel, attacker))
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}
/*
 * AI: Check for windows.
 * @return direction or -1 when no window found
 */
int TileEngine::faceWindow(const Position &position)
{
	static const Position oneTileEast = Position(1, 0, 0);
	static const Position oneTileSouth = Position(0, 1, 0);

	Tile *tile = _save->getTile(position);
	if (tile && tile->getMapData(MapData::O_NORTHWALL) && tile->getMapData(MapData::O_NORTHWALL)->getBlock(DT_NONE)==0) return 0;
	tile = _save->getTile(position + oneTileEast);
	if (tile && tile->getMapData(MapData::O_WESTWALL) && tile->getMapData(MapData::O_WESTWALL)->getBlock(DT_NONE)==0) return 2;
	tile = _save->getTile(position + oneTileSouth);
	if (tile && tile->getMapData(MapData::O_NORTHWALL) && tile->getMapData(MapData::O_NORTHWALL)->getBlock(DT_NONE)==0) return 4;
	tile = _save->getTile(position);
	if (tile && tile->getMapData(MapData::O_WESTWALL) && tile->getMapData(MapData::O_WESTWALL)->getBlock(DT_NONE)==0) return 6;

	return -1;
}

/*
 * Validate a throw action
 * @param action the action to validate.
 * @return validity of action.
 */
bool TileEngine::validateThrow(BattleAction *action)
{
	Position originVoxel, targetVoxel;
	bool foundCurve = false;
	Position origin = action->actor->getPosition();
	std::vector<Position> _trajectory;
	// object blocking - can't throw here
	if (action->type == BA_THROW && _save->getTile(action->target) && _save->getTile(action->target)->getMapData(MapData::O_OBJECT) && _save->getTile(action->target)->getMapData(MapData::O_OBJECT)->getTUCost(MT_WALK) == 255)
	{
		return false;
	}

	originVoxel = Position(origin.x*16 + 8, origin.y*16 + 8, origin.z*24);
	originVoxel.z += -_save->getTile(origin)->getTerrainLevel();
	originVoxel.z += action->actor->getHeight() + action->actor->getFloatHeight();
	originVoxel.z -= 3;
	if (originVoxel.z >= (origin.z + 1)*24)
	{
		origin.z++;
	}


	// determine the target voxel.
	// aim at the center of the floor
	targetVoxel = Position(action->target.x*16 + 8, action->target.y*16 + 8, action->target.z*24 + 2);
	targetVoxel.z -= _save->getTile(action->target)->getTerrainLevel();
	if (action->type != BA_THROW)
	{
		BattleUnit *tu = _save->getTile(action->target)->getUnit();
		if(!tu && action->target.z > 0)
			tu = _save->getTile(Position(action->target.x, action->target.y, action->target.z-1))->getUnit();
		if (tu)
		{
			targetVoxel.z += (tu->getHeight() / 2) + tu->getFloatHeight();
		}
	}

	// we try 4 different curvatures to try and reach our goal.
	double curvature = 1.0;
	while (!foundCurve && curvature < 5.0)
	{
		int check = calculateParabola(originVoxel, targetVoxel, false, &_trajectory, action->actor, curvature, 1.0);
		if (check != 5 && (int)_trajectory.at(0).x/16 == (int)targetVoxel.x/16 && (int)_trajectory.at(0).y/16 == (int)targetVoxel.y/16 && (int)_trajectory.at(0).z/24 == (int)targetVoxel.z/24)
		{
			foundCurve = true;
		}
		else
		{
			curvature += 1.0;
		}
		_trajectory.clear();
	}
	if (AreSame(curvature, 5.0))
	{
		return false;
	}

	return ProjectileFlyBState::validThrowRange(action);
}

/*
 * Recalculate FOV of all units in-game.
 */
void TileEngine::recalculateFOV()
{
	for (std::vector<BattleUnit*>::iterator bu = _save->getUnits()->begin(); bu != _save->getUnits()->end(); ++bu)
	{
		if ((*bu)->getTile() != 0)
		{
			calculateFOV(*bu);
		}
	}
}

}
