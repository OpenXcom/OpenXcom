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
#define _USE_MATH_DEFINES
#include <cmath>
#include <set>
#include "TileEngine.h"
#include <SDL.h>
#include "BattleAIState.h"
#include "AggroBAIState.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Soldier.h"
#include "../Engine/RNG.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/MapData.h"
#include "../Ruleset/Unit.h"
#include "../Ruleset/RuleSoldier.h"
#include "../Ruleset/Armor.h"
#include "../Resource/ResourcePack.h"
#include "Pathfinding.h"
#include "../Engine/Options.h"

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

	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
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
		if (verticalBlockage(_save->getTile(Position(tile->getPosition().x, tile->getPosition().y, _save->getHeight() - 1)), tile, DT_NONE))
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
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
	{
		_save->getTiles()[i]->resetLight(layer);
	}

	// add lighting of terrain
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
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

	// reset all light to 0 first
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
	{
		_save->getTiles()[i]->resetLight(layer);
	}

	if (_personalLighting)
	{
		// add lighting of soldiers
		for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			if ((*i)->getFaction() == FACTION_PLAYER && !(*i)->isOut())
			{
				addLight((*i)->getPosition(), personalLightPower, layer);
			}
		}
	}
}

/**
 * Adds circular light pattern starting from center and loosing power with distance travelled.
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
			for (int z = 0; z < _save->getHeight(); z++)
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
	size_t visibleUnitsChecksum = 0, oldNumVisibleUnits = 0;
	Position center = unit->getPosition();
	Position test;
	int direction;
	bool swap;
	std::vector<Position> _trajectory;
	if (Options::getBool("strafe") && (unit->getTurretType() > -1)) {
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

	// calculate a visible units checksum - if it changed during this step, the soldier stops walking
	// the unit's Xposition * 100 + y seems a simple but unique ID for each unit
	for (std::vector<BattleUnit*>::iterator i = unit->getVisibleUnits()->begin(); i != unit->getVisibleUnits()->end(); ++i)
		visibleUnitsChecksum += (*i)->getPosition().x*100 + (*i)->getPosition().y;

	oldNumVisibleUnits = unit->getVisibleUnits()->size();

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
			for (int z = 0; z < _save->getHeight(); z++)
			{
				int distance = int(floor(sqrt(float(x*x + y*y)) + 0.5));
				test.z = z;
				if (distance <= MAX_VIEW_DISTANCE)
				{
					test.x = center.x + signX[direction]*(swap?y:x);
					test.y = center.y + signY[direction]*(swap?x:y);
					if (_save->getTile(test))
					{
						BattleUnit *visibleUnit = _save->getTile(test)->getUnit();
						if (visibleUnit && !visibleUnit->isOut() && visible(unit, _save->getTile(test)))
						{
							if ((visibleUnit->getFaction() == FACTION_HOSTILE && unit->getFaction() != FACTION_HOSTILE)
								|| (visibleUnit->getFaction() != FACTION_HOSTILE && unit->getFaction() == FACTION_HOSTILE))
							{
								unit->addToVisibleUnits(visibleUnit);
								unit->addToVisibleTiles(visibleUnit->getTile());
								if (unit->getFaction() == FACTION_PLAYER)
								{
								//	visibleUnit->getTile()->setDiscovered(true, 2);
									visibleUnit->getTile()->setVisible(+1);
								}
							}
							if (unit->getFaction() == FACTION_PLAYER)
							{
								visibleUnit->setVisible(true);
							}
							else if (unit->getFaction() == FACTION_HOSTILE && visibleUnit->getFaction() == FACTION_PLAYER && unit->getIntelligence() > visibleUnit->getTurnsExposed())
							{
								visibleUnit->setTurnsExposed(unit->getIntelligence());
								_save->updateExposedUnits();
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
									int tsize = _trajectory.size();
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

	size_t newChecksum = 0;
	for (std::vector<BattleUnit*>::iterator i = unit->getVisibleUnits()->begin(); i != unit->getVisibleUnits()->end(); ++i)
		newChecksum += (*i)->getPosition().x*100 + (*i)->getPosition().y;

	// we only react when there are at least the same amount of visible units as before AND the checksum is different
	// this way we stop if there are the same amount of visible units, but a different unit is seen
	// or we stop if there are more visible units seen
	if (visibleUnitsChecksum != newChecksum && unit->getVisibleUnits()->size() >= oldNumVisibleUnits && unit->getVisibleUnits()->size() > 0)
	{
		// a hostile unit will aggro on the new unit if it sees one - it will not start walking
		if (unit->getFaction() == FACTION_HOSTILE)
		{
			AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(unit->getCurrentAIState());
			if (aggro == 0)
			{
				aggro = new AggroBAIState(_save, unit);
				unit->setAIState(aggro);
			}
			aggro->setAggroTarget(unit->getVisibleUnits()->at(0)); // just pick the first one - maybe we need to prioritize on distance to unit or other parameters?
		}

		return true;
	}

	return false;

}


/**
 * Check for an opposing unit on this tile
 * @param currentUnit the watcher
 * @param tile the tile to check for
 * @return true/false
 */
