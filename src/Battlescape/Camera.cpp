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
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include "Camera.h"
#include "Map.h"
#include "Position.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"
#include "../Engine/Timer.h"

namespace OpenXcom
{

/**
 * Sets up a camera.
 */
Camera::Camera(int spriteWidth, int spriteHeight, int mapsize_x, int mapsize_y, int mapsize_z, Map *map, int visibleMapHeight) : _scrollTimer(0), _spriteWidth(spriteWidth), _spriteHeight(spriteHeight), _mapsize_x(mapsize_x), _mapsize_y(mapsize_y), _mapsize_z(mapsize_z), _screenWidth(map->getWidth()), _screenHeight(map->getHeight()),
																																_mapOffset(-250,250,0), _center(), _scrollX(0), _scrollY(0), _visibleMapHeight(visibleMapHeight), _showAllLayers(false), _map(map)
{
}

/**
 * Deletes the Camera.
 */
Camera::~Camera()
{

}

void Camera::setScrollTimer(Timer *timer)
{
	_scrollTimer = timer;
}

/**
 * Sets the value to min if it is below min, sets value to max if above max.
 * @param value pointer to the value
 * @param minimum value
 * @param maximum value
 */
void Camera::minMaxInt(int *value, const int minValue, const int maxValue) const
{
	if (*value < minValue)
	{
		*value = minValue;
	}
	else if (*value > maxValue)
	{
		*value = maxValue;
	}
}


/**
 * Handles map mouse shortcuts.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Camera::mouseClick(Action *action, State *)
{
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP)
	{
		up();
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN)
	{
		down();
	}
}


/**
 * Handles mouse over events.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Camera::mouseOver(Action *action, State *)
{
	if (_map->getCursorType() == CT_NONE)
	{
		return;
	}

	if (Options::getInt("battleScrollType") == SCROLL_AUTO)
	{
		int posX = action->getXMouse();
		int posY = action->getYMouse();

		if (posX < (SCROLL_BORDER * action->getXScale()) && posX > 0)
		{
			_scrollX = Options::getInt("battleScrollSpeed");
			// if close to top or bottom, also scroll diagonally
			if (posY < (SCROLL_DIAGONAL_EDGE * action->getYScale()) && posY > 0)
			{
				_scrollY = Options::getInt("battleScrollSpeed")/2;
			}
			else if (posY > (_screenHeight - SCROLL_DIAGONAL_EDGE) * action->getYScale())
			{
				_scrollY = -Options::getInt("battleScrollSpeed")/2;
			}
		}
		else if (posX > (_screenWidth - SCROLL_BORDER) * action->getXScale())
		{
			_scrollX = -Options::getInt("battleScrollSpeed");
			// if close to top or bottom, also scroll diagonally
			if (posY < (SCROLL_DIAGONAL_EDGE * action->getYScale()) && posY > 0)
			{
				_scrollY = Options::getInt("battleScrollSpeed")/2;
			}
			else if (posY > (_screenHeight - SCROLL_DIAGONAL_EDGE) * action->getYScale())
			{
				_scrollY = -Options::getInt("battleScrollSpeed")/2;
			}
		}
		else if (posX)
		{
			_scrollX = 0;
		}

		if (posY < (SCROLL_BORDER * action->getYScale()) && posY > 0)
		{
			_scrollY = Options::getInt("battleScrollSpeed");
			// if close to left or right edge, also scroll diagonally
			if (posX < (SCROLL_DIAGONAL_EDGE * action->getXScale()) && posX > 0)
			{
				_scrollX = Options::getInt("battleScrollSpeed");
				_scrollY /=2;
			}
			else if (posX > (_screenWidth - SCROLL_DIAGONAL_EDGE) * action->getXScale())
			{
				_scrollX = -Options::getInt("battleScrollSpeed");
				_scrollY /=2;
			}
		}
		else if (posY > (_screenHeight- SCROLL_BORDER) * action->getYScale())
		{
			_scrollY = -Options::getInt("battleScrollSpeed");
			// if close to left or right edge, also scroll diagonally
			if (posX < (SCROLL_DIAGONAL_EDGE * action->getXScale()) && posX > 0)
			{
				_scrollX = Options::getInt("battleScrollSpeed");
				_scrollY /=2;
			}
			else if (posX > (_screenWidth - SCROLL_DIAGONAL_EDGE) * action->getXScale())
			{
				_scrollX = -Options::getInt("battleScrollSpeed");
				_scrollY /=2;
			}
		}
		else if (posY && _scrollX == 0)
		{
			_scrollY = 0;
		}

		if ((_scrollX || _scrollY) && !_scrollTimer->isRunning())
		{
			_scrollTimer->start();
		}
		else if ((!_scrollX && !_scrollY) && _scrollTimer->isRunning())
		{
			_scrollTimer->stop();
		}
	}
}


/**
 * Handle scrolling.
 */
void Camera::scroll()
{
	scrollXY(_scrollX, _scrollY, true);
}

/**
 * Handle scrolling with given deviation.
 */
bool Camera::scrollXY(int x, int y, bool redraw)
{
	bool result = true;

	_mapOffset.x += x;
	_mapOffset.y += y;

	convertScreenToMap((_screenWidth / 2), (_visibleMapHeight / 2), &_center.x, &_center.y);
// if center goes out of map bounds, hold the scrolling
	if (_center.x > _mapsize_x -1 || _center.x < 0 || 
		_center.y > _mapsize_y -1  || _center.y < 0 )
	{
		_mapOffset.x -= x;
		_mapOffset.y -= y;
		result = false;
	}
	if (redraw) _map->draw();
	return result;
}


/**
 * Handle jumping with given deviation.
 */
void Camera::jumpXY(int x, int y)
{
	int oldX = _mapOffset.x, oldY= _mapOffset.y;

	_mapOffset.x += x;
	_mapOffset.y += y;

	convertScreenToMap((_screenWidth / 2), (_visibleMapHeight / 2), &_center.x, &_center.y);
// if center goes out of map bounds, hold the scrolling
	if (_center.x > _mapsize_x -1 || _center.x < 0 || 
		_center.y > _mapsize_y -1  || _center.y < 0 )
	{
		for (int k=1; k<128; ++k) //try intermediate values
		{
			_mapOffset.x = oldX + x*k/128;
			_mapOffset.y = oldY + y*k/128;
			convertScreenToMap((_screenWidth / 2), (_visibleMapHeight / 2), &_center.x, &_center.y);
			if (_center.x > _mapsize_x -1 || _center.x < 0 || 
				_center.y > _mapsize_y -1  || _center.y < 0 )
			{
				//step back
				_mapOffset.x = oldX + x*(k-1)/128;
				_mapOffset.y = oldY + y*(k-1)/128;
				break;
			}
		}
	}
}


/**
 * Go one level up.
 */
void Camera::up()
{
	if (_mapOffset.z < _mapsize_z - 1)
	{
		_mapOffset.z++;
		_mapOffset.y += _spriteHeight / 2;
		_map->draw();
	}
}

/**
 * Go one level down.
 */
void Camera::down()
{
	if (_mapOffset.z > 0)
	{
		_mapOffset.z--;
		_mapOffset.y -= _spriteHeight / 2;
		_map->draw();
	}
}

/**
 * Set view level.
 * @param view level
 */
void Camera::setViewLevel(int viewlevel)
{
	_mapOffset.z = viewlevel;
	minMaxInt(&_mapOffset.z, 0, _mapsize_z-1);
	_map->draw();
}


/**
 * Center map on a certain position.
 * @param mapPos Position to center on.
 * @param redraw Redraw map or not.
 */
void Camera::centerOnPosition(const Position &mapPos, bool redraw)
{
	Position screenPos;
	_center = mapPos;
	minMaxInt(&_center.x, -1, _mapsize_y);
	minMaxInt(&_center.y, -1, _mapsize_x);
	convertMapToScreen(_center, &screenPos);

	_mapOffset.x = -(screenPos.x - (_screenWidth / 2));
	_mapOffset.y = -(screenPos.y - (_visibleMapHeight / 2));

	_mapOffset.z = _center.z;
	if (redraw) _map->draw();
}

/**
 * Get map's center position.
 */
Position Camera::getCenterPosition()
{
	_center.z = _mapOffset.z;
	return _center;
}

/**
 * Converts screen coordinates to map coordinates.
 * @param screenX screen x position
 * @param screenY screen y position
 * @param mapX map x position
 * @param mapY map y position
 */
void Camera::convertScreenToMap(int screenX, int screenY, int *mapX, int *mapY) const
{
	// add half a tileheight to the mouseposition per layer we are above the floor
	screenY += (-_spriteWidth/2) + (_mapOffset.z) * ((_spriteHeight + _spriteWidth / 4) / 2);

	// calculate the actual x/y pixelposition on a diamond shaped map
	// taking the view offset into account
	*mapY = - screenX + _mapOffset.x + 2 * screenY - 2 * _mapOffset.y;
	*mapX = screenY - _mapOffset.y - *mapY / 4 - (_spriteWidth/4);

	// to get the row&col itself, divide by the size of a tile
	*mapX /= (_spriteWidth / 4);
	*mapY /= _spriteWidth;

	minMaxInt(mapX, -1, _mapsize_y);
	minMaxInt(mapY, -1, _mapsize_x);
}

/**
 * Convert map coordinates X,Y,Z to screen positions X, Y.
 * @param mapPos X,Y,Z coordinates on the map.
 * @param screenPos to screen position.
 */
void Camera::convertMapToScreen(const Position &mapPos, Position *screenPos) const
{
	screenPos->z = 0; // not used
	screenPos->x = mapPos.x * (_spriteWidth / 2) - mapPos.y * (_spriteWidth / 2);
	screenPos->y = mapPos.x * (_spriteWidth / 4) + mapPos.y * (_spriteWidth / 4) - mapPos.z * ((_spriteHeight + _spriteWidth / 4) / 2);
}

/**
 * Convert voxel coordinates X,Y,Z to screen positions X, Y.
 * @param voxelPos X,Y,Z coordinates on the voxel.
 * @param screenPos to screen position.
 */
void Camera::convertVoxelToScreen(const Position &voxelPos, Position *screenPos) const
{
	Position mapPosition = Position(voxelPos.x / 16, voxelPos.y / 16, voxelPos.z / 24);
	convertMapToScreen(mapPosition, screenPos);
	double dx = voxelPos.x - (mapPosition.x * 16);
	double dy = voxelPos.y - (mapPosition.y * 16);
	double dz = voxelPos.z - (mapPosition.z * 24);
	screenPos->x += (int)(dx - dy) + (_spriteWidth/2);
	screenPos->y += (int)(((_spriteHeight / 2.0)) + (dx / 2.0) + (dy / 2.0) - dz);
	screenPos->x += _mapOffset.x;
	screenPos->y += _mapOffset.y;
}

/**
 * Get the displayed level
 * @return the displayed layer
*/
int Camera::getViewLevel() const
{
	return _mapOffset.z;
}

/**
 * Get the map size x
 * @return the map size x
*/
int Camera::getMapSizeX() const
{
	return _mapsize_x;
}

/**
 * Get the map size y
 * @return the map size y
*/
int Camera::getMapSizeY() const
{
	return _mapsize_y;
}

Position Camera::getMapOffset()
{
	return _mapOffset;
}

void Camera::setMapOffset(Position pos)
{
	_mapOffset = pos;
}

int Camera::toggleShowAllLayers()
{
	_showAllLayers = !_showAllLayers;
	return _showAllLayers?2:1;
}

bool Camera::getShowAllLayers() const
{
	return _showAllLayers;
}

/**
 * Check if map coordinates X,Y,Z are on screen
 */
bool Camera::isOnScreen(const Position &mapPos) const
{
	Position screenPos;
	convertMapToScreen(mapPos, &screenPos);
	screenPos.x += _mapOffset.x;
	screenPos.y += _mapOffset.y;
	return screenPos.x >= -24
		&& screenPos.x <= _screenWidth + 24
		&& screenPos.y >= -32
		&& screenPos.y <= _screenHeight - 48;
}

}
