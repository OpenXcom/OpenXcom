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
#define _USE_MATH_DEFINES
#include <cmath>
#include <set>
#include "TileEngine.h"
#include "SDL.h"
#include "BattleAIState.h"
#include "AggroBAIState.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Alien.h"
#include "../Engine/RNG.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/MapData.h"
#include "../Ruleset/RuleAlien.h"
#include "../Ruleset/RuleSoldier.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{

/**
 * Sets up a TileEngine.
 * @param save pointer to SavedBattleGame object.
 */
TileEngine::TileEngine(SavedBattleGame *save, std::vector<Uint16> *voxelData) : _save(save), _voxelData(voxelData)
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
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
	{
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

	// At night/dusk sun isn't dropping shades
	if (_save->getGlobalShade() <= 5)
	{
		if (verticalBlockage(_save->getTile(Position(tile->getPosition().x, tile->getPosition().y, _save->getHeight() - 1)), tile, DT_NONE))
		{
			power-=2;
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

	// add lighting of soldiers
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if ((*i)->getFaction() == FACTION_PLAYER && !(*i)->isOut())
		{
			addLight((*i)->getPosition(), personalLightPower, layer);
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
	int visibleUnitsChecksum = 0;
	Position center = unit->getPosition();
	Position test;
	bool swap = (unit->getDirection()==0 || unit->getDirection()==4);
	int signX[8] = { +1, +1, +1, +1, -1, -1, -1, -1 };
	int signY[8] = { +1, +1, +1, -1, -1, -1, +1, +1 };
	int y1, y2;

	// calculate a visible units checksum - if it changed during this step, the soldier stops walking
	for (std::vector<BattleUnit*>::iterator i = unit->getVisibleUnits()->begin(); i != unit->getVisibleUnits()->end(); ++i)
		visibleUnitsChecksum += (*i)->getId()+1;

	unit->clearVisibleUnits();

	for (int x = 0; x <= MAX_VIEW_DISTANCE; ++x)
	{
		if (unit->getDirection()%2)
		{
			y1 = 0;
			y2 = MAX_VIEW_DISTANCE - x;
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
					test.x = center.x + signX[unit->getDirection()]*(swap?y:x);
					test.y = center.y + signY[unit->getDirection()]*(swap?x:y);
					if (_save->getTile(test))
					{
						checkIfUnitVisible(unit, _save->getTile(test)->getUnit());
						if (unit->getFaction() == FACTION_PLAYER)
							calculateLine(unit->getPosition(), test, false, 0, unit, false);
					}
				}
			}
		}
	}
	
	int newChecksum = 0;
	for (std::vector<BattleUnit*>::iterator i = unit->getVisibleUnits()->begin(); i != unit->getVisibleUnits()->end(); ++i)
		newChecksum += (*i)->getId()+1;

	return visibleUnitsChecksum < newChecksum;

}


/**
 * Check for an opposing unit on this tile
 * @param currentUnit the watcher
 * @param otherUnit the unit to check for
 */
bool TileEngine::checkIfUnitVisible(BattleUnit *currentUnit, BattleUnit *otherUnit)
{
	// only check if unit is alive
	if (otherUnit == 0 || otherUnit->isOut())
	{
		return false;
	}

	// only check for opposing factions
	if (currentUnit->getFaction() == FACTION_PLAYER && (otherUnit->getFaction() == FACTION_PLAYER || otherUnit->getFaction() == FACTION_NEUTRAL))
	{
		return false;
	}

	if (currentUnit->getFaction() == FACTION_HOSTILE && otherUnit->getFaction() == FACTION_HOSTILE)
	{
		return false;
	}

	// if the tile is too dark, we can't see the unit
	if (_save->getTile(otherUnit->getPosition())->getShade() > MAX_DARKNESS_TO_SEE_UNITS)
	{
		return false;
	}

	// determine the origin and target voxels for the raytrace
	Position originVoxel, targetVoxel;
	std::vector<Position> _trajectory;
	originVoxel = Position((currentUnit->getPosition().x * 16) + 8, (currentUnit->getPosition().y * 16) + 8, currentUnit->getPosition().z*24);
	originVoxel.z += -_save->getTile(currentUnit->getPosition())->getTerrainLevel();
	originVoxel.z += currentUnit->getHeight();
	bool unitSeen = false;

	targetVoxel = Position((otherUnit->getPosition().x * 16) + 8, (otherUnit->getPosition().y * 16) + 8, otherUnit->getPosition().z*24);
	int targetMinHeight = targetVoxel.z - _save->getTile(otherUnit->getPosition())->getTerrainLevel();
	int targetMaxHeight = targetMinHeight + otherUnit->getHeight();

	// scan ray from top to bottom
	for (int i = targetMaxHeight; i > targetMinHeight; i-=2)
	{
		targetVoxel.z = i;
		_trajectory.clear();
		int test = calculateLine(originVoxel, targetVoxel, false, &_trajectory, currentUnit);
		if (test == 4)
		{
			Position hitPosition = Position(_trajectory.at(0).x/16, _trajectory.at(0).y/16, _trajectory.at(0).z/24);
			if (otherUnit->getPosition() == hitPosition)
			{
				unitSeen = true;
				break;
			}
		}
		if (test == -1)
		{
			unitSeen = true;
			break;
		}
	}

	if (unitSeen)
	{
		// now check if we really see it taking into account smoke tiles
		// initial smoke "density" of a smoke grenade is around 10 per tile
		// we do density/2 to get the decay of visibility, so in fresh smoke we only have 4 tiles of visibility
		_trajectory.clear();
		calculateLine(originVoxel, targetVoxel, true, &_trajectory, currentUnit);
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
		int x = abs(currentUnit->getPosition().x - otherUnit->getPosition().x);
		int y = abs(currentUnit->getPosition().y - otherUnit->getPosition().y);
		int distance = int(floor(sqrt(float(x*x + y*y)) + 0.5));
		if (distance <= maxViewDistance)
		{
			currentUnit->addToVisibleUnits(otherUnit);
		}
		else
		{
			unitSeen = false;
		}
	}

	return unitSeen;
}

/**
 * Calculates line of sight of a soldiers within range of the Position
 * (used when terrain has changed, which can reveil new parts of terrain or units)
 * @param position Position of the changed terrain.
 */
void TileEngine::calculateFOV(const Position &position)
{
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		int x = abs(position.x - (*i)->getPosition().x);
		int y = abs(position.y - (*i)->getPosition().y);
		int distance = int(floor(sqrt(float(x*x + y*y)) + 0.5));

		if (distance < 20 && (*i)->getFaction() == _save->getSide())
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
 * @param potentialVictim The unit that is targetted when shot.
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

	// from time to time just don't do a reaction shot
	// (implemented this after doing lots of test cycles - in the original x-com on average we get to see far less reaction shots although the formula used is the one on UFOPAEDIA)
	if (RNG::generate(0, 4) == 1)
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
	}

	// we reset the unit to false here - if it is seen by any unit in range below the unit becomes visible again
	unit->setVisible(false);

	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		int x = abs(unit->getPosition().x - (*i)->getPosition().x);
		int y = abs(unit->getPosition().y - (*i)->getPosition().y);
		int distance = int(floor(sqrt(float(x*x + y*y)) + 0.5));
		if (distance < 19 && (*i)->getFaction() != _save->getSide() && !(*i)->isOut())
		{
			if (recalculateFOV)
			{
				calculateFOV(*i);
			}
			for (std::vector<BattleUnit*>::iterator j = (*i)->getVisibleUnits()->begin(); j != (*i)->getVisibleUnits()->end(); ++j)
			{
				if ((*j) == unit && (*i)->getReactionScore() > highestReactionScore)
				{
					// I see you!
					highestReactionScore = (*i)->getReactionScore();
					action->actor = (*i);
				}
			}
		}
	}

	if (action->actor && highestReactionScore > unit->getReactionScore())
	{
		action->actor->addReactionExp();
		action->type = BA_SNAPSHOT;
		action->target = unit->getPosition();
		// lets try and shoot
		action->weapon = action->actor->getMainHandWeapon();
		int tu = action->actor->getActionTUs(action->type, action->weapon);
		if (action->weapon && action->weapon->getAmmoItem() && action->weapon->getAmmoItem()->getAmmoQuantity())
		{
			if (action->actor->spendTimeUnits(tu, _save->getDebugMode()))
			{
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
				// if the shooter is hostile, he will aggro
				if (action->actor->getFaction() == FACTION_HOSTILE)
				{
					AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(action->actor->getCurrentAIState());
					if (aggro == 0)
					{
						aggro = new AggroBAIState(_save, action->actor);
						action->actor->setAIState(aggro);
					}
					aggro->setAggroTarget(unit);
				}
				return true;
			}
		}
	}

	return false;
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
	if (type == DT_AP || type == DT_PLASMA || type == DT_LASER)
	{
		int part = voxelCheck(center, unit);
		if (part >= 0 && part <= 3)
		{
			// power 25% to 75%
			int rndPower = RNG::generate(power/4, (power*3)/4); //RNG::boxMuller(power, power/6)
			_save->getTile(Position(center.x/16, center.y/16, center.z/24))->damage(part, rndPower);
		}
		else if (part == 4)
		{
			// power 0 - 200%
			int rndPower = RNG::generate(0, power*2); // RNG::boxMuller(power, power/3)
			_save->getTile(Position(center.x/16, center.y/16, center.z/24))->getUnit()->damage(Position(center.x%16, center.y%16, center.z%24), rndPower);

			// conventional weapons can cause additional stun damage
			if (type == DT_AP)
			{
				_save->getTile(Position(center.x/16, center.y/16, center.z/24))->getUnit()->stun(RNG::generate(0, rndPower/4));
			}

			unit->addFiringExp();
		}
	}
	else
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

		// raytrace every 3 degrees makes sure we cover all tiles in a circle.
		for (int te = 0; te <= 360; te += 3)
		{
			double cos_te = cos(te * M_PI / 180.0);
			double sin_te = sin(te * M_PI / 180.0);

			Tile *origin = _save->getTile(center);
			double l = 0;
			double vx, vy, vz;
			int tileX, tileY, tileZ;
			power_ = power + 1;

			while (power_ > 0 && l <= maxRadius)
			{
				vx = centerX + l * cos_te;
				vy = centerY + l * sin_te;
				vz = centerZ;

				tileZ = int(floor(vz));
				tileX = int(floor(vx));
				tileY = int(floor(vy));

				Tile *dest = _save->getTile(Position(tileX, tileY, tileZ));
				if (!dest) break; // out of map!

				// horizontal blockage by walls
				power_ -= (horizontalBlockage(origin, dest, type) + verticalBlockage(origin, dest, type));

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
						if (type == DT_HE)
						{
							// power 50 - 150%
							if (dest->getUnit())
								dest->getUnit()->damage(Position(0, 0, 0), (int)(RNG::generate(power_/2.0, power_*1.5)));
							// destroy floors above
							Tile *tileAbove = _save->getTile(Position(tileX, tileY, tileZ+1));
							if ( tileAbove && tileAbove->getMapData(MapData::O_FLOOR) && power_ / 2 >= tileAbove->getMapData(MapData::O_FLOOR)->getArmor())
							{
								tileAbove->destroy(MapData::O_FLOOR);
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
						if (type == DT_IN)
						{
							if (dest->getFire() == 0)
							{
								dest->ignite();
							}
							if (dest->getUnit())
							{
								dest->getUnit()->damage(Position(0, 0, 0), RNG::generate(0, power_/3)); // immediate IN damage
								dest->getUnit()->setFire(RNG::generate(1, 5)); // catch fire and burn for 1-5 rounds
							}
						}
					}
				}
				power_ -= 10; // explosive damage decreases by 10
				origin = dest;
				l++;
			}
		}

		// now detonate the tiles affected with HE
		if (type == DT_HE)
		{
			for (std::set<Tile*>::iterator i = tilesAffected.begin(); i != tilesAffected.end(); ++i)
			{
				(*i)->detonate();
			}
		}
	}

	calculateFOV(center);
	calculateTerrainLighting(); // fires could have been started
}