bool TileEngine::visible(BattleUnit *currentUnit, Tile *tile)
{
	// if the tile is too dark, we can't see it
	if (!tile || tile->getShade() > MAX_DARKNESS_TO_SEE_UNITS)
	{
		return false;
	}

	// determine the origin and target voxels for the raytrace
	Position originVoxel;
	originVoxel = Position((currentUnit->getPosition().x * 16) + 7, (currentUnit->getPosition().y * 16) + 8, currentUnit->getPosition().z*24);
	originVoxel.z += -_save->getTile(currentUnit->getPosition())->getTerrainLevel();
	originVoxel.z += currentUnit->getHeight() + currentUnit->getFloatHeight() - 1; //one voxel lower (eye level)

	// fix!!! need to add flying elevation
	bool unitSeen = false;
	// for large units origin voxel is in the middle
	if (currentUnit->getArmor()->getSize() > 1)
	{
		originVoxel.x += 8;
		originVoxel.y += 8;
		originVoxel.z += 1; //topmost voxel
	}

	Position scanVoxel;
	std::vector<Position> _trajectory;
	unitSeen = canTargetUnit(&originVoxel, tile, &scanVoxel, currentUnit);

	if (unitSeen)
	{
		// now check if we really see it taking into account smoke tiles
		// initial smoke "density" of a smoke grenade is around 10 per tile
		// we do density/2 to get the decay of visibility, so in fresh smoke we only have 4 tiles of visibility
		_trajectory.clear();
		calculateLine(originVoxel, scanVoxel, true, &_trajectory, currentUnit);
		Tile *t = _save->getTile(currentUnit->getPosition());
		int maxViewDistance = MAX_VIEW_DISTANCE - (t->getSmoke()/2);
		for (unsigned int i = 0; i < _trajectory.size(); i++)
		{
			if (t != _save->getTile(Position(_trajectory.at(i).x/16,_trajectory.at(i).y/16, _trajectory.at(i).z/24)))
			{
				t = _save->getTile(Position(_trajectory.at(i).x/16,_trajectory.at(i).y/16, _trajectory.at(i).z/24));
				maxViewDistance -= t->getSmoke()/2;
			}
		}
		if (distance(currentUnit->getPosition(), tile->getPosition()) > maxViewDistance)
		{
			unitSeen = false;
		}
	}
	return unitSeen;
}

/**
 * Check for an another unit is available for targeting and what particular voxel
 * @param originVoxel voxel of trace origin (eye or gun's barrel)
 * @param tile the tile to check for
 * @param scanVoxel is returned coordinate of hit
 * @param excludeUnit is self (not to hit self)
 * @return true/false
 */
