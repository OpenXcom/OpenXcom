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
#ifndef OPENXCOM_SCANNERSTATE_H
#define OPENXCOM_SCANNERSTATE_H

#include "../Engine/State.h"
#include "BattlescapeGame.h"

namespace OpenXcom
{
class InteractiveSurface;
class Timer;
class ScannerView;
class Text;
class BattleItem;
class BattleUnit;

/**
 * The Scanner User Interface.
 */
class ScannerState : public State
{
	InteractiveSurface *_surface1, *_surface2;
	ScannerView *_scannerView;
	BattleAction *_action;
	/// update scanner interface
	void update();
	Timer *_timerAnimate;
	/// Handle Minimap animation
	void animate();
public:
	/// Create the ScannerState
	ScannerState (Game * game, BattleAction *action);
	~ScannerState();
	/// Handler for right-clicking anything.
	void handle(Action *action);
	/// Handle timers
	void think ();
};
}

#endif
