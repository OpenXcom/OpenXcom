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
class Tile;

/**
 * A class that represents a projectile. Map is the owner of an instance of this class during its short life.
 * It calculates its own trajectory and then moves along this precalculated trajectory in voxel space.
 */
class Projectile
{
private:
	ResourcePack *_res;
	SavedBattleGame *_save;
	BattleAction _action;
	Position _origin, _targetVoxel;
	std::vector<Position> _trajectory;
	size_t _position;
	Surface *_sprite;
	int _speed;
	void applyAccuracy(const Position& origin, Position *target, double accuracy, bool keepRange, Tile *targetTile, bool extendLine);
public:
	/// Creates a new Projectile.
	Projectile(ResourcePack *res, SavedBattleGame *save, BattleAction action, Position origin, Position target);
	/// Cleans up the Projectile.
	~Projectile();
	/// Calculates the trajectory for a straight path.
	int calculateTrajectory(double accuracy);
	int calculateTrajectory(double accuracy, Position originVoxel);
	/// Calculates the trajectory for a curved path.
	int calculateThrow(double accuracy);
	/// Moves the projectile one step in its trajectory.
	bool move();
	/// Gets the current position in voxel space.
	Position getPosition(int offset = 0) const;
	/// Gets a particle from the particle array.
	int getParticle(int i) const;
	/// Gets the item.
	BattleItem *getItem() const;
	/// Gets the sprite.
	Surface *getSprite() const;
	/// Skips the bullet flight.
	void skipTrajectory();
	/// Gets the Position of origin for the projectile.
	Position getOrigin();
	/// Gets the targetted tile for the projectile.
	Position getTarget();
};

}

#endif