bool TileEngine::canTargetUnit(Position *originVoxel, Tile *tile, Position *scanVoxel, BattleUnit* excludeUnit)
{
	Position targetVoxel = Position((tile->getPosition().x * 16) + 7, (tile->getPosition().y * 16) + 8, tile->getPosition().z * 24);
	std::vector<Position> _trajectory;
	BattleUnit *otherUnit = tile->getUnit();
	int targetMinHeight = targetVoxel.z - tile->getTerrainLevel();
	if (otherUnit)
		 targetMinHeight += otherUnit->getFloatHeight();
	int targetMaxHeight = targetMinHeight;
	int targetCenterHeight;
	// if there is an other unit on target tile, we assume we want to check against this unit's height
	int heightRange;
 
	if (otherUnit == 0) return false; //no unit in this tile, even if it elevated and appearing in it.
	if (otherUnit == excludeUnit) return false; //skip self
	int unitRadius = otherUnit->getLoftemps(); //width == loft in default loftemps set
	if (otherUnit->getArmor()->getSize() > 1)
	{
		unitRadius = 6;
	}
	int sliceTargets[10]={0,0, 0,unitRadius, 0,-unitRadius, unitRadius,0, -unitRadius,0};
 
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
			scanVoxel->x=targetVoxel.x + sliceTargets[j*2];
			scanVoxel->y=targetVoxel.y + sliceTargets[j*2+1];
			_trajectory.clear();
			int test = calculateLine(*originVoxel, *scanVoxel, false, &_trajectory, excludeUnit, true, true);
			if (test == 4)
			{
				//voxel of hit must be inside of scanned box
				if (_trajectory.at(0).x/16 == scanVoxel->x/16 &&
					_trajectory.at(0).y/16 == scanVoxel->y/16 &&
					_trajectory.at(0).z >= targetMinHeight &&
					_trajectory.at(0).z <= targetMaxHeight)
				{
					return true;
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
			int test = calculateLine(*originVoxel, *scanVoxel, false, &_trajectory, excludeUnit, true, true);
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
		if (distance(position, (*i)->getPosition()) < 20 && (*i)->getFaction() == _save->getSide())
		{
			calculateFOV(*i);
		}
	}
}

/**
 * Checks if of the opposing faction a sniper sees this unit. The unit with the highest reaction score will be compared with the current unit's reaction score.
 * If it's higher, a shot is fired when enough time units a weapon and ammo available.
 * @param unit
 * @param action
 * @param potentialVictim The unit that is targeted when shot.
 * @param recalculateFOV Whether to recalculate first the FOV of the units within range.
 */
bool TileEngine::checkReactionFire(BattleUnit *unit, BattleAction *action, BattleUnit *potentialVictim, bool recalculateFOV)
{
	double highestReactionScore = 0;
	action->actor = 0;

	// reaction fire only triggered when the actioning unit is of the currently playing side
	if (unit->getFaction() != _save->getSide())
	{
		return false;
	}

	if (potentialVictim && RNG::generate(0, 4) == 1 && potentialVictim->getFaction() == FACTION_HOSTILE)
	{
		potentialVictim->lookAt(unit->getPosition());
		while (potentialVictim->getStatus() == STATUS_TURNING)
		{
			recalculateFOV = true;
			potentialVictim->turn();
		}
		// if the potentialVictim is hostile, he will aggro if he wasn't already or at least change aggro target
		if (potentialVictim->getFaction() == FACTION_HOSTILE)
		{
			AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(potentialVictim->getCurrentAIState());
			if (aggro == 0)
			{
				aggro = new AggroBAIState(_save, potentialVictim);
				potentialVictim->setAIState(aggro);
			}
			aggro->setAggroTarget(unit);
		}

	}

	// we reset the unit to false here - if it is seen by any unit in range below the unit becomes visible again
	//unit->setVisible(false);

	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if (distance(unit->getPosition(), (*i)->getPosition()) < 19 && (*i)->getFaction() != _save->getSide() && !(*i)->isOut())
		{
			if (recalculateFOV)
			{
				calculateFOV(*i);
			}
			for (std::vector<BattleUnit*>::iterator j = (*i)->getVisibleUnits()->begin(); j != (*i)->getVisibleUnits()->end(); ++j)
			{
				if ((*j) == unit && (*i)->getReactionScore() > highestReactionScore && (*i)->getMainHandWeapon())
				{
					// I see you!
					highestReactionScore = (*i)->getReactionScore();
					action->actor = (*i);
				}
			}
		}
	}

	if (action->actor && highestReactionScore > unit->getReactionScore() &&
		(action->actor->getMainHandWeapon()->getRules()->getTUSnap() ||
		(action->actor->getMainHandWeapon()->getRules()->getBattleType() == BT_MELEE &&
		validMeleeRange(action->actor, unit))))
	{
		action->actor->addReactionExp();
		action->type = BA_SNAPSHOT;
		action->target = unit->getPosition();
		// lets try and shoot: we need a weapon, ammo and enough time units
		action->weapon = action->actor->getMainHandWeapon();
		int tu = action->actor->getActionTUs(action->type, action->weapon);
		action->TU = tu;
		if (action->weapon && action->weapon->getAmmoItem() && action->weapon->getAmmoItem()->getAmmoQuantity() && action->actor->getTimeUnits() >= tu)
		{
			action->targeting = true;
			// if the target is hostile, it will aggro
			if (unit->getFaction() == FACTION_HOSTILE)
			{
				AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(unit->getCurrentAIState());
				if (aggro == 0)
				{
					aggro = new AggroBAIState(_save, unit);
					unit->setAIState(aggro);
				}
				aggro->setAggroTarget(action->actor);
			}
			return true;
		}
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
	int part = voxelCheck(center, unit);
	if (part >= 0 && part <= 3)
	{
		// power 25% to 75%
		int rndPower = RNG::generate(power/4, (power*3)/4); //RNG::boxMuller(power, power/6)
		if (tile->damage(part, rndPower))
			_save->setObjectiveDestroyed(true);
	}
	else if (part == 4)
	{
		// power 0 - 200%
		int rndPower = RNG::generate(0, power*2); // RNG::boxMuller(power, power/3)
		if (bu)
		{
			bu->damage(Position(center.x%16, center.y%16, center.z%24 + tile->getTerrainLevel()), rndPower, type);
		}
		else
		{
			// it's possible we have a unit below the actual tile, when he stands on a stairs and sticks his head out to the next tile
			Tile *below = _save->getTile(Position(center.x/16, center.y/16, (center.z/24)-1));
			if (below)
			{
				BattleUnit *buBelow = below->getUnit();
				if (buBelow)
				{
					buBelow->damage(Position(center.x%16, center.y%16, center.z%24 + below->getTerrainLevel() + 24), rndPower, type);
					bu = buBelow;
				}
			}
		}

		// conventional weapons can cause additional stun damage
		if (type == DT_AP && bu)
		{
			bu->damage(Position(center.x%16, center.y%16, center.z%24), RNG::generate(0, rndPower/4), DT_STUN, true);
		}

		if (bu && bu->getFaction() != unit->getFaction() && type != DT_NONE)
		{
			unit->addFiringExp();
		}
	}
	applyItemGravity(tile);
	calculateSunShading(); // roofs could have been destroyed
	calculateFOV(center);
	calculateTerrainLighting(); // fires could have been started
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

			Tile *origin = _save->getTile(center);
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
				if (l != 0) // no need to block epicentrum
				{
					power_ -= (horizontalBlockage(origin, dest, type) + verticalBlockage(origin, dest, type)) * 2;
					power_ -= 10; // explosive damage decreases by 10 per tile
					if (origin->getPosition().z != tileZ) power_ -= vertdec; //3d explosion factor
				}

				if (power_ > 0)
				{
					if (type == DT_HE)
					{
						// explosives do 1/2 damage to terrain and 1/2 up to 3/2 random damage to units
						dest->setExplosive(power_ / 2);
					}

					ret = tilesAffected.insert(dest); // check if we had this tile already
					if (ret.second)
					{
						if (type == DT_HE || type == DT_STUN)
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
								dest->addSmoke(RNG::generate(power_/10, 14));
							}
						}

						if (type == DT_IN && !dest->isVoid())
						{
							if (dest->getFire() == 0)
							{
								dest->ignite();
							}
							if (dest->getUnit())
							{
								dest->getUnit()->damage(Position(0, 0, 0), RNG::generate(0, power_/3), type); // immediate IN damage
								dest->getUnit()->setFire(RNG::generate(1, 5)); // catch fire and burn for 1-5 rounds
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
			applyItemGravity(*i);
		}
	}

	calculateSunShading(); // roofs could have been destroyed
	calculateFOV(center);
	calculateTerrainLighting(); // fires could have been started
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
		// explosions create smoke which only stays 1 or 2 turns
		tile->addSmoke(1);
		for (int i = 0; i < 7; ++i)
		{
			if(tiles[i] && tiles[i]->getMapData(parts[i]))
			{
				int armor = tiles[i]->getMapData(parts[i])->getArmor();
				if (explosive >= armor)
				{
					objective = tiles[i]->destroy(parts[i]);
					if (i > 3) tiles[i]->addSmoke(2); //only current tile produces smoke[2]
					if (tiles[i]->getMapData(parts[i]) && explosive >= 2 * armor) //double destruction
					{
						tiles[i]->destroy(parts[i]);
					}
				}
			}
		}

		// flammable of the tile needs to be 20 or lower (lower is better chance of catching fire) to catch fire
		// note that when we get here, flammable objects can already be destroyed by the explosion, thus not catching fire.
		int flam = tile->getFlammability();
		if (flam <= 20)
		{
			if (RNG::generate(0, 20) - flam >= 0)
			{
				tile->ignite();
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

	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
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

	int direction = vectorToDirection(endTile->getPosition() - startTile->getPosition());
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
			block += blockage(_save->getTile(startTile->getPosition() + oneTileNorth), MapData::O_OBJECT, type);
			if (block == 0) break; //this way is opened
			block = blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_NORTHWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_WESTWALL, type); //right+up
			block += blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_OBJECT, type);
}
		else
		{
			block = (blockage(startTile,MapData::O_NORTHWALL, type) + blockage(endTile,MapData::O_WESTWALL, type))/2
				+ (blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_NORTHWALL, type))/2;
			if (type == DT_HE)
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type) +
				blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type))/2;
			else
			block = (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type) +
				  blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type)) < 510?0:255;
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
			block += blockage(_save->getTile(startTile->getPosition() + oneTileSouth), MapData::O_OBJECT, type);
			if (block == 0) break; //this way is opened
			block = blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_WESTWALL, type)
				+ blockage(endTile, MapData::O_NORTHWALL, type); //right+down
			block += blockage(_save->getTile(startTile->getPosition() + oneTileEast), MapData::O_OBJECT, type);
		}
		else
		{
			block = (blockage(endTile,MapData::O_WESTWALL, type) + blockage(endTile,MapData::O_NORTHWALL, type))/2
				+ (blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_NORTHWALL, type))/2;
			if (type == DT_HE)
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type))/2;
			else
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type)) < 510?0:255;
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
			block += blockage(_save->getTile(startTile->getPosition() + oneTileSouth), MapData::O_OBJECT, type);
			if (block == 0) break; //this way is opened
			block = blockage(startTile, MapData::O_WESTWALL, type) + blockage(endTile, MapData::O_NORTHWALL, type); //left+down
			block += blockage(_save->getTile(startTile->getPosition() + oneTileWest), MapData::O_OBJECT, type);
		}
		else
		{
			block = (blockage(endTile,MapData::O_NORTHWALL, type) + blockage(startTile,MapData::O_WESTWALL, type))/2
				+ (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_NORTHWALL, type))/2;
			if (type == DT_HE)
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type))/2;
			else
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type)) < 510?0:255;
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
			block += blockage(_save->getTile(startTile->getPosition() + oneTileNorth), MapData::O_OBJECT, type);
			if (block == 0) break; //this way is opened
			block = blockage(startTile, MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileWest), MapData::O_NORTHWALL, type); //left+up
			block += blockage(_save->getTile(startTile->getPosition() + oneTileWest), MapData::O_OBJECT, type);
		}
		else
		{
			block = (blockage(startTile,MapData::O_WESTWALL, type) + blockage(startTile,MapData::O_NORTHWALL, type))/2
				+ (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_WESTWALL, type)
				+ blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_NORTHWALL, type))/2;
			if (type == DT_HE)
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type))/2;
			else
			block += (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type) +
					  blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type)) < 510?0:255;
		}
		break;
	}

	block += blockage(startTile,MapData::O_OBJECT, type);

	return block;
}



