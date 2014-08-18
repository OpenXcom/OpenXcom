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
#ifndef OPENXCOM_ITEMSPRITE_H
#define OPENXCOM_ITEMSPRITE_H

#include "../Engine/Surface.h"
#include "../Engine/Script.h"

namespace OpenXcom
{

class BattleUnit;
class BattleItem;
class SurfaceSet;
class ResourcePack;

/**
 * A class that renders a specific unit, given its render rules
 * combining the right frames from the surfaceset.
 */
class ItemSprite
{
private:
	SurfaceSet *_itemSurface;
	ResourcePack *_res;
	int _animationFrame;
	Surface *_dest;
	ScriptWorker _scriptWorkRef;

public:
	/// Creates a new ItemSprite at the specified position and size.
	ItemSprite(Surface* dest, ResourcePack* res, int frame);
	/// Cleans up the ItemSprite.
	~ItemSprite();
	/// Draws the unit.
	void draw(BattleItem* item, int x, int y, int shade);
};

} //namespace OpenXcom

#endif