/**
 * Chained explosions are explosions wich occur after an explosive map object is destroyed.
 * May be due a direct hit, other explosion or fire.
 * @return tile on which a explosion occured
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
 * @param endTile The adjecant tile where the power ends.
 * @param type The type of power/damage.
 * @return Amount of blockage of this power.
 */
int TileEngine::verticalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type)
{
	int block = 0;

	// safety check
	if (startTile == 0 || endTile == 0) return 0;

	int direction = endTile->getPosition().z - startTile->getPosition().z;
	int x = startTile->getPosition().x;
	int y = startTile->getPosition().y;

	if (direction < 0) // down
	{
		if (x != endTile->getPosition().x || y != endTile->getPosition().y)
		{
			x = endTile->getPosition().x;
			y = endTile->getPosition().y;
			for (int z = startTile->getPosition().z; z > endTile->getPosition().z; z--)
			{
				block += blockage(_save->getTile(Position(x, y, z)), MapData::O_FLOOR, type);
			}
		}
	}
	else if (direction > 0) // up
	{
		for (int z = startTile->getPosition().z + 1; z <= endTile->getPosition().z; z++)
		{
			block += blockage(_save->getTile(Position(x, y, z)), MapData::O_FLOOR, type);
		}
		if (x != endTile->getPosition().x || y != endTile->getPosition().y)
		{
			x = endTile->getPosition().x;
			y = endTile->getPosition().y;
			for (int z = startTile->getPosition().z + 1; z <= endTile->getPosition().z; z++)
			{
				block += blockage(_save->getTile(Position(x, y, z)), MapData::O_FLOOR, type);
			}
		}
	}

	return block;
}

