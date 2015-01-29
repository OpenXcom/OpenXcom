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
	int _part, _animationFrame, _drawingRoutine;
	bool _helmet;
	/// Drawing routine for XCom soldiers in overalls, sectoids (routine 0),
	/// mutons (routine 10),
	/// aquanauts (routine 13),
	/// aquatoids, calcinites, deep ones, gill men, lobster men, tasoths (routine 14).
	void drawRoutine0();
	/// Drawing routine for floaters.
	void drawRoutine1();
	/// Drawing routine for XCom tanks.
	void drawRoutine2();
	/// Drawing routine for cyberdiscs.
	void drawRoutine3();
	/// Drawing routine for civilians, ethereals, zombies (routine 4),
	/// tftd civilians, tftd zombies (routine 16), more tftd civilians (routine 17).
	void drawRoutine4();
	/// Drawing routine for sectopods and reapers.
	void drawRoutine5();
	/// Drawing routine for snakemen.
	void drawRoutine6();
	/// Drawing routine for chryssalid.
	void drawRoutine7();
	/// Drawing routine for silacoids.
	void drawRoutine8();
	/// Drawing routine for celatids.
	void drawRoutine9();
	/// Drawing routine for TFTD tanks.
	void drawRoutine11();
	/// Drawing routine for hallucinoids (routine 12) and biodrones (routine 15).
	void drawRoutine12();
	/// Drawing routine for tentaculats.
	void drawRoutine19();
	/// Drawing routine for triscenes.
	void drawRoutine20();
	/// Drawing routine for xarquids.
	void drawRoutine21();
	/// sort two handed sprites out.
	void sortRifles();
public:
	/// Creates a new UnitSprite at the specified position and size.
	UnitSprite(int width, int height, int x, int y, bool helmet);
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
	/// Draws the unit.
	void draw();
};

}

#endif
