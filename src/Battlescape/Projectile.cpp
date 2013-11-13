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
#include <cmath>
#include "Projectile.h"
#include "TileEngine.h"
#include "../aresame.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Surface.h"
#include "../Battlescape/Position.h"
#include "../Battlescape/TileEngine.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Unit.h"
#include "../Ruleset/RuleSoldier.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/MapData.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Game.h"

namespace OpenXcom
{

/**
 * Sets up a UnitSprite with the specified size and position.
 * @param res Pointer to resourcepack.
 * @param save Pointer to battlesavegame.
 * @param action An action.
 * @param origin Position the projectile originates from.
 */
Projectile::Projectile(ResourcePack *res, SavedBattleGame *save, BattleAction action, Position origin) : _res(res), _save(save), _action(action), _origin(origin), _position(0)
{
	// this is the number of pixels the sprite will move between frames
	_speed = Options::getInt("battleFireSpeed");

	if (_action.weapon)
	{
		if (_action.type == BA_THROW)
		{
			_sprite = _res->getSurfaceSet("FLOOROB.PCK")->getFrame(getItem()->getRules()->getFloorSprite());
		}
		else
		{
			_speed = std::max(1, _speed + _action.weapon->getRules()->getBulletSpeed());
		}
	}
}

/**
 * Deletes the Projectile.
 */
Projectile::~Projectile()
{

}

/**
 * Calculates the trajectory for a straight path.
 * @param accuracy The unit's accuracy.
 * @param doCalcChance Do only clculating probability of hitting to unit. Default = false.
 * @return The objectnumber(0-3) or unit(4) or out of map (5) or -1 (no line of fire)
 *			or chance to hit (-1 none; 0-100 percent)
 */
int Projectile::calculateTrajectory(double accuracy, bool doCalcChance)
{
	Position originVoxel, targetVoxel;
	Tile *targetTile = 0;
	int smokeDensity;
	//int dirYshift[24] = {1, 3, 9, 15, 15, 13, 7, 1,  1, 1, 7, 13, 15, 15, 9, 3,  1, 2, 8, 14, 15, 14, 8, 2};
	//int dirXshift[24] = {9, 15, 15, 13, 8, 1, 1, 3,  7, 13, 15, 15, 9, 3, 1, 1,  8, 14, 15, 14, 8, 2, 1, 2};
	// maybe if i get around to making that function to calculate a firepoint origin for fire point estimations i'll use the array above
	// so i'll leave it commented for the time being.

	originVoxel = Position(_origin.x*16, _origin.y*16, _origin.z*24);
	BattleUnit *bu = _action.actor;
	int offset = 8 * bu->getArmor()->getSize();

	// take into account soldier height and terrain level if the projectile is launched from a soldier
	if (_action.actor->getPosition() == _origin)
	{
		// calculate offset of the starting point of the projectile
		originVoxel.z += -_save->getTile(_origin)->getTerrainLevel();

		originVoxel.z += bu->getHeight() + bu->getFloatHeight();
		originVoxel.z -= 4;
		Tile *tileAbove = _save->getTile(_origin + Position(0,0,1));
		if (originVoxel.z >= (_origin.z + 1)*24)
		{
			if (tileAbove && tileAbove->hasNoFloor(0))
			{
				_origin.z++;
			}
			else
			{
				while (originVoxel.z >= (_origin.z + 1)*24)
				{
					originVoxel.z--;
				}
				originVoxel.z -= 4;
			}
		}
		// originally used the dirXShift and dirYShift as detailed above, this however results in MUCH more predictable results.
		originVoxel.x += offset;
		originVoxel.y += offset;
	}
	else
	{
		// don't take into account soldier height and terrain level if the projectile is not launched from a soldier(from a waypoint)
		originVoxel.x += 8;
		originVoxel.y += 8;
		originVoxel.z += 12;
	}

	if (_action.type == BA_LAUNCH || (SDL_GetModState() & KMOD_CTRL) != 0)
	{
		if (doCalcChance) return -1;
		// target nothing, targets the middle of the tile
		targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24 + 12);
	}
	else
	{
		// determine the target voxel.
		// aim at the center of the unit, the object, the walls or the floor (in that priority)
		// if there is no LOF to the center, try elsewhere (more outward).
		// Store this target voxel.
		targetTile = _save->getTile(_action.target);
		Position hitPos;
		int test = -1;
		if (targetTile->getUnit() != 0)
		{
			if (_origin == _action.target || targetTile->getUnit() == _action.actor)
			{
				// don't shoot at yourself but shoot at the floor
				targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24);
			}
			else
			{
				_save->getTileEngine()->canTargetUnit(&originVoxel, targetTile, &targetVoxel, bu);
			}
		}
		else if (targetTile->getMapData(MapData::O_OBJECT) != 0)
		{
			if (!_save->getTileEngine()->canTargetTile(&originVoxel, targetTile, MapData::O_OBJECT, &targetVoxel, bu))
			{
				targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24 + 10);
			}
		}
		else if (targetTile->getMapData(MapData::O_NORTHWALL) != 0)
		{
			if (!_save->getTileEngine()->canTargetTile(&originVoxel, targetTile, MapData::O_NORTHWALL, &targetVoxel, bu))
			{
				targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16, _action.target.z*24 + 9);
			}
		}
		else if (targetTile->getMapData(MapData::O_WESTWALL) != 0)
		{
			if (!_save->getTileEngine()->canTargetTile(&originVoxel, targetTile, MapData::O_WESTWALL, &targetVoxel, bu))
			{
				targetVoxel = Position(_action.target.x*16, _action.target.y*16 + 8, _action.target.z*24 + 9);
			}
		}
		else if (targetTile->getMapData(MapData::O_FLOOR) != 0)
		{
			if (!_save->getTileEngine()->canTargetTile(&originVoxel, targetTile, MapData::O_FLOOR, &targetVoxel, bu))
			{
				targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24 + 2);
			}
		}
		else
		{
			// target nothing, targets the middle of the tile
			targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24 + 12);
		}
		test = _save->getTileEngine()->calculateLine(originVoxel, targetVoxel, false, &_trajectory, bu, true, false, 0, &smokeDensity);
		smokeDensity /= 16;	// normalization (16 voxels per tile)
		if (test != -1 && !_trajectory.empty() && _action.actor->getFaction() == FACTION_PLAYER && _action.autoShotCounter == 1)
		{
			hitPos = Position(_trajectory.at(0).x/16, _trajectory.at(0).y/16, _trajectory.at(0).z/24);
			if (test == 4 && _save->getTile(hitPos) && _save->getTile(hitPos)->getUnit() == 0) //no unit? must be lower
			{
				hitPos = Position(hitPos.x, hitPos.y, hitPos.z-1);
			}

			if (hitPos != _action.target && _action.result == "")
			{
				if (test == 2)
				{
					if (hitPos.y - 1 != _action.target.y)
					{
						_trajectory.clear();
						return -1;
					}
				}
				else if (test == 1)
				{
					if (hitPos.x - 1 != _action.target.x)
					{
						_trajectory.clear();
						return -1;
					}
				}
				else if (test == 4)
				{
					BattleUnit *hitUnit = _save->getTile(hitPos)->getUnit();
					BattleUnit *targetUnit = targetTile->getUnit();
					if (hitUnit != targetUnit)
					{
						_trajectory.clear();
						return -1;
					}
				}
				else
				{
					_trajectory.clear();
					return -1;
				}
			}
		}

		if (doCalcChance)
		{
			if (test != 4 || _trajectory.empty() ||
				_trajectory.at(0).x/16 != _action.target.x || _trajectory.at(0).y/16 != _action.target.y) // Don't need check z axis.
				return -1;
			return applyAccuracy(originVoxel, &targetVoxel, accuracy, false, targetTile, smokeDensity, true);
		}
	}
	_trajectory.clear();

	// This will results in a new target voxel
	if (_action.type != BA_LAUNCH)
		applyAccuracy(originVoxel, &targetVoxel, accuracy, false, targetTile, smokeDensity);

	// finally do a line calculation and store this trajectory.
	return _save->getTileEngine()->calculateLine(originVoxel, targetVoxel, true, &_trajectory, bu);
}

