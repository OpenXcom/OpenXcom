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
#include "Projectile.h"
#include "../Engine/SurfaceSet.h"
#include "../Battlescape/Position.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleUnitSprite.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/MapData.h"
#include "../Ruleset/MapModel.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Engine/RNG.h"

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
 * @param origin Projectile's start position in tile x/y/z.
 * @param target Projectile's target position in tile x/y/z.
 * @param bulletType
 */
Projectile::Projectile(ResourcePack *res, SavedBattleGame *save, Position origin, Position target, int bulletType) : _res(res), _save(save), _origin(origin), _target(target), _position(0), _bulletType(bulletType)
{

}

/**
 * Deletes the Projectile.
 */
Projectile::~Projectile()
{

}

/**
 * calculateTrajectory.
 * @return true when a trajectory is possible.
 */
bool Projectile::calculateTrajectory()
{
	Position originVoxel, targetVoxel;
	// First determine the origin voxel. This depends on soldier direction, stance, terrain level...
	// For testing we some voxel in the middle.
	originVoxel = Position(_origin.x*16 + 8, _origin.y*16 + 8, _origin.z*24 + 19);

	//  determine the target voxel.
	// aim at the center of the unit, the object, the walls or the floor (in that priority)
	// if there is no LOF to the center, try elsewhere (more outward).
	// Store this target voxel.
	Tile *tile = _save->getTile(_target);
	if (tile->getUnit() != 0)
	{
		targetVoxel = Position(_target.x*16 + 8, _target.y*16 + 8, _target.z*24 + 19);
	}
	else if (tile->getMapData(O_OBJECT) != 0)
	{
		targetVoxel = Position(_target.x*16 + 8, _target.y*16 + 8, _target.z*24 + 10);
	}
	else if (tile->getMapData(O_NORTHWALL) != 0)
	{
		targetVoxel = Position(_target.x*16 + 8, _target.y*16 + 16, _target.z*24 + 10);
	}
	else if (tile->getMapData(O_WESTWALL) != 0)
	{
		targetVoxel = Position(_target.x*16, _target.y*16 + 8, _target.z*24 + 10);
	}
	else if (tile->getMapData(O_FLOOR) != 0)
	{
		targetVoxel = Position(_target.x*16 + 8, _target.y*16 + 8, _target.z*24);
	}
	else
	{
		return false; // no line of fire
	}

	// apply some accuracy modifiers (todo: calculate this)
	// This will results in a new target voxel
	applyAccuracy(originVoxel, &targetVoxel, 1.0); // test

	// finally do a line calculation and store this trajectory.
	calculateLine(originVoxel, targetVoxel, true);

	return true;
}

/**
 * applyAccuracy calculates the new target in voxel space, based on the given accuracy modifier.
 * @param origin Startposition of the trajectory.
 * @param target Endpoint of the trajectory.
 * @param accuracy Accuracy modifier.
 */
void Projectile::applyAccuracy(const Position& origin, Position *target, double accuracy)
{
	// maxDeviation is the max angle deviation for accuracy 0% in degrees
	static const double maxDeviation = 10;
	// minDeviation is the max angle deviation for accuracy 100% in degrees
	static const double minDeviation = 1;
	// maxRange is the maximum range a projectile shall ever travel in voxel space
	static const double maxRange = 16*1000; // 1000 tiles

	double dRot, dTilt;
	double rotation, tilt;
	double baseDeviation = (maxDeviation - (maxDeviation * accuracy)) + minDeviation;
	// the angle deviations are spread using a normal distribution between 0 and baseDeviation
	dRot = RNG::getNormal() * baseDeviation;
	dTilt = RNG::getNormal() * baseDeviation;
	// or in the other direction perhaps
	if (RNG::generate(0,1) == 1)
		dRot *= -1;
	if (RNG::generate(0,1) == 1)
		dTilt *= -1;
	dTilt /= 10.0; // test
	rotation = atan2(double(target->y - origin.y), double(target->x - origin.x)) * 180 / M_PI;
	tilt = atan2(double(target->z - origin.z),
		sqrt(double(target->x - origin.x)*double(target->x - origin.x)+double(target->y - origin.y)*double(target->y - origin.y))) * 180 / M_PI;
	// add deviations
	rotation += dRot;
	tilt += dTilt;
	// calculate new target
	// this can be very far out of the map, but we don't care about that right now
	double cos_fi = cos(tilt * M_PI / 180.0);
	double sin_fi = sin(tilt * M_PI / 180.0);
	double cos_te = cos(rotation * M_PI / 180.0);
	double sin_te = sin(rotation * M_PI / 180.0);
	target->x = (int)(origin.x + maxRange * cos_te * cos_fi);
	target->y = (int)(origin.y + maxRange * sin_te * cos_fi);
	target->z = (int)(origin.z + maxRange * sin_fi);
}

/**
 * calculateLine. Using bresenham algorithm in 3D.
 * @param origin
 * @param target
 * @param store
 * @return the objectnumber(0-3) or unit(4) or out of map (5) or -1(hit nothing)
 */
int Projectile::calculateLine(const Position& origin, const Position& target, bool store)
{
	int x, x0, x1, delta_x, step_x;
    int y, y0, y1, delta_y, step_y;
    int z, z0, z1, delta_z, step_z;
    int swap_xy, swap_xz;
    int drift_xy, drift_xz;
    int cx, cy, cz;

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
        
		if (store)
		{
			_trajectory.push_back(Position(cx, cy, cz));
		}
        //passes through this point?
		int result = voxelCheck(Position(cx, cy, cz));
		if (result != -1)
		{
			return result;
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
 * check if we hit a voxel.
 * @return the objectnumber(0-3) or unit(4) or out of map (5) or -1(hit nothing)
 */
int Projectile::voxelCheck(const Position& voxel)
{
	//todo add unit models

	for (int i=0; i< 4; i++)
	{
		int tx = voxel.x/16;
		int ty = voxel.y/16;
		int tz = voxel.z/24;
		Tile *tile = _save->getTile(Position(voxel.x/16, voxel.y/16, voxel.z/24));
		// check if we are not out of the map
		if (tile == 0)
		{
			return 5;
		}
		MapData *mp = tile->getMapData(i);
		if (mp != 0 && mp->getModel()->getVoxel(voxel.x%16, voxel.y%16, voxel.z%24))
		{
			return i;
		}
	}
	return -1;
}

/**
 * Move one step further in the trajectory.
 * @return false if the trajectory is finished - no new position exists in the trajectory.
 */
bool Projectile::move()
{
	_position++;
	if (_trajectory.size() == _position)
	{
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
	if (_position + offset >= 0)
		return _trajectory.at(_position + offset);
	else
		return _trajectory.at(_position);
}

/**
 * Get a particle from the particle array.
 * @param i
 * @return particle id
 */
int Projectile::getParticle(int i)
{
	return _trail[_bulletType][i];
}

}
