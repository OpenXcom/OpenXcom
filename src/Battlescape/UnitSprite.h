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
#ifndef OPENXCOM_UNITSPRITE_H
#define OPENXCOM_UNITSPRITE_H

#include "../Engine/Surface.h"

namespace OpenXcom
{

class BattleUnit;
class BattleItem;
class SurfaceSet;

/**
 * A class that renders a specific unit, given its render rules
 * combining the right frames from the surfaceset.
 */
class UnitSprite : public Surface
{
private:
	BattleUnit *_unit;
	BattleItem *_itemA, *_itemB;
	SurfaceSet *_unitSurface, *_itemSurfaceA, *_itemSurfaceB;
	int _part, _animationFrame;
	void drawRoutine0();
	void drawRoutine1();
	void drawRoutine2();
	void drawRoutine3();
	void drawRoutine4();
	void drawRoutine5();
	void drawRoutine6();
	void drawRoutine7();
	void drawRoutine8();
	void drawRoutine9();
	void drawRoutine10();
	int _drawingRoutine;
	/// sort two handed sprites out.
	void sortRifles();
public:
	/// Creates a new UnitSprite at the specified position and size.
	UnitSprite(int width, int height, int x, int y);
	/// Cleans up the UnitSprite.
	~UnitSprite();
	/// Sets surfacesets for rendering.
	void setSurfaces(SurfaceSet *unitSurface, SurfaceSet *itemSurfaceA, SurfaceSet *itemSurfaceB);
	/// Sets the battleunit to be rendered.
	void setBattleUnit(BattleUnit *unit, int part = 0);
	/// Sets the battleitem to be rendered.
	void setBattleItem(BattleItem *item);
	/// Sets the animation frame.
	void setAnimationFrame(int frame);
	/// Draw the unit.
	void draw();
};

}

#endif
