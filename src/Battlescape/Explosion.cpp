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
#include "Explosion.h"

namespace OpenXcom
{

/**
 * Sets up a Explosion sprite with the specified size and position.
 * @param position Explosion center position in voxel x/y/z.
 * @param startFrame A startframe - can be used to offset different explosions at different frames.
 * @param big Flag to indicate it is a bullet hit (false), or a real explosion (true).
 * @param hit True for melee and psi attacks.
 */
Explosion::Explosion(Position position, int startFrame, bool big, bool hit) : _position(position), _currentFrame(startFrame), _startFrame(startFrame), _big(big), _hit(hit)
{

}

/**
 * Deletes the Explosion.
 */
Explosion::~Explosion()
{

}

/**
 * Animates the explosion further.
 * @return false If the animation is finished.
 */
bool Explosion::animate()
{
	_currentFrame++;
	if ((_hit && _currentFrame == 4) || (_big && _currentFrame == 8) || (!_big && _currentFrame == _startFrame+10))
	{
		return false;
	}
	else
	{
		return true;
	}
}

/**
 * Gets the current position in voxel space.
 * @return position in voxel space.
 */
Position Explosion::getPosition() const
{
	return _position;
}

/**
 * Gets the current frame in the animation.
 * @return frame number.
 */
int Explosion::getCurrentFrame() const
{
	return _currentFrame;
}

/**
 * Returns flag to indicate if it is a bullet hit (false), or a real explosion (true).
 * @return True if it is a real explosion, false if it is a bullet hit.
 */
bool Explosion::isBig() const
{
	return _big;
}

/**
 * Returns flag to indicate if it is a melee or psi hit.
 * @return True if it is a melee hit or psi hit.
 */
bool Explosion::isHit() const
{
	return _hit;
}
}
