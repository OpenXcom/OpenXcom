/*
 * Copyright 2010 OpenXcom Developers.
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
	Position _mapOffset;
	int _RMBClickX, _RMBClickY;
	int _scrollX, _scrollY;
	bool _RMBDragging;
	int _centerX, _centerY;
	bool _cameraFollowed;
	int _visibleMapHeight;
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
	/// Special handling for mous over
	void mouseOver(Action *action, State *state);
	/// Special handling for key presses.
	void keyboardPress(Action *action, State *state);
	/// Scrolls the view (eg when mouse is on the edge of the screen)
	void scroll();
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
	void convertScreenToMap(int screenX, int screenY, int *mapX, int *mapY) const;
	/// Center map on a unit.
	void centerOnPosition(const Position &pos, bool redraw = true);
	/// Check if the camera was following a bullet.
	bool didCameraFollow();
	/// Get the map displayed level
	int getViewHeight() const;
	/// Get the center X of displayed map
	int getCenterX() const;
	/// Get the center Y of displayed map
	int getCenterY() const;
	Position getMapOffset();
};

}

#endif
