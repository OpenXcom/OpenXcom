/*
 * Copyright 2011 OpenXcom Developers.
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
class Map;
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
	Map * _map;
	int _startX;
	int _startY;
	int _lvl;
	SavedBattleGame * _battleGame;
	int _frame;
	std::map<Tile *, BattleUnit *> _battleUnits;
	SurfaceSet * _set;
	/// Handle clicking on the MiniMap
	void mouseClick (Action *action, State *state);
public:
	/// Create the MiniMapView
	MiniMapView(int w, int h, int x, int y, Game * game, Map * map, SavedBattleGame * battleGame);
	/// Draw the minimap
	void draw();
	/// Blit the minimap to another surface
	void blit(Surface *surface);
	/// Change the displayed minimap level
	void up ();
	/// Change the displayed minimap level
	void down ();
	/// Get the displayed minimap level
	int getDisplayedLevel ();
	/// Set the displayed minimap level
	void setDisplayedLevel (int level);
	/// update minimap animation
	void animate();
	/// Return the minimap center position
	Position getCenter();
};

}

#endif
