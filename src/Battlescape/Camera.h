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
#include "Position.h"

namespace OpenXcom
{

class Action;
class State;
class Timer;
class Map;

/**
 * Class handling camera movement, either by mouse or by events on the battlescape.
 */
class Camera
{
private:
	Timer *_scrollMouseTimer, *_scrollKeyTimer;
	int _spriteWidth, _spriteHeight;
	int _mapsize_x, _mapsize_y, _mapsize_z;
	int _screenWidth, _screenHeight;
	Position _mapOffset, _center;
	int _scrollMouseX, _scrollMouseY, _scrollKeyX, _scrollKeyY;
	bool _scrollTrigger;
	int _visibleMapHeight;
	bool _showAllLayers;
	Map *_map;
public:
	static const int SCROLL_BORDER = 5;
	static const int SCROLL_DIAGONAL_EDGE = 60;
	/// Creates a new camera.
	Camera(int spriteWidth, int spriteHeight, int mapsize_x, int mapsize_y, int mapsize_z, Map *map, int visibleMapHeight);
	/// Cleans up the camera.
	~Camera();
	/// Sets the camera's scroll timers.
	void setScrollTimer(Timer *mouse, Timer *key);
	/// Special handling for mouse press.
	void mousePress(Action *action, State *state);
	/// Special handling for mouse release.
	void mouseRelease(Action *action, State *state);
	/// Special handling for mouse over.
	void mouseOver(Action *action, State *state);
	/// Special handling for key presses.
	void keyboardPress(Action *action, State *state);
	/// Special handling for key releases.
	void keyboardRelease(Action *action, State *state);
	/// Scrolls the view for mouse-scrolling.
	void scrollMouse();
	/// Scrolls the view for keyboard-scrolling.
	void scrollKey();
	/// Scrolls the view a certain amount.
	void scrollXY(int x, int y, bool redraw);
	/// Jumps the view (when projectile in motion).
	void jumpXY(int x, int y);
	/// Moves map layer up.
	void up();
	/// Move map layer down.
	void down();
	/// Sets the view level.
	void setViewLevel(int viewlevel);
	/// Converts map coordinates to screen coordinates.
	void convertMapToScreen(Position mapPos, Position *screenPos) const;
	/// Converts voxel coordinates to screen coordinates.
	void convertVoxelToScreen(Position voxelPos, Position *screenPos) const;
	/// Converts screen coordinates to map coordinates.
	void convertScreenToMap(int screenX, int screenY, int *mapX, int *mapY) const;
	/// Center map on a position.
	void centerOnPosition(Position pos, bool redraw = true);
	/// Gets map's center position.
	Position getCenterPosition();
	/// Gets the map displayed level.
	int getViewLevel() const;
	/// Gets the map size x.
	int getMapSizeX() const;
	/// Gets the map size y.
	int getMapSizeY() const;
	/// Get the map x/y screen offset.
	Position getMapOffset() const;
	/// Sets the map x/y screen offset.
	void setMapOffset(const Position& pos);
	/// Toggles showing all map layers.
	int toggleShowAllLayers();
	/// Checks if the camera is showing all map layers.
	bool getShowAllLayers() const;
	/// Checks if map coordinates X,Y,Z are on screen.
	bool isOnScreen(Position mapPos, const bool unitWalking, const int unitSize, const bool boundary) const;
	/// Resize the viewable area.
	void resize();
	/// stop mouse scrolling.
	void stopMouseScrolling();
};

}
