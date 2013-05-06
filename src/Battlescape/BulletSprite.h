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
#ifndef OPENXCOM_BULLETSPRITE_H
#define OPENXCOM_BULLETSPRITE_H

#include "../Engine/Surface.h"

namespace OpenXcom
{

/**
 * A class that renders a bullet sprite, which is a 3x3 Surface.
 * This small sprite either represents the bullet itself or it's trailing particles.
 */
class BulletSprite : public Surface
{
private:
	static const int pixels[37][9];
	int _type;
public:
	/// Creates a new BulletSprite.
	BulletSprite(int type);
	/// Cleans up the BulletSprite.
	~BulletSprite();
	/// Draw the bullet.
	void draw();
};

}

#endif