/*
 * The amount this certain wall or floor-part of the tile blocks.
 * @param startTile The tile where the power starts.
 * @param part The part of the tile the power needs to go through.
 * @param type The type of power/damage.
 * @return amount of blockage
 */
int TileEngine::blockage(Tile *tile, const int part, ItemDamageType type)
{
	int blockage = 0;

	if (tile == 0) return 0; // probably outside the map here

	if (tile->getMapData(part))
	{
		blockage += tile->getMapData(part)->getBlock(type);
	}

	// open ufo doors are actually still closed behind the scenes
	// so a special trick is needed to see if they are open, if they are, they obviously don't block anything
	if (tile->isUfoDoorOpen(part))
		blockage = 0;

	return blockage;
}



/*
 * Converts direction to a vector. Direction starts north = 0 and goes clockwise.
 * @param vector pointer to a position (which acts as a vector)
 * @return direction
 */
int TileEngine::vectorToDirection(const Position &vector)
{
	int x[8] = {0, 1, 1, 1, 0, -1, -1, -1};
	int y[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
	for (int i = 0; i < 8; ++i)
	{
		if (x[i] == vector.x && y[i] == vector.y)
			return i;
	}
	return -1;
}

/**
 * Soldier opens a door (if any) by rightclick, or by walking through it. The unit has to face in the right direction.
 * @param unit
 * @return -1 there is no door, you can walk through.
 *		  0 normal door opened, make a squeaky sound and you can walk through.
 *		  1 ufo door is starting to open, make a whoosh sound, don't walk through.
 *		  3 ufo door is still opening, don't walk through it yet. (have patience, futuristic technology...)
 */
int TileEngine::unitOpensDoor(BattleUnit *unit, bool rClick)
{
	int door = -1;
	int TUCost = 0;
	int size = unit->getArmor()->getSize();

	if (size > 1 && rClick)
		return door;

	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			Tile *tile = _save->getTile(unit->getPosition() + Position(x,y,0));
			if (!tile) continue;

			if (unit->getDirection() == 0 || unit->getDirection() == 1 || unit->getDirection() == 7) // north, northeast or northwest
			{
				door = tile->openDoor(MapData::O_NORTHWALL, unit, _save->getDebugMode());
				if (door == 0 && rClick)
					TUCost = tile->getTUCost(MapData::O_WESTWALL, unit->getArmor()->getMovementType());
				if (door == 1)
				{
					TUCost = tile->getTUCost(MapData::O_NORTHWALL, unit->getArmor()->getMovementType());
					// check for adjacent door(s)
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(1, 0, 0));
					if (tile) tile->openDoor(MapData::O_NORTHWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(-1, 0, 0));
					if (tile) tile->openDoor(MapData::O_NORTHWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(2, 0, 0));
					if (tile) tile->openDoor(MapData::O_NORTHWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(-2, 0, 0));
					if (tile) tile->openDoor(MapData::O_NORTHWALL);
				}
			}
			if ((unit->getDirection() == 2 || unit->getDirection() == 1 || unit->getDirection() == 3) && door == -1) // east, northeast or southeast
			{
				tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(1, 0, 0));
				if (tile)
				{
					door = tile->openDoor(MapData::O_WESTWALL, unit, _save->getDebugMode());
				}
				else
				{
					tile = _save->getTile(unit->getPosition() + Position(x,y,0));
				}
				if (door == 0 && rClick)
					TUCost = tile->getTUCost(MapData::O_NORTHWALL, unit->getArmor()->getMovementType());
				if (door == 1)
				{
					TUCost = tile->getTUCost(MapData::O_WESTWALL, unit->getArmor()->getMovementType());
					// check for adjacent door(s)
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(1, 1, 0));
					if (tile) tile->openDoor(MapData::O_WESTWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(1, -1, 0));
					if (tile) tile->openDoor(MapData::O_WESTWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(1, 2, 0));
					if (tile) tile->openDoor(MapData::O_WESTWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(1, -2, 0));
					if (tile) tile->openDoor(MapData::O_WESTWALL);
				}
			}
			if ((unit->getDirection() == 4 || unit->getDirection() == 5 || unit->getDirection() == 3) && door == -1) // south, southwest or southeast
			{
				tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(0, 1, 0));
				if (tile)
				{
					door = tile->openDoor(MapData::O_NORTHWALL, unit, _save->getDebugMode());
				}
				else
				{
					tile = _save->getTile(unit->getPosition() + Position(x,y,0));
				}
				if (door == 0 && rClick)
					TUCost = tile->getTUCost(MapData::O_WESTWALL, unit->getArmor()->getMovementType());
				if (door == 1)
				{
					TUCost = tile->getTUCost(MapData::O_NORTHWALL, unit->getArmor()->getMovementType());
					// check for adjacent door(s)
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(1, 1, 0));
					if (tile) tile->openDoor(MapData::O_NORTHWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(-1, 1, 0));
					if (tile) tile->openDoor(MapData::O_NORTHWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(2, 1, 0));
					if (tile) tile->openDoor(MapData::O_NORTHWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(-2, 1, 0));
					if (tile) tile->openDoor(MapData::O_NORTHWALL);
				}
			}
			if ((unit->getDirection() == 6 || unit->getDirection() == 5 || unit->getDirection() == 7) && door == -1) // west, southwest or northwest
			{
				door = tile->openDoor(MapData::O_WESTWALL, unit, _save->getDebugMode());
				if (door == 0 && rClick)
					TUCost = tile->getTUCost(MapData::O_NORTHWALL, unit->getArmor()->getMovementType());
				if (door == 1)
				{
					TUCost = tile->getTUCost(MapData::O_WESTWALL, unit->getArmor()->getMovementType());
					// check for adjacent door(s)
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(0, 1, 0));
					if (tile) tile->openDoor(MapData::O_WESTWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(0, -1, 0));
					if (tile) tile->openDoor(MapData::O_WESTWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(0, 2, 0));
					if (tile) tile->openDoor(MapData::O_WESTWALL);
					tile = _save->getTile(unit->getPosition() + Position(x,y,0) + Position(0, -2, 0));
					if (tile) tile->openDoor(MapData::O_WESTWALL);
				}
			}
		}
	}


	if (door == 0 || door == 1)
	{

		unit->spendTimeUnits(TUCost);
		calculateFOV(unit->getPosition());
	}

	return door;
}

