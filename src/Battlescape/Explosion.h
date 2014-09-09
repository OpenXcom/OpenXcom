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
#ifndef OPENXCOM_EXPLOSION_H
#define OPENXCOM_EXPLOSION_H

#include "Position.h"

namespace OpenXcom
{

/**
 * A class that represents an explosion animation. Map is the owner of an instance of this class during its short life.
 * It represents both a bullet hit, as a real explosion animation.
 */
class Explosion
{
private:
	Position _position;
	int _currentFrame, _startFrame, _endFrame;
	bool _big, _hit;
	std::string _spriteSheet;
public:
	/// Creates a new Explosion.
	Explosion(Position _position, int startFrame, bool big, bool hit = false, int endFrame = 0, std::string spriteSheet = "SMOKE.PCK");
	/// Cleans up the Explosion.
	~Explosion();
	/// Moves the Explosion on one frame.
	bool animate();
	/// Gets the current position in voxel space.
	Position getPosition() const;
	/// Sets the current frame.  If this is less than the start frame, the start frame will show
	void setCurrentFrame(int currentFrame);
	/// Gets the current frame.
	int getCurrentFrame() const;
	/// Checks if this is a real explosion.
	bool isBig() const;
	/// Checks if this is a melee or psi hit.
	bool isHit() const;
	/// Gets the spritesheet used by this explosion
	std::string getSpriteSheet() const {return _spriteSheet;}
};

}

#endif