/**
 * The amount of power that is blocked going from one tile to another on the same level.
 * @param startTile The tile where the power starts.
 * @param endTile The adjecant tile where the power ends.
 * @param type The type of power/damage.
 * @return amount of blockage
 */
int TileEngine::horizontalBlockage(Tile *startTile, Tile *endTile, ItemDamageType type)
{
	static const Position oneTileNorth = Position(0, 1, 0);
	static const Position oneTileEast = Position(1, 0, 0);
	static const Position oneTileSouth = Position(0, -1, 0);
	static const Position oneTileWest = Position(-1, 0, 0);

	// safety check
	if (startTile == 0 || endTile == 0) return 0;

	int direction = vectorToDirection(endTile->getPosition() - startTile->getPosition());
	if (direction == -1) return 0;

	int block = 0;

	switch(direction)
	{
	case 0:	// north
		block = blockage(startTile, MapData::O_NORTHWALL, type);
		break;
	case 1: // north east
		block = (blockage(startTile,MapData::O_NORTHWALL, type) + blockage(endTile,MapData::O_WESTWALL, type))/2
			+ (blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_WESTWALL, type)
			+ blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_NORTHWALL, type))/2;
		block += (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type) +
			      blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type))/2;
		break;
	case 2: // east
		block = blockage(endTile,MapData::O_WESTWALL, type);
		break;
	case 3: // south east
		block = (blockage(endTile,MapData::O_WESTWALL, type) + blockage(endTile,MapData::O_NORTHWALL, type))/2
			+ (blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_WESTWALL, type)
			+ blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_NORTHWALL, type))/2;
		block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type) +
			      blockage(_save->getTile(startTile->getPosition() + oneTileEast),MapData::O_OBJECT, type))/2;
		break;
	case 4: // south
		block = blockage(endTile,MapData::O_NORTHWALL, type);
		break;
	case 5: // south west
		block = (blockage(endTile,MapData::O_NORTHWALL, type) + blockage(startTile,MapData::O_WESTWALL, type))/2
			+ (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_WESTWALL, type)
			+ blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_NORTHWALL, type))/2;
		block += (blockage(_save->getTile(startTile->getPosition() + oneTileSouth),MapData::O_OBJECT, type) +
			      blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type))/2;
		break;
	case 6: // west
		block = blockage(startTile,MapData::O_WESTWALL, type);
		break;
	case 7: // north west
		block = (blockage(startTile,MapData::O_WESTWALL, type) + blockage(startTile,MapData::O_NORTHWALL, type))/2
			+ (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_WESTWALL, type)
			+ blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_NORTHWALL, type))/2;
		block += (blockage(_save->getTile(startTile->getPosition() + oneTileNorth),MapData::O_OBJECT, type) +
			      blockage(_save->getTile(startTile->getPosition() + oneTileWest),MapData::O_OBJECT, type))/2;
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

	if (part == MapData::O_FLOOR && tile->getMapData(MapData::O_FLOOR))
	{
		// blockage modifiers of floors in ufo only counted for horizontal stuff, so this is kind of an experiment
		if (type == DT_HE)
			blockage += 15;
		else
			blockage += 255;
	}
	else
	{
		if (tile->getMapData(part))
		{
			blockage += tile->getMapData(part)->getBlock(type);
		}

		// open ufo doors are actually still closed behind the scenes
		// so a special trick is needed to see if they are open, if they are, they obviously don't block anything
		if (tile->isUfoDoorOpen(part))
			blockage = 0;
	}

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
	int y[8] = {1, 1, 0, -1, -1, -1, 0, 1};
	for (int i=0;i<9;++i)
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
 *          0 normal door opened, make a squeeky sound and you can walk through.
 *          1 ufo door is starting to open, make a woosh sound, don't walk through.
 *          3 ufo door is still opening, don't walk through it yet. (have patience, futuristic technology...)
 */