/**
 * Calculates the trajectory for a curved path.
 * @param accuracy The unit's accuracy.
 * @return True when a trajectory is possible.
 */
int Projectile::calculateThrow(double accuracy)
{
	Position originVoxel, targetVoxel;
	bool foundCurve = false;

	// object blocking - can't throw here
	if (_action.type == BA_THROW &&_save->getTile(_action.target) && _save->getTile(_action.target)->getMapData(MapData::O_OBJECT) && _save->getTile(_action.target)->getMapData(MapData::O_OBJECT)->getTUCost(MT_WALK) == 255)
	{
		return -1;
	}

	originVoxel = Position(_origin.x*16 + 8, _origin.y*16 + 8, _origin.z*24);
	originVoxel.z += -_save->getTile(_origin)->getTerrainLevel();
	BattleUnit *bu = _save->getTile(_origin)->getUnit();
	Tile *tileAbove = _save->getTile(_origin + Position(0,0,1));
	if(!bu)
		bu = _save->getTile(Position(_origin.x, _origin.y, _origin.z-1))->getUnit();
	originVoxel.z += bu->getHeight() + bu->getFloatHeight();
	originVoxel.z -= 3;
	if (originVoxel.z >= (_origin.z + 1)*24)
	{
		if (!tileAbove || !tileAbove->hasNoFloor(0))
		{
			while (originVoxel.z > (_origin.z + 1)*24)
			{
				originVoxel.z--;
			}
			originVoxel.z -=4;
		}
		else
		{
			_origin.z++;
		}
	}

	// determine the target voxel.
	// aim at the center of the floor
	targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24 + 2);
	targetVoxel.z -= _save->getTile(_action.target)->getTerrainLevel();
	if (_action.type != BA_THROW)
	{
		BattleUnit *tu = _save->getTile(_action.target)->getUnit();
		if(!tu && _action.target.z > 0 && _save->getTile(_action.target)->hasNoFloor(0))
			tu = _save->getTile(Position(_action.target.x, _action.target.y, _action.target.z-1))->getUnit();
		if (tu)
		{
			targetVoxel.z += (tu->getHeight()/2) + tu->getFloatHeight();
		}
	}

	// we try 4 different curvatures to try and reach our goal.
	double curvature = 1.0;
	while (!foundCurve && curvature < 5.0)
	{
		int check = _save->getTileEngine()->calculateParabola(originVoxel, targetVoxel, false, &_trajectory, bu, curvature, 1.0);
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
	if ( AreSame(curvature, 5.0) )
	{
		return -1;
	}

	// apply some accuracy modifiers
	if (accuracy > 1)
		accuracy = 1;
	static const double maxDeviation = 0.08;
	static const double minDeviation = 0;
	double baseDeviation = (maxDeviation - (maxDeviation * accuracy)) + minDeviation;
	double deviation = RNG::boxMuller(0, baseDeviation);

	int retValue = -1;

	_trajectory.clear();
	// finally do a line calculation and store this trajectory.
	retValue = _save->getTileEngine()->calculateParabola(originVoxel, targetVoxel, true, &_trajectory, bu, curvature, 1.0 + deviation);

	Position endPoint = _trajectory.at(_trajectory.size() - 1);
	endPoint.x /= 16;
	endPoint.y /= 16;
	endPoint.z /= 24;
	// check if the item would land on a tile with a blocking object, if so then we let it fly without deviation, it must land on a valid tile in that case
	if (_save->getTile(endPoint) && _save->getTile(endPoint)->getMapData(MapData::O_OBJECT) && _save->getTile(endPoint)->getMapData(MapData::O_OBJECT)->getTUCost(MT_WALK) == 255)
	{
		_trajectory.clear();
		// finally do a line calculation and store this trajectory.
		retValue = _save->getTileEngine()->calculateParabola(originVoxel, targetVoxel, true, &_trajectory, bu, curvature, 1.0);
	}


	return retValue;
}

