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
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/Soldier.h"

namespace OpenXcom
{

/**
 * Sets up a UnitSprite with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Projectile::Projectile(ResourcePack *res, Position origin) : Surface(32, 32, 0, 0), _res(res), _item(0), _origin(origin)
{


}

/**
 * Deletes the Projectile.
 */
Projectile::~Projectile()
{

}

/**
 * Links this sprite to a BattleItem to get the data for rendering.
 * @param unit Pointer to the BattleItem.
 */
void Projectile::setBattleItem(BattleItem *item)
{
	_item = item;
}

bool Projectile::setLaunchAngles(double fi, double te, double ro)
{
	_fi = fi;
	_te = te;
	_ro = ro;

	return true;
}

bool Projectile::move()
{
	return true;
}

/**
 * Draws a projectile.
 */
void Projectile::draw()
{

}

/**
 * Blits the unit onto another surface. 
 * @param surface Pointer to another surface.
 */
void Projectile::blit(Surface *surface)
{
	Surface::blit(surface);
}

}