int TileEngine::unitOpensDoor(BattleUnit *unit)
{
	int door = -1;
	Tile *tile = unit->getTile();
	if (unit->getDirection() == 0 || unit->getDirection() == 1 || unit->getDirection() == 7) // north, northeast or northwest
	{
		door = tile->openDoor(MapData::O_NORTHWALL);
		if (door == 1)
		{
			// check for adjacent door(s)
			tile = _save->getTile(unit->getPosition() + Position(1, 0, 0));
			if (tile) tile->openDoor(MapData::O_NORTHWALL);
			tile = _save->getTile(unit->getPosition() + Position(-1, 0, 0));
			if (tile) tile->openDoor(MapData::O_NORTHWALL);
		}
	}
	if ((unit->getDirection() == 2 || unit->getDirection() == 1 || unit->getDirection() == 3) && door == -1) // east, northeast or southeast
	{
		tile = _save->getTile(unit->getPosition() + Position(1, 0, 0));
		if (tile) door = tile->openDoor(MapData::O_WESTWALL);
		if (door == 1)
		{
			// check for adjacent door(s)
			tile = _save->getTile(unit->getPosition() + Position(1, -1, 0));
			if (tile) tile->openDoor(MapData::O_WESTWALL);
			tile = _save->getTile(unit->getPosition() + Position(1, 1, 0));
			if (tile) tile->openDoor(MapData::O_WESTWALL);
		}
	}
	if ((unit->getDirection() == 4 || unit->getDirection() == 5 || unit->getDirection() == 3) && door == -1) // south, southwest or southeast
	{
		tile = _save->getTile(unit->getPosition() + Position(0, -1, 0));
		if (tile) door = tile->openDoor(MapData::O_NORTHWALL);
		if (door == 1)
		{
			// check for adjacent door(s)
			tile = _save->getTile(unit->getPosition() + Position(1, -1, 0));
			if (tile) tile->openDoor(MapData::O_NORTHWALL);
			tile = _save->getTile(unit->getPosition() + Position(-1, -1, 0));
			if (tile) tile->openDoor(MapData::O_NORTHWALL);
		}
	}
	if ((unit->getDirection() == 6 || unit->getDirection() == 5 || unit->getDirection() == 7) && door == -1) // west, southwest or northwest
	{
		door = tile->openDoor(MapData::O_WESTWALL);
		if (door == 1)
		{
			// check for adjacent door(s)
			tile = _save->getTile(unit->getPosition() + Position(0, -1, 0));
			if (tile) tile->openDoor(MapData::O_WESTWALL);
			tile = _save->getTile(unit->getPosition() + Position(0, 1, 0));
			if (tile) tile->openDoor(MapData::O_WESTWALL);
		}
	}

	if (door == 0 || door == 1)
	{
		_save->getTileEngine()->calculateFOV(tile->getPosition());
	}

	return door;
}

