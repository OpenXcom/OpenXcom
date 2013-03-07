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
#ifndef OPENXCOM_PROJECTILE_H
#define OPENXCOM_PROJECTILE_H

#include <vector>
#include "Position.h"
#include "BattlescapeGame.h"

namespace OpenXcom
{

class ResourcePack;
class BattleItem;
class SavedBattleGame;
class Surface;

/**
 * A class that represents a projectile. Map is the owner of an instance of this class during it's short life.
 * It calculates it's own trajectory and then moves along this precalculated trajectory in voxel space.
 */
class Projectile
{
private:
	ResourcePack *_res;
	SavedBattleGame *_save;
	BattleAction _action;
	Position _origin;
	std::vector<Position> _trajectory;
	unsigned int _position;
	static const int _trail[11][36];
	Surface *_sprite;
	void applyAccuracy(const Position& origin, Position *target, double accuracy, bool keepRange = false);
public:
	/// Creates a new Projectile.
	Projectile(ResourcePack *res, SavedBattleGame *save, BattleAction action, Position origin);
	/// Cleans up the Projectile.
	~Projectile();
	/// Calculates the trajectory for straight path.
	int calculateTrajectory(double accuracy);
	/// Calculates the trajectory for curved path.
	bool calculateThrow(double accuracy);
	/// Move the projectile one step in it's trajectory.
	bool move();
	/// Get the current position in voxel space.
	Position getPosition(int offset = 0) const;
	/// Get a particle from the particle array.
	int getParticle(int i) const;
	/// Get the item
	BattleItem *getItem() const;
	/// Get the sprite
	Surface *getSprite() const;
	/// skip the bullet flight
	void skipTrajectory();
};

}

#endif