/**
 * Close ufo doors.
 * @return whether doors are closed.
 */
int TileEngine::closeUfoDoors()
{
	int doorsclosed = 0;

	// prepare a list of tiles on fire/smoke & close any ufo doors
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
	{
		doorsclosed += _save->getTiles()[i]->closeUfoDoor();
	}

	return doorsclosed;
}
/**
 * calculateLine. Using bresenham algorithm in 3D.
 * @param origin
 * @param target
 * @param storeTrajectory true will store the whole trajectory - otherwise it just stores the last position.
 * @param trajectory A vector of positions in which the trajectory is stored.
 * @param excludeUnit Excludes this unit in the collision detection.
 * @param doVoxelCheck Check against voxel or tile blocking? (first one for units visibility and line of fire, second one for terrain visibility)
 * @return the objectnumber(0-3) or unit(4) or out of map (5) or -1(hit nothing)
 */
int TileEngine::calculateLine(const Position& origin, const Position& target, bool storeTrajectory, std::vector<Position> *trajectory, BattleUnit *excludeUnit, bool doVoxelCheck, bool LOSCalc)
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

		if (storeTrajectory)
		{
			trajectory->push_back(Position(cx, cy, cz));
		}
		//passes through this point?
		if (doVoxelCheck)
		{
			result = voxelCheck(Position(cx, cy, cz), excludeUnit);
			if (result != -1)
			{
				if (trajectory != 0)
				{ // store the position of impact
					trajectory->push_back(Position(cx, cy, cz));
				}
				return result;
			}
		}
		else
		{
			result = horizontalBlockage(_save->getTile(lastPoint), _save->getTile(Position(cx, cy, cz)), DT_NONE)
					   + verticalBlockage(_save->getTile(lastPoint), _save->getTile(Position(cx, cy, cz)), DT_NONE);
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
				result = voxelCheck(Position(cx, cy, cz), excludeUnit);
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
				result = voxelCheck(Position(cx, cy, cz), excludeUnit);
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
		if (storeTrajectory)
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
 * Check if we hit a voxel.
 * @param voxel The voxel to check.
 * @param excludeUnit Don't do checks on this unit.
 * @param excludeAllUnits Don't do checks on any unit.
 * @return the objectnumber(0-3) or unit(4) or out of map (5) or -1(hit nothing)
 */
int TileEngine::voxelCheck(const Position& voxel, BattleUnit *excludeUnit, bool excludeAllUnits)
{

	Tile *tile = _save->getTile(Position(voxel.x/16, voxel.y/16, voxel.z/24));
	// check if we are not out of the map
	if (tile == 0 || voxel.x < 0 || voxel.y < 0 || voxel.z < 0)
	{
		return 5;
	}

	if (!excludeAllUnits)
	{
		BattleUnit *unit = tile->getUnit();
		if (unit != 0 && unit != excludeUnit)
		{
			if ((voxel.z%24) < (unit->getHeight() + unit->getFloatHeight() + (-tile->getTerrainLevel()))
				&& (voxel.z%24) > (1 + unit->getFloatHeight() + (-tile->getTerrainLevel())))
			{
				int x = voxel.x%16;
				int y = voxel.y%16;
				int part = 0;
				if (unit->getArmor()->getSize() > 1)
				{
					if (tile->getPosition().x - unit->getPosition().x == 1)
					{
						part = 1;
					}
					if (tile->getPosition().y - unit->getPosition().y == 1)
					{
						part = 2;
					}
					if (tile->getPosition().x - unit->getPosition().x == 1 && tile->getPosition().y - unit->getPosition().y == 1)
					{
						part = 3;
					}
				}
				int idx = (unit->getLoftemps(part) * 16) + y;
				if (_voxelData->at(idx) & (1 << x))
				{
					return 4;
				}
			}
		}
		// sometimes there is unit on the tile below, but sticks up to this tile with his head
		Tile *below = _save->getTile(Position(voxel.x/16, voxel.y/16, (voxel.z/24)-1));
		if (below)
		{
			BattleUnit *unit = below->getUnit();
			if (unit != 0 && unit != excludeUnit)
			{
				if ((voxel.z%24) < (unit->getHeight()+unit->getFloatHeight()+(-tile->getTerrainLevel()))-24
					&& (voxel.z%24) > (unit->getFloatHeight()+(-tile->getTerrainLevel())-24))
				{
					int x = voxel.x%16;
					int y = voxel.y%16;
					int part = 0;
					if (unit->getArmor()->getSize() > 1)
					{
						if (below->getPosition().x - unit->getPosition().x == 1)
						{
							part = 1;
						}
						if (below->getPosition().y - unit->getPosition().y == 1)
						{
							part = 2;
						}
						if (below->getPosition().x - unit->getPosition().x == 1 && below->getPosition().y - unit->getPosition().y == 1)
						{
							part = 3;
						}
					}
					int idx = (unit->getLoftemps(part) * 16) + y;
					if (_voxelData->at(idx) & (1 << x))
					{
						return 4;
					}
				}
			}
		}
	}

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
	int x = abs(pos1.x - pos2.x);
	int y = abs(pos1.y - pos2.y);
	return int(floor(sqrt(float(x*x + y*y)) + 0.5));
}


/**
 * Psionic attack mechanism.
 * @param action
 * @return whether it failed or succeeded
 */
bool TileEngine::psiAttack(BattleAction *action)
{
	BattleUnit *victim = _save->getTile(action->target)->getUnit();
	double attackStrength = action->actor->getStats()->psiStrength * action->actor->getStats()->psiSkill / 50;
	double defenseStrength = victim->getStats()->psiStrength + 30 + (victim->getStats()->psiSkill / 5);
	int d = distance(action->actor->getPosition(), action->target);
	attackStrength -= d/2;
	attackStrength += RNG::generate(0,55);
	if (action->type == BA_PANIC)
	{
		if (attackStrength > defenseStrength)
		{
			action->actor->addPsiExp();
			action->actor->addPsiExp();
			action->actor->addPsiExp();
			int moraleLoss = (110-_save->getTile(action->target)->getUnit()->getStats()->bravery);
			if (moraleLoss > 0)
			_save->getTile(action->target)->getUnit()->moraleChange(-moraleLoss);
			return true;
		}
		else
		{
			action->actor->addPsiExp();
			return false;
		}
	}
	else if (action->type == BA_MINDCONTROL)
	{
		if (attackStrength > defenseStrength)
		{
			action->actor->addPsiExp();
			action->actor->addPsiExp();
			action->actor->addPsiExp();
			victim->convertToFaction(action->actor->getFaction());
			calculateFOV(victim);
			if (action->actor->getFaction() == FACTION_PLAYER)
				_save->setSelectedUnit(victim);
			victim->setTimeUnits(victim->getStats()->tu);
			return true;
		}
		else
		{
			action->actor->addPsiExp();
			return false;
		}
	}
	return false;
}

/**
 * Apply gravity to a tile. Causes items to drop.
 * @param t Tile
 * @return Tile where the items end up in eventually.
 */
Tile *TileEngine::applyItemGravity(Tile *t)
{
	if (t->getInventory()->size() == 0 && !t->getUnit()) return t; // skip this if there are no items

	Position p = t->getPosition();
	Tile *rt = t;
	BattleUnit *occupant = t->getUnit();

	if (occupant && occupant->getArmor()->getMovementType() != MT_FLY)
	{
		Tile *rt1 = _save->getTile(p);
		if (occupant->getArmor()->getSize() >= 2)
		{
			Position xOffset (1,0,0);
			Position yOffset (0,1,0);
			Tile *rt2 = _save->getTile(p + xOffset);
			Tile *rt3 = _save->getTile(p + yOffset);
			Tile *rt4 = _save->getTile(p + xOffset + yOffset);
		
			while (rt1->getMapData(MapData::O_FLOOR) == 0 && rt2->getMapData(MapData::O_FLOOR) == 0 && rt3->getMapData(MapData::O_FLOOR) == 0 && rt4->getMapData(MapData::O_FLOOR) == 0 && p.z > 0)
			{
				p.z--;
				rt1 = _save->getTile(p);
				rt2 = _save->getTile(p + xOffset);
				rt3 = _save->getTile(p + yOffset);
				rt4 = _save->getTile(p + xOffset + yOffset);
			}
		}
		else
		{
			while (rt1->getMapData(MapData::O_FLOOR) == 0 && p.z > 0)
			{
				p.z--;
				rt1 = _save->getTile(p);
			}
		}
		if (t != rt1)
		{
			occupant->startWalking(Pathfinding::DIR_DOWN, rt1->getPosition(), rt1, occupant->getVisible());
			_save->addFallingUnit(occupant);
			_save->setUnitsFalling(true);
		}
	}

	while (rt->getMapData(MapData::O_FLOOR) == 0 && p.z > 0)
	{
		p.z--;
		rt = _save->getTile(p);
	}

	if (t != rt)
	{
		// copy items
		for (std::vector<BattleItem*>::iterator it = t->getInventory()->begin(); it != t->getInventory()->end(); ++it)
		{
			rt->addItem(*it, (*it)->getSlot());
		}

		// clear tile
		t->getInventory()->clear();
	}

	return rt;
}
/*
 * Validate the melee range between two units.
 * @param *unit the attacking unit.
 * @param *target the unit we want to attack.
 * @return true when range is valid.
 */
bool TileEngine::validMeleeRange(BattleUnit *unit, BattleUnit *target)
{
	return validMeleeRange(unit->getPosition(), unit->getDirection(), unit->getArmor()->getSize(), unit->getHeight(), target);
}

/*
 * Validate the melee range between a tile and a unit.
 * @param pos Position to check from.
 * @param direction direction to check, -1 for all.
 * @param size for large units, we have to do extra checks.
 * @param height units on stairs might protrude into the tile above.
 * @param *target the unit we want to attack, 0 for any unit.
 * @return true when range is valid.
 */
bool TileEngine::validMeleeRange(Position pos, int direction, int size, int height, BattleUnit *target)
{
	Position p;
	
	if (direction == -1)
	{
		for (int dir = 0; dir <= 7; ++dir)
		{
			Pathfinding::directionToVector(dir, &p);
			for (int x = 0; x <= size-1; ++x)
			{
				for (int y = 0; y <= size-1; ++y)
				{
					Tile * tile (_save->getTile(Position(pos + Position(x, y, 0) + p)));
					if (tile)
					{
						if (height - _save->getTile(pos)->getTerrainLevel() > 24 && ((tile->getUnit() && tile->getUnit() != target) || !tile->getUnit()))
							tile = _save->getTile(tile->getPosition() + Position(0, 0, 1));
						if (tile->getUnit() && ((target != 0 && tile->getUnit() == target ) || (target == 0)))
						{
							if (!_save->getPathfinding()->isBlocked(_save->getTile(pos + Position(x, y, 0)), tile, dir, 0))
								return true;
						}
					}
				}
			}
		}
	}
	else
	{
		Pathfinding::directionToVector(direction, &p);
		for (int x = 0; x <= size-1; ++x)
		{
			for (int y = 0; y <= size-1; ++y)
			{
				Tile * tile (_save->getTile(Position(pos + Position(x, y, 0) + p)));
				if (tile)
				{
					if (height - _save->getTile(pos)->getTerrainLevel() > 24 && ((tile->getUnit() && tile->getUnit() != target) || !tile->getUnit()))
						tile = _save->getTile(tile->getPosition() + Position(0, 0, 1));
					if (tile->getUnit() && ((target != 0 && tile->getUnit() == target ) || (target == 0)))
					{
						if (!_save->getPathfinding()->isBlocked(_save->getTile(pos + Position(x, y, 0)), tile, direction, 0))
							return true;
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


}