/**
 * calculateLine. Using bresenham algorithm in 3D.
 * @param origin
 * @param target
 * @param storeTrajectory true will store the whole trajectory - otherwise it just stores the last position.
 * @param trajector A vector of positions in which the trajectory is stored.
 * @param excludeUnit Excludes this unit in the collision detection.
 * @param doVoxelCheck Check against voxel or tile blocking? (first one for units visibility and line of fire, second one for terrain visibility)
 * @return the objectnumber(0-3) or unit(4) or out of map (5) or -1(hit nothing)
 */
int TileEngine::calculateLine(const Position& origin, const Position& target, bool storeTrajectory, std::vector<Position> *trajectory, BattleUnit *excludeUnit, bool doVoxelCheck)
{
	int x, x0, x1, delta_x, step_x;
    int y, y0, y1, delta_y, step_y;
    int z, z0, z1, delta_z, step_z;
    int swap_xy, swap_xz;
    int drift_xy, drift_xz;
    int cx, cy, cz;
	Position lastPoint(origin);

    //start and end points
    x0 = origin.x;     x1 = target.x;
    y0 = origin.y;     y1 = target.y;
    z0 = origin.z;     z1 = target.z;

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
    for (x = x0; x != x1; x += step_x)
	{
        //copy position
        cx = x;    cy = y;    cz = z;

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
			int result = voxelCheck(Position(cx, cy, cz), excludeUnit);
			if (result != -1)
			{
				if (!storeTrajectory && trajectory != 0)
				{ // store the position of impact
					trajectory->push_back(Position(cx, cy, cz));
				}
				return result;
			}
		}
		else
		{
			int result = horizontalBlockage(_save->getTile(lastPoint), _save->getTile(Position(cx, cy, cz)), DT_NONE)
				       + verticalBlockage(_save->getTile(lastPoint), _save->getTile(Position(cx, cy, cz)), DT_NONE);
			if (result != 0)
			{
				return result;
			}
			 _save->getTile(Position(cx, cy, cz))->setDiscovered(true, 2);
			// walls to the east or south of a visible tile, we see that too
			Tile* t = _save->getTile(Position(cx + 1, cy, cz));
			if (t) t->setDiscovered(true, 0);
			t = _save->getTile(Position(cx, cy - 1, cz));
			if (t) t->setDiscovered(true, 1);

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
		}

        //same in z
        if (drift_xz < 0)
		{
            z = z + step_z;
            drift_xz = drift_xz + delta_x;
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
	if (tile == 0)
	{
		return 5;
	}

	if (!excludeAllUnits)
	{
		BattleUnit *unit = tile->getUnit();
		if (unit != 0 && unit != excludeUnit)
		{
			if ((voxel.z%24) < unit->getHeight())
			{
				int x = 15 - voxel.x%16;
				int y = 15 - voxel.y%16;
				int idx = (unit->getUnit()->getLoftemps() * 16) + y;
				if ((_voxelData->at(idx) & (1 << x))==(1 << x))
				{
					return 4;
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
			int y = 15 - voxel.y%16;
			int idx = (mp->getLoftID((voxel.z%24)/2)*16) + y;
			if ((_voxelData->at(idx) & (1 << x))==(1 << x))
			{
				return i;
			}
		}
	}
	return -1;
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
 * New turn preparations. Like fire and smoke spreading.
 */
void TileEngine::prepareNewTurn()
{
	std::vector<Tile*> tilesOnFire;
	std::vector<Tile*> tilesOnSmoke;

	// prepare a list of tiles on fire/smoke
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
	{
		if (_save->getTiles()[i]->getFire() > 0)
		{
			tilesOnFire.push_back(_save->getTiles()[i]);
		}
		if (_save->getTiles()[i]->getSmoke() > 0)
		{
			tilesOnSmoke.push_back(_save->getTiles()[i]);
		}
	}

	// smoke spreads in 1 random direction, but the direction is same for all smoke
	int spreadX = RNG::generate(-1, +1);
	int spreadY = RNG::generate(-1, +1);
	for (std::vector<Tile*>::iterator i = tilesOnSmoke.begin(); i != tilesOnSmoke.end(); ++i)
	{
		int x = (*i)->getPosition().x;
		int y = (*i)->getPosition().y;
		int z = (*i)->getPosition().z;

		if ((*i)->getUnit())
		{
			// units in smoke suffer stun
			(*i)->getUnit()->stun(((*i)->getSmoke()/5)+1);
		}

		Tile *t = _save->getTile(Position(x+spreadX, y+spreadY, z));
		if (t && !t->getSmoke() && horizontalBlockage((*i), t, DT_SMOKE) == 0)
		{
			t->addSmoke((*i)->getSmoke()/2);
		}
		Tile *t2 = _save->getTile(Position(x+spreadX+spreadX, y+spreadY+spreadY, z));
		if (t && t2 && !t2->getSmoke() && horizontalBlockage(t, t2, DT_SMOKE) == 0)
		{
			t2->addSmoke((*i)->getSmoke()/4);
		}

		// smoke also spreads upwards
		t = _save->getTile(Position(x, y, z+1));
		if (t && !t->getSmoke() && verticalBlockage((*i), t, DT_SMOKE) == 0)
		{
			t->addSmoke((*i)->getSmoke()/2);
		}

		(*i)->prepareNewTurn();
	}

	for (std::vector<Tile*>::iterator i = tilesOnFire.begin(); i != tilesOnFire.end(); ++i)
	{
		if ((*i)->getUnit())
		{
			// units on a flaming tile suffer damage
			(*i)->getUnit()->damage(Position(0,0,0), RNG::generate(1,12));
			// units on a flaming tile can catch fire 33% chance
			if (RNG::generate(0,2) == 1)
			{
				(*i)->getUnit()->setFire(RNG::generate(1,5));
			}
		}

		int z = (*i)->getPosition().z;
		for (int x = (*i)->getPosition().x-1; x <= (*i)->getPosition().x+1; ++x)
		{
			for (int y = (*i)->getPosition().y-1; y <= (*i)->getPosition().y+1; ++y)
			{
				Tile *t = _save->getTile(Position(x, y, z));
				if (t && t->getFire() == 0)
				{
					// check adjacent tiles - if they have a flammability of < 255, there is a chance...
					if (horizontalBlockage((*i), t, DT_IN) == 0)
					{
						int flam = t->getFlammability();
						if (flam < 255)
						{
							double base = RNG::boxMuller(0,126);
							if (base < 0) base *= -1;

							if (flam < base)
							{
								if (RNG::generate(0, flam) < 2)
								{
									t->ignite();
								}
							}
						}
					}
				}
			}
		}
		(*i)->prepareNewTurn();
	}

	if (!tilesOnFire.empty())
	{
		calculateTerrainLighting(); // fires could have been stopped
	}

}

}
