#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Engine/InteractiveSurface.h"

namespace OpenXcom
{

class BattleUnit;
class Game;

/**
 * Displays a view of units movement.
 */
class ScannerView : public InteractiveSurface
{
	Game * _game;
	/// Handle clicking
	void mouseClick (Action *action, State *state);
	BattleUnit *_unit;
	int _frame;
public:
	/// Create the ScannerView
	ScannerView (int w, int h, int x, int y, Game * game, BattleUnit *unit);
	/// Draw the scanner view
	void draw();
	void animate();
};
}
