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
#include "Projectile.h"
#include "TileEngine.h"
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

namespace OpenXcom
{

// the wonderful bullet trail art
const int Projectile::_trail[11][36] = {
	{0x23, 0x0, 0x0, 0x0, 0x1, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x2, 0x4, 0x4, 0x4, 0x4, 0x5, 0x5, 0xFF, 0x6, 0xFF, 0x6, 0xFF, 0x6, 0xFF, 0x7, 0xFF, 0x7, 0xFF, 0x7, 0xFF, 0xFF, 0x7, 0xFF, 0xFF, 0xFF},
	{0x7, 0x0D, 0x0A, 0x0A, 0x0B, 0x0C, 0xFF, 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x11, 0x8, 0x9, 0x9, 0x0A, 0x0A, 0x0B, 0x0B, 0x0C, 0x0C, 0xFF, 0x0C, 0xFF, 0xFF, 0x0C, 0xFF, 0xFF, 0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x16, 0x0E, 0x0E, 0x0F, 0xFF, 0x8, 0x10, 0x11, 0x12, 0xFF, 0x12, 0xFF, 0xFF, 0x6, 0xFF, 0xFF, 0x6, 0xFF, 0xFF, 0x7, 0xFF, 0xFF, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x20, 0x0E, 0x0E, 0x0F, 0xFF, 0x8, 0x10, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0xFF, 0x6, 0xFF, 0x6, 0xFF, 0xFF, 0x6, 0xFF, 0xFF, 0x6, 0xFF, 0xFF, 0x7, 0xFF, 0xFF, 0x7, 0xFF, 0xFF, 0x7, 0xFF, 0xFF, 0xFF},
	{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x1A, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x23, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x13, 0x14, 0x15, 0x16, 0x17},
	{0x1D, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B, 0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0xFF, 0x1E, 0xFF, 0x1E, 0xFF, 0xFF, 0x1E, 0xFF, 0xFF, 0xFF, 0x1E, 0xFF, 0xFF, 0xFF, 0xFF, 0x1E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x0F, 0x1F, 0x21, 0x22, 0x23, 0x24, 0x24, 0xFF, 0x24, 0xFF, 0xFF, 0x24, 0xFF, 0xFF, 0xFF, 0x24, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x1E, 0x20, 0x20, 0x1F, 0x1F, 0x1F, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23, 0x23, 0x24, 0x24, 0x24, 0x24, 0xFF, 0x24, 0xFF, 0xFF, 0x24, 0xFF, 0xFF, 0xFF, 0x24, 0xFF, 0xFF, 0xFF, 0xFF, 0x24, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

/**
 * Sets up a UnitSprite with the specified size and position.
 * @param res Pointer to resourcepack.
 * @param save Pointer to battlesavegame.
 */
Projectile::Projectile(ResourcePack *res, SavedBattleGame *save, BattleAction action, Position origin) : _res(res), _save(save), _action(action), _origin(origin), _position(0)
{
	if (_action.weapon && _action.type == BA_THROW)
	{
		_sprite = _res->getSurfaceSet("FLOOROB.PCK")->getFrame(getItem()->getRules()->getFloorSprite());
	}
}

/**
 * Deletes the Projectile.
 */
Projectile::~Projectile()
{

}

/**
 * calculateTrajectory.
 * @return the objectnumber(0-3) or unit(4) or out of map (5) or -1(no line of fire)
 */
int Projectile::calculateTrajectory(double accuracy)
{
	Position originVoxel, targetVoxel;
	int direction;		
	int dirYshift[24] = {1, 3, 9, 15, 15, 13, 7, 1,  1, 1, 7, 13, 15, 15, 9, 3,  1, 2, 8, 14, 15, 14, 8, 2};
	int dirXshift[24] = {9, 15, 15, 13, 7, 1, 1, 3,  7, 13, 15, 15, 9, 3, 1, 1,  8, 14, 15, 14, 8, 2, 1, 2};
	int offset = 0;

	originVoxel = Position(_origin.x*16, _origin.y*16, _origin.z*24);
	BattleUnit *bu = _action.actor;
	
	if (bu->getArmor()->getSize() > 1)
	{
		offset = 16;
	}
	else if(_action.weapon == _action.weapon->getOwner()->getItem("STR_LEFT_HAND") && !_action.weapon->getRules()->isTwoHanded())
	{
		offset = 8;
	}

	// take into account soldier height and terrain level if the projectile is launched from a soldier
	if (_action.actor->getPosition() == _origin)
	{
		// calculate offset of the starting point of the projectile
		originVoxel.z += -_save->getTile(_origin)->getTerrainLevel();

		originVoxel.z += bu->getHeight();
		originVoxel.z -= 4;
		if (originVoxel.z >= (_origin.z + 1)*24)
		{
			_origin.z++;
		}
		direction = bu->getDirection();
		if (bu->getTurretType() != -1)
			direction = bu->getTurretDirection();
		originVoxel.x += dirXshift[direction+offset]*bu->getArmor()->getSize();
		originVoxel.y += dirYshift[direction+offset]*bu->getArmor()->getSize();
	}
	else
	{
		// don't take into account soldier height and terrain level if the projectile is not launched from a soldier(from a waypoint)
		originVoxel.x += 8;
		originVoxel.y += 8;
		originVoxel.z += 12;
	}

	if (_action.type == BA_LAUNCH)
	{
		// target nothing, targets the middle of the tile
		targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24 + 12);
	}
	else
	{
		// determine the target voxel.
		// aim at the center of the unit, the object, the walls or the floor (in that priority)
		// if there is no LOF to the center, try elsewhere (more outward).
		// Store this target voxel.
		Tile *targetTile = _save->getTile(_action.target);
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
				targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24);
			}
		}
		else
		{
			// target nothing, targets the middle of the tile
			targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24 + 10);
		}
		test = _save->getTileEngine()->calculateLine(originVoxel, targetVoxel, false, &_trajectory, bu);
		if (test == 4 && !_trajectory.empty())
		{
			hitPos = Position(_trajectory.at(0).x/16, _trajectory.at(0).y/16, _trajectory.at(0).z/24);
			if (_save->getTile(hitPos) && _save->getTile(hitPos)->getUnit() == 0) //no unit? must be lower
			{
				hitPos = Position(hitPos.x, hitPos.y, hitPos.z-1);
			}
		}
		if (test != -1 && !_trajectory.empty() && _action.actor->getFaction() == FACTION_PLAYER && _action.autoShotCounter == 1)
		{
			//skip already estimated hitPos
			if (test != 4)
			{
				hitPos = Position(_trajectory.at(0).x/16, _trajectory.at(0).y/16, _trajectory.at(0).z/24);
			}
			if (hitPos != _action.target && _action.result == "")
			{
				if (test == 2)
				{
					if (hitPos.y - 1 == _action.target.y)
					{
						_trajectory.clear();
						return _save->getTileEngine()->calculateLine(originVoxel, targetVoxel, true, &_trajectory, bu);
					}
				}
				if (test == 1)
				{
					if (hitPos.x - 1 == _action.target.x)
					{
						_trajectory.clear();
						return _save->getTileEngine()->calculateLine(originVoxel, targetVoxel, true, &_trajectory, bu);
					}
				}
				_trajectory.clear();
				return -1;
			}
		}
		_trajectory.clear();
	}

	// apply some accuracy modifiers (todo: calculate this)
	// This will results in a new target voxel
	if (_action.type != BA_LAUNCH)
		applyAccuracy(originVoxel, &targetVoxel, accuracy, _action.type == BA_LAUNCH);

	// finally do a line calculation and store this trajectory.
	return _save->getTileEngine()->calculateLine(originVoxel, targetVoxel, true, &_trajectory, bu);
}