/**
 * Calculates the new target in voxel space, based on the given accuracy modifier.
 * @param origin Start position of the trajectory.
 * @param target Endpoint of the trajectory.
 * @param accuracy Accuracy modifier.
 * @param keepRange Whether range affects accuracy.
 * @param targetTile Tile of target. Default = 0.
 * @param densitySmoke Density of smoke between positions. Default = 0.
 * @param doCalcChance Do only calculation the probability of hitting. Default = false.
 * @return Chance to hit 0-100% (-1 if cannot to calculate).
 */
int Projectile::applyAccuracy(const Position& origin, Position *target, double accuracy, bool keepRange, Tile *targetTile, int smokeDensity, bool doCalcChance)
{
	int xdiff = origin.x - target->x;
	int ydiff = origin.y - target->y;
	double realDistance = sqrt((double)(xdiff*xdiff)+(double)(ydiff*ydiff));
	// maxRange is the maximum range a projectile shall ever travel in voxel space
	double maxRange = keepRange?realDistance:16*1000; // 1000 tiles
	maxRange = _action.type == BA_HIT?46:maxRange; // up to 2 tiles diagonally (as in the case of reaper v reaper)

	if (Options::getBool("battleRangeBasedAccuracy"))
	{
		double baseDeviation, effectiveAccuracy = accuracy;

		BattleUnit* shooterUnit = _save->getTile(Position(origin.x/16, origin.y/16, origin.z/24))->getUnit();
		BattleUnit* targetUnit = targetTile? targetTile->getUnit() : 0;
		// Taken into account shades on the target. Enemy units can see in the dark.
		if (shooterUnit && shooterUnit->getFaction() == FACTION_PLAYER)
		{
			if (targetTile)
				effectiveAccuracy -= 0.01 * targetTile->getShade();		// Shade can be from 0 to 15
			else
				effectiveAccuracy -= 0.01 * _save->getGlobalShade();	// Shade can be from 0 (day) to 15 (night).
		}
		// If targetUnit not visible by shooter, then chance to hit them reduced on 5%.
		if (shooterUnit && targetUnit)
		{
			std::vector<BattleUnit*>::const_iterator noUnit = shooterUnit->getVisibleUnits()->end();
			std::vector<BattleUnit*>::const_iterator u = shooterUnit->getVisibleUnits()->begin();
			for ( ; u != noUnit && *u != targetUnit; ++u);	// by design

			if (u == noUnit)
				effectiveAccuracy -= 0.05;
		}
		// If unit is kneeled, then chance to hit them reduced on 5%. This is a compromise, because vertical deviation in 2 times less.
		if (targetUnit && targetUnit->isKneeled())
			effectiveAccuracy -= 0.05;
		// Taken into account smoke between shooter and target. 5% per tile with max density of smoke 15.
		// Anyway, we cannot to see more 4 tiles in dense smoke. Therefore maximum decrease is 20%.
		if (smokeDensity >= 60)
			effectiveAccuracy -= 0.20;
		else if (smokeDensity > 0)
			effectiveAccuracy -= 0.05 * smokeDensity / 15.0;
		// Each next shot of autoshot reduces accuracy.
		if (_action.type == BA_AUTOSHOT && _action.autoShotCounter > 1)
			effectiveAccuracy -= 0.05;

		if (effectiveAccuracy > -0.15)
			baseDeviation = -0.15 + 0.26 / (effectiveAccuracy + 0.25);
		else
			baseDeviation = 2.45;	// 2.45 radian - max deviation for worst case.

		// 0.02 is the min angle deviation for best accuracy (+-3s = 0.02 radian).
		if (baseDeviation < 0.02)
			baseDeviation = 0.02;

		if (!doCalcChance)
		{
			// the angle deviations are spread using a normal distribution for baseDeviation (+-3s with precision 99,7%)
			double dH = RNG::boxMuller(0.0, baseDeviation / 6.0);  // horizontal miss in radian
			double dV = RNG::boxMuller(0.0, baseDeviation /(6.0 * 2));
			double te = atan2(double(target->y - origin.y), double(target->x - origin.x)) + dH;
			double fi = atan2(double(target->z - origin.z), realDistance) + dV;
			double cos_fi = cos(fi);

			// It is a simple task - to hit in target width of 5-7 voxels. Good luck!
			target->x = (int)(origin.x + maxRange * cos(te) * cos_fi);
			target->y = (int)(origin.y + maxRange * sin(te) * cos_fi);
			target->z = (int)(origin.z + maxRange * sin(fi));
			return -1;
		}
		else
		{
			if (targetUnit)
			{
				int loftemps = targetUnit->getLoftemps();
				if (loftemps > 5) loftemps = 8;
				return (int) (0.5 + 100.0 * approxF(baseDeviation / 6.0, (0.5 + loftemps) / realDistance)
					* approxF(baseDeviation / 12.0, 0.5 * targetUnit->getHeight() / realDistance));
			}
			else
				return -1;
		}
	}

	if (doCalcChance)
	{
		BattleUnit* targetUnit = targetTile? targetTile->getUnit() : 0;
		if (targetUnit)
		{
			int loftemps = targetUnit->getLoftemps();
			if (loftemps > 5) loftemps = 8;
			return vanillaHit(origin, target, accuracy, 1 + 2 * loftemps, targetUnit->getHeight());
		}
		else
			return -1;
	}

	int xDist = abs(origin.x - target->x);
	int yDist = abs(origin.y - target->y);
	int zDist = abs(origin.z - target->z);
	int xyShift, zShift;

	if (xDist / 2 <= yDist)				//yes, we need to add some x/y non-uniformity
		xyShift = xDist / 4 + yDist;	//and don't ask why, please. it's The Commandment
	else
		xyShift = (xDist + yDist) / 2;	//that's uniform part of spreading

	if (xyShift <= zDist)				//slight z deviation
		zShift = xyShift / 2 + zDist;
	else
		zShift = xyShift + zDist / 2;

	int deviation = RNG::generate(0, 100) - (accuracy * 100);

	if (deviation >= 0)
		deviation += 50;				// add extra spread to "miss" cloud
	else
		deviation += 10;				//accuracy of 109 or greater will become 1 (tightest spread)
	
	deviation = std::max(1, zShift * deviation / 200);	//range ratio
		
	target->x += RNG::generate(0, deviation) - deviation / 2;
	target->y += RNG::generate(0, deviation) - deviation / 2;
	target->z += RNG::generate(0, deviation / 2) / 2 - deviation / 8;
	
	double rotation, tilt;
	rotation = atan2(double(target->y - origin.y), double(target->x - origin.x)) * 180 / M_PI;
	tilt = atan2(double(target->z - origin.z),
		sqrt(double(target->x - origin.x)*double(target->x - origin.x)+double(target->y - origin.y)*double(target->y - origin.y))) * 180 / M_PI;
	// calculate new target
	// this new target can be very far out of the map, but we don't care about that right now
	double cos_fi = cos(tilt * M_PI / 180.0);
	double sin_fi = sin(tilt * M_PI / 180.0);
	double cos_te = cos(rotation * M_PI / 180.0);
	double sin_te = sin(rotation * M_PI / 180.0);
	target->x = (int)(origin.x + maxRange * cos_te * cos_fi);
	target->y = (int)(origin.y + maxRange * sin_te * cos_fi);
	target->z = (int)(origin.z + maxRange * sin_fi);

	return -1;
}

