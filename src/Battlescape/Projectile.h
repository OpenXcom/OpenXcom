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

#include "../Engine/Surface.h"
#include "Position.h"

namespace OpenXcom
{

class ResourcePack;
class BattleItem;

/**
 * A class that represents & renders a bullet or a flying item.
 */
class Projectile : public Surface
{
private:
	ResourcePack *_res;
	BattleItem *_item;
	Position _origin;
	double _fi, _te, _ro;
public:
	/// Creates a new Projectile .
	Projectile(ResourcePack *res, Position _origin);
	/// Cleans up the Projectile.
	~Projectile();
	/// Sets the battleitem to be rendered.
	void setBattleItem(BattleItem *item);
	/// Sets the trajectory initial angles.
	bool setLaunchAngles(double fi, double te, double ro);
	/// Move the projectile one step.
	bool move();
	/// Draw the surface.
	void draw();
	/// Blit the surface.
	void blit(Surface *surface);
};

}

#endif