/**
 * calculateTrajectory.
 * @return true when a trajectory is possible.
 */
bool Projectile::calculateThrow(double accuracy)
{
	Position originVoxel, targetVoxel;
	bool foundCurve = false;

	// object blocking - can't throw here
	if (_action.type == BA_THROW &&_save->getTile(_action.target) && _save->getTile(_action.target)->getMapData(MapData::O_OBJECT) && _save->getTile(_action.target)->getMapData(MapData::O_OBJECT)->getTUCost(MT_WALK) == 255)
	{
		return false;
	}

	originVoxel = Position(_origin.x*16 + 8, _origin.y*16 + 8, _origin.z*24);
	originVoxel.z += -_save->getTile(_origin)->getTerrainLevel();
	BattleUnit *bu = _save->getTile(_origin)->getUnit();
	if(!bu)
		bu = _save->getTile(Position(_origin.x, _origin.y, _origin.z-1))->getUnit();
	originVoxel.z += bu->getHeight();
	originVoxel.z -= 3;
	if (originVoxel.z >= (_origin.z + 1)*24)
	{
		_origin.z++;
	}


	// determine the target voxel.
	// aim at the center of the floor
	targetVoxel = Position(_action.target.x*16 + 8, _action.target.y*16 + 8, _action.target.z*24 + 2);

	// we try 4 different curvatures to try and reach our goal.
	double curvature = 1.0;
	while (!foundCurve && curvature < 5.0)
	{
		_save->getTileEngine()->calculateParabola(originVoxel, targetVoxel, false, &_trajectory, bu, curvature, 1.0);
		if ((int)_trajectory.at(0).x/16 == (int)targetVoxel.x/16 && (int)_trajectory.at(0).y/16 == (int)targetVoxel.y/16)
		{
			foundCurve = true;
		}
		else
		{
			curvature += 1.0;
		}
		_trajectory.clear();
	}
	if (curvature == 5.0)
	{
		return false;
	}

	// apply some accuracy modifiers
	if (accuracy > 1)
		accuracy = 1;
	static const double maxDeviation = 0.08;
	static const double minDeviation = 0;
	double baseDeviation = (maxDeviation - (maxDeviation * accuracy)) + minDeviation;
	double deviation = RNG::boxMuller(0, baseDeviation);

	_trajectory.clear();
	// finally do a line calculation and store this trajectory.
	_save->getTileEngine()->calculateParabola(originVoxel, targetVoxel, true, &_trajectory, bu, curvature, 1.0 + deviation);

	Position endPoint = _trajectory.at(_trajectory.size() - 1);
	endPoint.x /= 16;
	endPoint.y /= 16;
	endPoint.z /= 24;
	// check if the item would land on a tile with a blocking object, if so then we let it fly without deviation, it must land on a valid tile in that case
	if (_save->getTile(endPoint) && _save->getTile(endPoint)->getMapData(MapData::O_OBJECT) && _save->getTile(endPoint)->getMapData(MapData::O_OBJECT)->getTUCost(MT_WALK) == 255)
	{
		_trajectory.clear();
		// finally do a line calculation and store this trajectory.
		_save->getTileEngine()->calculateParabola(originVoxel, targetVoxel, true, &_trajectory, bu, curvature, 1.0);
	}


	return true;
}


