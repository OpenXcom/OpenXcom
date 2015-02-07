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
#ifndef OPENXCOM_PARTICLE_H
#define OPENXCOM_PARTICLE_H

#include <SDL_types.h>

namespace OpenXcom
{

class Particle
{
private:
	float _xOffset, _yOffset, _density;
	Uint8 _color, _opacity, _size;
public:
	/// Create a particle.
	Particle(float xOffset, float yOffset, float density, Uint8 color, Uint8 opacity);
	/// Destroy a particle.
	~Particle();
	/// Animate a particle.
	bool animate();
	/// Get the size value.
	int getSize() { return _size; }
	/// Get the color.
	Uint8 getColor() { return _color; }
	/// Get the opacity.
	Uint8 getOpacity() {return _opacity / 5; }
	/// Get the horizontal shift.
	float getX() { return _xOffset; }
	/// Get the vertical shift.
	float getY() { return _yOffset; }
};

}

#endif