/**
 * Approximation of the F-function (cumulative distribution function).
 * @param sigm Standart deviation (in radians).
 * @param delta Half of the angle of view of the profile of the enemy (in radians).
 * @return Chance to hit (from 0 to 1).
 */
double Projectile::approxF(double sigm, double delta)
{
	double a = 0.7071067812 * delta / sigm;	// 1/sqrt(2) =  0.7071067812
	// max error calculation is 1-2%
	double r = 0.2820947918 * a * (exp(-a * a) + 2*exp(-0.25 * a * a) + 1.0);	// 1/(2*sqrt(pi)) = 0.2820947918

	return (r > 1.0)? 1.0 : r;
}

/**
 * Calculation of the probability of hitting (vanilla version).
 * @param origin Start position of the trajectory.
 * @param target Endpoint of the trajectory.
 * @param accuracy Accuracy modifier.
 * @param d Diameter of target.
 * @param h Height of target.
 * @return Chance to hit (from 0 to 100).
 */
int Projectile::vanillaHit(const Position& origin, Position *target, double accuracy, int d, int h)
{
	if (accuracy > 1.09) return 100;	// We have hit. I guarantee it!

	double mayMiss, mayHit, guarantHit;
	if (accuracy > 1.01)
	{
		mayMiss = 0.00;
		mayHit = 1.09 - accuracy;
		guarantHit = 1.01 - mayMiss - mayHit;
	}
	else if (accuracy > 0.09)
	{
		mayMiss = 1.01 - accuracy;
		mayHit = 0.09;
		guarantHit = 1.01 - mayMiss - mayHit;
	}
	else
	{
		mayMiss = 1.01 - accuracy;
		mayHit = accuracy;
		guarantHit = 0.00;
	}

	double dx = origin.x - target->x;
	double dy = origin.y - target->y;
	double dz = origin.z - target->z;
	double range = sqrt(dx*dx + dy*dy + dz*dz);

	return (int) (0.5 + 100.0 * (mayMiss * approxHit(range * (mayMiss + 0.5) / 2.0, d, h)
		+ mayHit * approxHit(range * 0.09 / 2.0, d, h) + guarantHit) / 1.01);
}