/**
 * applyAccuracy calculates the new target in voxel space, based on the given accuracy modifier.
 * @param origin Startposition of the trajectory.
 * @param target Endpoint of the trajectory.
 * @param accuracy Accuracy modifier.
 */
void Projectile::applyAccuracy(const Position& origin, Position *target, double accuracy, bool keepRange)
{
	int xdiff = origin.x - target->x;
	int ydiff = origin.y - target->y;
	double realDistance = sqrt((double)(xdiff*xdiff)+(double)(ydiff*ydiff));
	// maxRange is the maximum range a projectile shall ever travel in voxel space
	double maxRange = keepRange?realDistance:16*1000; // 1000 tiles

	/*
	This modifies the accuracy based on the distance from the target. The accuracy decreases linearly (2% per tile or 0.125% per voxel) when shooting beyond the limit of the firing mode:
	auto shot: 7 tiles or 112 voxels
	snap shot: 15 tiles or 240 voxels
	aimed shot: no penalty
	*/
	if (Options::getBool("battleRangeBasedAccuracy"))
	{
		if (_action.type == BA_AUTOSHOT && realDistance > 112)
		{
			accuracy -= 0.00125 * (realDistance - 112);
		}
		if (_action.type == BA_SNAPSHOT && realDistance > 240)
		{
			accuracy -= 0.00125 * (realDistance - 240);
		}
	}
	// maxDeviation is the max angle deviation for accuracy 0% in degrees
	double maxDeviation = 2.5;
	// minDeviation is the min angle deviation for accuracy 100% in degrees
	double minDeviation = 0.4;
	double dRot, dTilt;
	double rotation, tilt;
	double baseDeviation = (maxDeviation - (maxDeviation * accuracy)) + minDeviation;
	// the angle deviations are spread using a normal distribution between 0 and baseDeviation
	// check if we hit
	if (RNG::generate(0.0, 1.0) < accuracy)
	{
		// we hit, so no deviation
		dRot = 0;
		dTilt = 0;
	}
	else
	{
		dRot = RNG::boxMuller(0, baseDeviation);
		dTilt = RNG::boxMuller(0, baseDeviation / 2.0); // tilt deviation is halved
	}
	rotation = atan2(double(target->y - origin.y), double(target->x - origin.x)) * 180 / M_PI;
	tilt = atan2(double(target->z - origin.z),
		sqrt(double(target->x - origin.x)*double(target->x - origin.x)+double(target->y - origin.y)*double(target->y - origin.y))) * 180 / M_PI;
	// add deviations
	rotation += dRot;
	tilt += dTilt;
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

/**
 * Move further in the trajectory.
 * @return false if the trajectory is finished - no new position exists in the trajectory.
 */
bool Projectile::move()
{
	_position++;
	if (_position == _trajectory.size())
	{
		_position--;
		return false;
	}
	_position++;
	if (_position == _trajectory.size())
	{
		_position--;
		return false;
	}
	else
	{
		return true;
	}
}

/**
 * Get the current position in voxel space.
 * @param offset
 * @return position in voxel space.
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
 * Get a particle from the particle array.
 * @param i
 * @return particle id
 */
int Projectile::getParticle(int i) const
{
	if (_action.weapon->getRules()->getBulletSprite() == -1)
		return 0;
	else
		return _trail[_action.weapon->getRules()->getBulletSprite()][i];
}

/**
 * Get the project tile item.
 * Returns 0 when there is no item thrown.
 * @return pointer to BattleItem
 */
BattleItem *Projectile::getItem() const
{
	if (_action.type == BA_THROW)
		return _action.weapon;
	else
		return 0;
}

/**
 * Get the bullet sprite.
 * @return pointer to Surface
 */
Surface *Projectile::getSprite() const
{
	return _sprite;
}

}
