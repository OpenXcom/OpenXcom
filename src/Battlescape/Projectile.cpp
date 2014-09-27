/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "../fmath.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Surface.h"
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
 * @param targetVoxel Position the projectile is targeting.
 */
Projectile::Projectile(ResourcePack *res, SavedBattleGame *save, BattleAction action, Position origin, Position targetVoxel, int bulletSprite) : _res(res), _save(save), _action(action), _origin(origin), _targetVoxel(targetVoxel), _position(0), _bulletSprite(bulletSprite), _reversed(false)
{
	// this is the number of pixels the sprite will move between frames
	_speed = Options::battleFireSpeed;

	if (_action.weapon)
	{
		if (_action.type == BA_THROW)
		{
			_sprite = _res->getSurfaceSet("FLOOROB.PCK")->getFrame(getItem()->getRules()->getFloorSprite());
		}
		else
		{
			if (_action.weapon->getRules()->getBulletSpeed() != 0)
			{
				_speed = std::max(1, _speed + _action.weapon->getRules()->getBulletSpeed());
			}
			else if (_action.weapon->getAmmoItem() && _action.weapon->getAmmoItem()->getRules()->getBulletSpeed() != 0)
			{
				_speed = std::max(1, _speed + _action.weapon->getAmmoItem()->getRules()->getBulletSpeed());
			}
		}
	}
	if ((targetVoxel.x - origin.x) + (targetVoxel.y - origin.y) >= 0)
	{
		_reversed = true;
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
 * @return The objectnumber(0-3) or unit(4) or out of map (5) or -1 (no line of fire).
 */

int Projectile::calculateTrajectory(double accuracy)
{
	Position originVoxel = _save->getTileEngine()->getOriginVoxel(_action, _save->getTile(_origin));
	return calculateTrajectory(accuracy, originVoxel);
}

int Projectile::calculateTrajectory(double accuracy, Position originVoxel)
{
	Tile *targetTile = _save->getTile(_action.target);
	BattleUnit *bu = _action.actor;
	
	int test = _save->getTileEngine()->calculateLine(originVoxel, _targetVoxel, false, &_trajectory, bu);
	if (test != V_EMPTY &&
		!_trajectory.empty() &&
		_action.actor->getFaction() == FACTION_PLAYER &&
		_action.autoShotCounter == 1 &&
		((SDL_GetModState() & KMOD_CTRL) == 0 || !Options::forceFire) &&
		_save->getBattleGame()->getPanicHandled() &&
		_action.type != BA_LAUNCH)
	{
		Position hitPos = Position(_trajectory.at(0).x/16, _trajectory.at(0).y/16, _trajectory.at(0).z/24);
		if (test == V_UNIT && _save->getTile(hitPos) && _save->getTile(hitPos)->getUnit() == 0) //no unit? must be lower
		{
			hitPos = Position(hitPos.x, hitPos.y, hitPos.z-1);
		}

		if (hitPos != _action.target && _action.result.empty())
		{
			if (test == V_NORTHWALL)
			{
				if (hitPos.y - 1 != _action.target.y)
				{
					_trajectory.clear();
					return V_EMPTY;
				}
			}
			else if (test == V_WESTWALL)
			{
				if (hitPos.x - 1 != _action.target.x)
				{
					_trajectory.clear();
					return V_EMPTY;
				}
			}
			else if (test == V_UNIT)
			{
				BattleUnit *hitUnit = _save->getTile(hitPos)->getUnit();
				BattleUnit *targetUnit = targetTile->getUnit();
				if (hitUnit != targetUnit)
				{
					_trajectory.clear();
					return V_EMPTY;
				}
			}
			else
			{
				_trajectory.clear();
				return V_EMPTY;
			}
		}
	}

	_trajectory.clear();

	bool extendLine = true;
	// even guided missiles drift, but how much is based on
	// the shooter's faction, rather than accuracy.
	if (_action.type == BA_LAUNCH)
	{
		if (_action.actor->getFaction() == FACTION_PLAYER)
		{
			accuracy = 0.60;
		}
		else
		{
			accuracy = 0.55;
		}
		extendLine = _action.waypoints.size() <= 1;
	}

	// apply some accuracy modifiers.
	// This will results in a new target voxel
	applyAccuracy(originVoxel, &_targetVoxel, accuracy, false, targetTile, extendLine);

	// finally do a line calculation and store this trajectory.
	return _save->getTileEngine()->calculateLine(originVoxel, _targetVoxel, true, &_trajectory, bu);
}

/**
 * Calculates the trajectory for a curved path.
 * @param accuracy The unit's accuracy.
 * @return True when a trajectory is possible.
 */
int Projectile::calculateThrow(double accuracy)
{
	Tile *targetTile = _save->getTile(_action.target);
		
	Position originVoxel = _save->getTileEngine()->getOriginVoxel(_action, 0);
	Position targetVoxel = _action.target * Position(16,16,24) + Position(8,8, (2 + -targetTile->getTerrainLevel()));

	if (_action.type != BA_THROW)
	{
		BattleUnit *tu = targetTile->getUnit();
		if (!tu && _action.target.z > 0 && targetTile->hasNoFloor(0))
			tu = _save->getTile(_action.target - Position(0, 0, 1))->getUnit();
		if (tu)
		{
			targetVoxel.z += ((tu->getHeight()/2) + tu->getFloatHeight()) - 2;
		}
	}

	double curvature = 0.0;
	int retVal = V_OUTOFBOUNDS;
	if (_save->getTileEngine()->validateThrow(_action, originVoxel, targetVoxel, &curvature, &retVal))
	{
		int test = V_OUTOFBOUNDS;
		// finally do a line calculation and store this trajectory, make sure it's valid.
		while (test == V_OUTOFBOUNDS)
		{
			Position deltas = targetVoxel;
			// apply some accuracy modifiers
			applyAccuracy(originVoxel, &deltas, accuracy, true, _save->getTile(_action.target), false); //calling for best flavor
			deltas -= targetVoxel;
			_trajectory.clear();
			test = _save->getTileEngine()->calculateParabola(originVoxel, targetVoxel, true, &_trajectory, _action.actor, curvature, deltas);

			Position endPoint = _trajectory.back();
			endPoint.x /= 16;
			endPoint.y /= 16;
			endPoint.z /= 24;
			Tile *endTile = _save->getTile(endPoint);
			// check if the item would land on a tile with a blocking object
			if (_action.type == BA_THROW
				&& endTile
				&& endTile->getMapData(MapData::O_OBJECT)
				&& endTile->getMapData(MapData::O_OBJECT)->getTUCost(MT_WALK) == 255)
			{
				test = V_OUTOFBOUNDS;
			}
		}
		return retVal;
	}
	return V_OUTOFBOUNDS;
}

/**
 * Calculates the new target in voxel space, based on the given accuracy modifier.
 * @param origin Startposition of the trajectory in voxels.
 * @param target Endpoint of the trajectory in voxels.
 * @param accuracy Accuracy modifier.
 * @param keepRange Whether range affects accuracy.
 * @param targetTile Tile of target. Default = 0.
 * @param extendLine should this line get extended to maximum distance?
 */
void Projectile::applyAccuracy(const Position& origin, Position *target, double accuracy, bool keepRange, Tile *targetTile, bool extendLine)
{
	int xdiff = origin.x - target->x;
	int ydiff = origin.y - target->y;
	double realDistance = sqrt((double)(xdiff*xdiff)+(double)(ydiff*ydiff));
	// maxRange is the maximum range a projectile shall ever travel in voxel space
	double maxRange = keepRange?realDistance:16*1000; // 1000 tiles
	maxRange = _action.type == BA_HIT?46:maxRange; // up to 2 tiles diagonally (as in the case of reaper v reaper)
	RuleItem *weapon = _action.weapon->getRules();

	if (_action.type != BA_THROW && _action.type != BA_HIT)
	{
		double modifier = 0.0;
		int upperLimit = weapon->getAimRange();
		int lowerLimit = weapon->getMinRange();
		if (Options::battleUFOExtenderAccuracy)
		{
			if (_action.type == BA_AUTOSHOT)
			{
				upperLimit = weapon->getAutoRange();
			}
			else if (_action.type == BA_SNAPSHOT)
			{
				upperLimit = weapon->getSnapRange();
			}
		}
		if (realDistance / 16 < lowerLimit)
		{
			modifier = (weapon->getDropoff() * (lowerLimit - realDistance / 16)) / 100;
		}
		else if (upperLimit < realDistance / 16)
		{
			modifier = (weapon->getDropoff() * (realDistance / 16 - upperLimit)) / 100;
		}
		accuracy = std::max(0.0, accuracy - modifier);
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
	
	if (extendLine)
	{
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
	}
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
	if (_bulletSprite != -1)
		return _bulletSprite + i;
	else
		return -1;
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
	_position = _trajectory.size() - 1;
}

/**
 * Gets the Position of origin for the projectile
 * @return origin as a tile position.
 */
Position Projectile::getOrigin()
{
	// instead of using the actor's position, we'll use the voxel origin translated to a tile position
	// this is a workaround for large units.
	return _trajectory.front() / Position(16,16,24);
}

/**
 * Gets the INTENDED target for this projectile
 * it is important to note that we do not use the final position of the projectile here,
 * but rather the targetted tile
 * @return target as a tile position.
 */
Position Projectile::getTarget()
{
	return _action.target;
}

bool Projectile::isReversed() const
{
	return _reversed;
}
}
