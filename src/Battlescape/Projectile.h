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
#ifndef OPENXCOM_PROJECTILE_H
#define OPENXCOM_PROJECTILE_H

#include <vector>
#include "Position.h"

namespace OpenXcom
{

class ResourcePack;
class BattleItem;
class SavedBattleGame;

/**
 * A class that represents a projectile. BattleAction is the owner of an instance of this class during it's short life.
 * It calculates it's own trajectory and then moves along this precalculated trajectory in voxel space.
 */
class Projectile
{
private:
	ResourcePack *_res;
	SavedBattleGame *_save;
	BattleItem *_item;
	Position _origin, _target;
	std::vector<Position> _trajectory;
	int _position;
	static const int _trail[11][36];
	int _bulletType;
	int calculateLine(const Position& origin, const Position& target, bool store);
	void applyAccuracy(const Position& origin, Position *target, double accuracy);
	int voxelCheck(const Position& voxel);
public:
	/// Creates a new Projectile.
	Projectile(ResourcePack *res, SavedBattleGame *save, Position _origin, Position _target, int bulletType);
	/// Cleans up the Projectile.
	~Projectile();
	/// Calculates the trajectory.
	bool calculateTrajectory();
	/// Move the projectile one step in it's trajectory.
	bool move();
	/// Get the current position in voxel space.
	Position getPosition(int offset = 0) const;
	/// Get a particle from the particle array.
	int getParticle(int i);
};

}

#endif
