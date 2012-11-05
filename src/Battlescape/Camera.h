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
#ifndef OPENXCOM_CAMERA_H
#define OPENXCOM_CAMERA_H

#include "Position.h"

namespace OpenXcom
{

class Action;
class State;
class Position;
class Timer;
class Map;

/**
 * Class handling camera movement, either by mouse or by events on the battlescape.
 */
class Camera
{
private:
	static const int SCROLL_BORDER = 5;
	static const int SCROLL_DIAGONAL_EDGE = 60;
	Timer *_scrollTimer;
	int _spriteWidth, _spriteHeight;
	int _mapWidth, _mapLength, _mapHeight;
	int _screenWidth, _screenHeight;
	Position _mapOffset, _center;
	int _scrollX, _scrollY;
	int _visibleMapHeight;
	bool _showAllLayers;
	void minMaxInt(int *value, const int minValue, const int maxValue) const;
	Map *_map;
public:
	/// Creates a new camera.
	Camera(int spriteWidth, int spriteHeight, int mapWidth, int mapLength, int mapHeight, Map *map, int visibleMapHeight);
	/// Cleans up the camera.
	~Camera();
	void setScrollTimer(Timer *timer);
	/// Special handling for mouse clicks.
	void mouseClick(Action *action, State *state);
	/// Special handling for mouse over
	void mouseOver(Action *action, State *state);
	/// Special handling for key presses.
	void keyboardPress(Action *action, State *state);
	/// Scrolls the view (eg when mouse is on the edge of the screen)
	void scroll();
	/// Scrolls the view (when mouse-scrolling)
	void scrollXY(int x, int y, bool redraw);
	/// move map layer up
	void up();
	/// move map layer down
	void down();
	/// set view height
	void setViewHeight(int viewheight);
	/// Converts map coordinates to screen coordinates.
	void convertMapToScreen(const Position &mapPos, Position *screenPos) const;
	/// Converts voxel coordinates to screen coordinates.
	void convertVoxelToScreen(const Position &voxelPos, Position *screenPos) const;
	/// Converts screen coordinates to map coordinates.
	void convertScreenToMap(int screenX, int screenY, int *mapX, int *mapY) const;
	/// Center map on a position.
	void centerOnPosition(const Position &pos, bool redraw = true);
	/// Get map's center position.
	Position getCenterPosition();
	/// Check if the camera was following a bullet.
	bool didCameraFollow();
	/// Get the map displayed level
	int getViewHeight() const;
	int getMapWidth() const;
	int getMapLength() const;
	/// Get the map x/y screen offset
	Position getMapOffset();
	/// Set the map x/y screen offset
	void setMapOffset(Position pos);
	int toggleShowAllLayers();
	bool getShowAllLayers() const;
	bool isOnScreen(const Position &mapPos) const;
};
}
#endif
