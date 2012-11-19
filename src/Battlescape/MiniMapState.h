/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_MINIMAP_H
#define OPENXCOM_MINIMAP_H

#include "../Engine/State.h"

namespace OpenXcom
{
class Camera;
class InteractiveSurface;
class Text;
class MiniMapView;
class Timer;
class SavedBattleGame;

/**
   The MiniMap is a representation of a Battlescape map. Allow to see more of the map.
*/
class MiniMapState : public State
{
	InteractiveSurface * _surface;
	MiniMapView * _miniMapView;
	Text * _txtLevel;
	Timer *_timerAnimate;
	/// Handle Minimap animation
	void animate();
public:
	/// Create the MiniMapState
	MiniMapState (Game * game, Camera * camera, SavedBattleGame * battleGame);
	/// Handler for the OK button
	void btnOkClick (Action * action);
	/// Handler for the one level up button
	void btnLevelUpClick (Action * action);
	/// Handler for the one level down button
	void btnLevelDownClick (Action * action);
	/// Handler for right-clicking anything.
	void handle(Action *action);
	/// Handle timers
	void think ();
};
}

#endif
