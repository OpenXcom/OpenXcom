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
Camera::Camera(int spriteWidth, int spriteHeight, int mapWidth, int mapLength, int mapHeight, Map *map, int visibleMapHeight) : _scrollX(0), _scrollY(0), _RMBDragging(false), _spriteWidth(spriteWidth), _spriteHeight(spriteHeight), _mapWidth(mapWidth), _mapLength(mapLength), _mapHeight(mapHeight), _map(map), _visibleMapHeight(visibleMapHeight)
{
	_mapOffset = Position(-250,250,0);
	_screenWidth = _map->getWidth();
	_screenHeight = _map->getHeight();
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
void Camera::mouseClick(Action *action, State *state)
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
void Camera::mouseOver(Action *action, State *state)
{
	int posX = action->getXMouse();
	int posY = action->getYMouse();

	// handle RMB dragging
	if ((action->getDetails()->motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) && Options::getInt("battleScrollType") == SCROLL_RMB)
	{
		_RMBDragging = true;
		_scrollX = (int)(-(double)(_RMBClickX - posX) * (action->getXScale() * 2));
		_scrollY = (int)(-(double)(_RMBClickY - posY) * (action->getYScale() * 2));
		_RMBClickX = posX;
		_RMBClickY = posY;
	}
	// handle scrolling with mouse at edge of screen
	else
	{
		if (posX < (SCROLL_BORDER * action->getXScale()) && posX > 0)
		{
			_scrollX = Options::getInt("battleScrollSpeed");
			// if close to top or bottom, also scroll diagonally
			if (posY < (SCROLL_DIAGONAL_EDGE * action->getYScale()) && posY > 0)
			{
				_scrollY = Options::getInt("battleScrollSpeed");
			}
			else if (posY > (_screenHeight - SCROLL_DIAGONAL_EDGE) * action->getYScale())
			{
				_scrollY = -Options::getInt("battleScrollSpeed");
			}
		}
		else if (posX > (_screenWidth - SCROLL_BORDER) * action->getXScale())
		{
			_scrollX = -Options::getInt("battleScrollSpeed");
			// if close to top or bottom, also scroll diagonally
			if (posY < (SCROLL_DIAGONAL_EDGE * action->getYScale()) && posY > 0)
			{
				_scrollY = Options::getInt("battleScrollSpeed");
			}
			else if (posY > (_screenHeight - SCROLL_DIAGONAL_EDGE) * action->getYScale())
			{
				_scrollY = -Options::getInt("battleScrollSpeed");
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
			}
			else if (posX > (_screenWidth - SCROLL_DIAGONAL_EDGE) * action->getXScale())
			{
				_scrollX = -Options::getInt("battleScrollSpeed");
			}
		}
		else if (posY > (_screenHeight- SCROLL_BORDER) * action->getYScale())
		{
			_scrollY = -Options::getInt("battleScrollSpeed");
			// if close to left or right edge, also scroll diagonally
			if (posX < (SCROLL_DIAGONAL_EDGE * action->getXScale()) && posX > 0)
			{
				_scrollX = Options::getInt("battleScrollSpeed");
			}
			else if (posX > (_screenWidth - SCROLL_DIAGONAL_EDGE) * action->getXScale())
			{
				_scrollX = -Options::getInt("battleScrollSpeed");
			}
		}
		else if (posY && _scrollX == 0)
		{
			_scrollY = 0;
		}
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


/**
 * Handle scrolling.
 */
void Camera::scroll()
{
	_mapOffset.x += _scrollX;
	_mapOffset.y += _scrollY;

	convertScreenToMap((_screenWidth / 2), (_screenHeight / 2), &_centerX, &_centerY);

	// if center goes out of map bounds, hold the scrolling (may need further tweaking)
	if (_centerX > _mapWidth - 1 || _centerY > _mapLength - 1 || _centerX < 0 || _centerY < 0)
	{
		_mapOffset.x -= _scrollX;
		_mapOffset.y -= _scrollY;
	}

	if (_RMBDragging)
	{
		_RMBDragging = false;
		_scrollX = 0;
		_scrollY = 0;
	}

	_map->draw();
}



/**
 * Go one level up.
 */
void Camera::up()
{
	if (_mapOffset.z < _mapHeight - 1)
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
 * Set viewheight.
 * @param viewheight
 */
void Camera::setViewHeight(int viewheight)
{
	_mapOffset.z = viewheight;
	minMaxInt(&_mapOffset.z, 0, _mapHeight-1);
	_map->draw();
}


/**
 * Center map on a certain position.
 * @param mapPos Position to center on.
 */
void Camera::centerOnPosition(const Position &mapPos, bool redraw)
{
	Position screenPos;

	convertMapToScreen(mapPos, &screenPos);

	_mapOffset.x = -(screenPos.x - (_screenWidth / 2));
	_mapOffset.y = -(screenPos.y - (_visibleMapHeight / 2));

	convertScreenToMap((_screenWidth / 2), (_visibleMapHeight / 2), &_centerX, &_centerY);

	_mapOffset.z = mapPos.z;
	if (redraw) _map->draw();
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
	screenY += (-_spriteWidth/2) + (_mapOffset.z) * (_spriteWidth);

	// calculate the actual x/y pixelposition on a diamond shaped map
	// taking the view offset into account
	*mapY = - screenX + _mapOffset.x + 2 * screenY - 2 * _mapOffset.y;
	*mapX = screenY - _mapOffset.y - *mapY / 4 - (_spriteWidth/4);

	// to get the row&col itself, divide by the size of a tile
	*mapX /= (_spriteWidth / 4);
	*mapY /= _spriteWidth;

	minMaxInt(mapX, 0, _mapLength - 1);
	minMaxInt(mapY, 0, _mapWidth - 1);
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
 * Convert map coordinates X,Y,Z to screen positions X, Y.
 * @param mapPos X,Y,Z coordinates on the map.
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
int Camera::getViewHeight() const
{
	return _mapOffset.z;
}

/**
 * Get the X displayed map center
 * @return the X displayed map center
*/
int Camera::getCenterX() const
{
	return _centerX;
}

/**
 * Get the Y displayed map center
 * @return the Y displayed map center
*/
int Camera::getCenterY() const
{
	return _centerY;
}

Position Camera::getMapOffset()
{
	return _mapOffset;
}


}