/**
 * Approximation of vanillas chance to hit.
 * @param dev Deviation (vanilla meaning).
 * @param d Diameter of target.
 * @param h Height of target.
 * @return Chance to hit (from 0 to 1).
 */
double Projectile::approxHit(double dev, int d, int h)
{
	// x-y square
	if (dev <= d) return 1.0;
	double r = d/dev + (1 - d/dev) * (2 * d)/(dev + d);

	// z axis
	dev /= 4.0;
	if (dev <= h) return r;
	return r * (h/dev + (1 - h/dev) * (2 * h)/(dev + h));
}

/**
 * Moves further in the trajectory.
 * @return false if the trajectory is finished - no new position exists in the trajectory.
 */
bool Projectile::move()
{
	for (int i = 0; i < _speed; ++i)
	{
		_position++;
		if (_position == _trajectory.size())
		{
			_position--;
			return false;
		}
	}
	return true;
}

/**
 * Gets the current position in voxel space.
 * @param offset Offset.
 * @return Position in voxel space.
 */
Position Projectile::getPosition(int offset) const
{
	int posOffset = (int)_position + offset;
	if (posOffset >= 0 && posOffset < (int)_trajectory.size())
		return _trajectory.at(posOffset);
	else
		return _trajectory.at(_position);
}

/**
 * Gets a particle reference from the projectile surfaces.
 * @param i Index.
 * @return Particle id.
 */
int Projectile::getParticle(int i) const
{
	if (_action.weapon->getRules()->getBulletSprite() == -1)
		return -1;
	else
		return _action.weapon->getRules()->getBulletSprite() + i;
}

/**
 * Gets the project tile item.
 * Returns 0 when there is no item thrown.
 * @return Pointer to BattleItem.
 */
BattleItem *Projectile::getItem() const
{
	if (_action.type == BA_THROW)
		return _action.weapon;
	else
		return 0;
}

/**
 * Gets the bullet sprite.
 * @return Pointer to Surface.
 */
Surface *Projectile::getSprite() const
{
	return _sprite;
}

/**
 * Skips to the end of the trajectory.
 */
void Projectile::skipTrajectory()
{
	_position = _trajectory.size() - 2;
}

}
