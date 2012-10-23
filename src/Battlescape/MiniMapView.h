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
#ifndef OPENXCOM_MINIMAPVIEW_H
#define OPENXCOM_MINIMAPVIEW_H

#include "../Engine/InteractiveSurface.h"
#include "Position.h"
#include <map>

namespace OpenXcom
{
class Game;
class Camera;
class SavedBattleGame;
class Tile;
class BattleUnit;
class SurfaceSet;
/**
   MiniMapView is the class used to display the map in the MiniMapState
*/
class MiniMapView : public InteractiveSurface
{
	Game * _game;
	Camera * _camera;
	SavedBattleGame * _battleGame;
	int _frame;
	SurfaceSet * _set;
	// these two are required for right-button scrolling on the minimap
	bool isMouseScrolling;
	bool isMouseScrolled;
	int xBeforeMouseScrolling, yBeforeMouseScrolling;
	int mouseScrollX, mouseScrollY;
	Position posBeforeMouseScrolling;
	Uint32 mouseScrollingStartTime;
	int absMouseMoveX, absMouseMoveY;
	bool mouseMovedOverThreshold;
	/// Handle pressing on the MiniMap
	void mousePress(Action *action, State *state);
	/// Handle clicking on the MiniMap
	void mouseClick(Action *action, State *state);
	/// Handle moving mouse over the MiniMap
	void mouseOver(Action *action, State *state);
public:
	/// Create the MiniMapView
	MiniMapView(int w, int h, int x, int y, Game * game, Camera * camera, SavedBattleGame * battleGame);
	/// Draw the minimap
	void draw();
	/// Change the displayed minimap level
	int up ();
	/// Change the displayed minimap level
	int down ();
	/// update minimap animation
	void animate();
};

}

#endif
