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
#ifndef OPENXCOM_UnitSprite_H
#define OPENXCOM_UnitSprite_H

#include "../Engine/Surface.h"

class ResourcePack;
class BattleUnit;

/**
 * A class that renders a specific unit, given its render rules
 * combining the right frames from the surfaceset.
 */
class UnitSprite : public Surface
{
private:
	ResourcePack *_res;
	BattleUnit *_unit;
public:
	/// Creates a new UnitSprite at the specified position and size.
	UnitSprite(int width, int height, int x, int y);
	/// Cleans up the UnitSprite.
	~UnitSprite();
	/// Resourcepack contains surfaceset for rendering.
	void setResourcePack(ResourcePack *res);
	/// Sets the battleunit to be rendered.
	void setBattleUnit(BattleUnit *unit);
	/// Draw the surface.
	void draw();
	/// Blit the surface.
	void blit(Surface *surface);
};

#endif
