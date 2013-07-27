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
#ifndef OPENXCOM_EXPLOSION_H
#define OPENXCOM_EXPLOSION_H

#include "Position.h"

namespace OpenXcom
{

class ResourcePack;
class BattleItem;
class SavedBattleGame;

/**
 * A class that represents an explosion animation. Map is the owner of an instance of this class during it's short life.
 * It represents both a bullet hit, as a real explosion animation.
 */
class Explosion
{
private:
	Position _position;
	int _currentFrame, _startFrame;
	bool _big, _hit;
public:
	/// Creates a new Explosion.
	Explosion(Position _position, int startFrame, bool big, bool hit = false);
	/// Cleans up the Explosion.
	~Explosion();
	/// Move the Explosion one frame.
	bool animate();
	/// Get the current position in voxel space.
	Position getPosition() const;
	/// Get a the current frame.
	int getCurrentFrame() const;
	/// Is big?
	bool isBig() const;
	bool isHit() const;
};

}

#endif
