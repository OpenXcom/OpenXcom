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
 * @param spriteWidth Width of map sprite.
 * @param spriteHeight Height of map sprite.
 * @param mapsize_x Current map size in X axis.
 * @param mapsize_y Current map size in Y axis.
 * @param mapsize_z Current map size in Z axis.
 * @param map Pointer to map surface.
 * @param visibleMapHeight Current height the view is at.
 */
Camera::Camera(int spriteWidth, int spriteHeight, int mapsize_x, int mapsize_y, int mapsize_z, Map *map, int visibleMapHeight) : _scrollMouseTimer(0), _scrollKeyTimer(0), _spriteWidth(spriteWidth), _spriteHeight(spriteHeight), _mapsize_x(mapsize_x), _mapsize_y(mapsize_y), _mapsize_z(mapsize_z), _screenWidth(map->getWidth()), _screenHeight(map->getHeight()),
																																 _mapOffset(-250,250,0), _center(), _scrollMouseX(0), _scrollMouseY(0), _scrollKeyX(0), _scrollKeyY(0), _scrollTrigger(false), _visibleMapHeight(visibleMapHeight), _showAllLayers(false), _map(map)
{
}

/**
 * Deletes the Camera.
 */
Camera::~Camera()
{

}

/**
 * Sets the camera's scrolling timer.
 * @param mouse Pointer to mouse timer.
 * @param key Pointer to key timer.
 */
void Camera::setScrollTimer(Timer *mouse, Timer *key)
{
	_scrollMouseTimer = mouse;
	_scrollKeyTimer = key;
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
 * Handles camera mouse shortcuts.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Camera::mousePress(Action *action, State *)
{
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP)
	{
		up();
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN)
	{
		down();
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_LEFT && Options::getInt("battleScrollType") == SCROLL_TRIGGER)
	{
		_scrollTrigger = true;
		mouseOver(action, 0);
	}
}

/**
 * Handles camera mouse shortcuts.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Camera::mouseRelease(Action *action, State *)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && Options::getInt("battleScrollType") == SCROLL_TRIGGER)
	{
		_scrollMouseX = 0;
		_scrollMouseY = 0;
		_scrollMouseTimer->stop();
		_scrollTrigger = false;
		int posX = action->getXMouse();
		int posY = action->getYMouse();
		if ((posX < (SCROLL_BORDER * action->getXScale()) && posX > 0)
			|| (posX > (_screenWidth - SCROLL_BORDER) * action->getXScale())
			|| (posY < (SCROLL_BORDER * action->getYScale()) && posY > 0)
			|| (posY > (_screenHeight - SCROLL_BORDER) * action->getYScale()))
			// A cheap hack to avoid handling this event as a click
			// on the map when the mouse is on the scroll-border
			action->getDetails()->button.button = 0;
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

	if (Options::getInt("battleScrollType") == SCROLL_AUTO || _scrollTrigger)
	{
		int posX = action->getXMouse();
		int posY = action->getYMouse();
		int scrollSpeed = Options::getInt("battleScrollSpeed");

		//left scroll
		if (posX < (SCROLL_BORDER * action->getXScale()) && posX >= 0)
		{
			_scrollMouseX = scrollSpeed;
			// if close to top or bottom, also scroll diagonally
			//downleft
			if (posY < (SCROLL_DIAGONAL_EDGE * action->getYScale()) && posY >= 0)
			{
				_scrollMouseY = scrollSpeed/2;
			}
			//upleft
			else if (posY > (_screenHeight - SCROLL_DIAGONAL_EDGE) * action->getYScale())
			{
				_scrollMouseY = -scrollSpeed/2;
			}
		}
		//right scroll
		else if (posX > (_screenWidth - SCROLL_BORDER) * action->getXScale())
		{
			_scrollMouseX = -scrollSpeed;
			// if close to top or bottom, also scroll diagonally
			//downright
			if (posY <= (SCROLL_DIAGONAL_EDGE * action->getYScale()) && posY >= 0)
			{
				_scrollMouseY = scrollSpeed/2;
			}
			//upright
			else if (posY > (_screenHeight - SCROLL_DIAGONAL_EDGE) * action->getYScale())
			{
				_scrollMouseY = -scrollSpeed/2;
			}
		}
		else if (posX)
		{
			_scrollMouseX = 0;
		}

		//up
		if (posY < (SCROLL_BORDER * action->getYScale()) && posY >= 0)
		{
			_scrollMouseY = scrollSpeed;
			// if close to left or right edge, also scroll diagonally
			//up left
			if (posX < (SCROLL_DIAGONAL_EDGE * action->getXScale()) && posX > 0)
			{
				_scrollMouseX = scrollSpeed;
				_scrollMouseY /=2;
			}
			//up right
			else if (posX > (_screenWidth - SCROLL_DIAGONAL_EDGE) * action->getXScale())
			{
				_scrollMouseX = -scrollSpeed;
				_scrollMouseY /=2;
			}
		}
		//down
		else if (posY > (_screenHeight- SCROLL_BORDER) * action->getYScale())
		{
			_scrollMouseY = -scrollSpeed;
			// if close to left or right edge, also scroll diagonally
			//down left
			if (posX < (SCROLL_DIAGONAL_EDGE * action->getXScale()) && posX >= 0)
			{
				_scrollMouseX = scrollSpeed;
				_scrollMouseY /=2;
			}
			//down right
			else if (posX > (_screenWidth - SCROLL_DIAGONAL_EDGE) * action->getXScale())
			{
				_scrollMouseX = -scrollSpeed;
				_scrollMouseY /=2;
			}
		}
		else if (posY && _scrollMouseX == 0)
		{
			_scrollMouseY = 0;
		}

		if ((_scrollMouseX || _scrollMouseY) && !_scrollMouseTimer->isRunning() && !_scrollKeyTimer->isRunning())
		{
			_scrollMouseTimer->start();
		}
		else if ((!_scrollMouseX && !_scrollMouseY) && _scrollMouseTimer->isRunning())
		{
			_scrollMouseTimer->stop();
		}
	}
}

/**
 * Handles camera keyboard shortcuts.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Camera::keyboardPress(Action *action, State *)
{
	if (_map->getCursorType() == CT_NONE)
	{
		return;
	}

	int key = action->getDetails()->key.keysym.sym;
	int scrollSpeed = Options::getInt("battleScrollSpeed");
	if (key == Options::getInt("keyBattleLeft"))
	{
		_scrollKeyX = scrollSpeed;
	}
	else if (key == Options::getInt("keyBattleRight"))
	{
		_scrollKeyX = -scrollSpeed;
	}
	else if (key == Options::getInt("keyBattleUp"))
	{
		_scrollKeyY = scrollSpeed;
	}
	else if (key == Options::getInt("keyBattleDown"))
	{
		_scrollKeyY = -scrollSpeed;
	}

	if ((_scrollKeyX || _scrollKeyY) && !_scrollKeyTimer->isRunning() && !_scrollMouseTimer->isRunning())
	{
		_scrollKeyTimer->start();
	}
	else if ((!_scrollKeyX && !_scrollKeyY) && _scrollKeyTimer->isRunning())
	{
		_scrollKeyTimer->stop();
	}
}

/**
 * Handles camera keyboard shortcuts.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Camera::keyboardRelease(Action *action, State *)
{
	if (_map->getCursorType() == CT_NONE)
	{
		return;
	}

	int key = action->getDetails()->key.keysym.sym;
	if (key == Options::getInt("keyBattleLeft"))
	{
		_scrollKeyX = 0;
	}
	else if (key == Options::getInt("keyBattleRight"))
	{
		_scrollKeyX = 0;
	}
	else if (key == Options::getInt("keyBattleUp"))
	{
		_scrollKeyY = 0;
	}
	else if (key == Options::getInt("keyBattleDown"))
	{
		_scrollKeyY = 0;
	}

	if ((_scrollKeyX || _scrollKeyY) && !_scrollKeyTimer->isRunning() && !_scrollMouseTimer->isRunning())
	{
		_scrollKeyTimer->start();
	}
	else if ((!_scrollKeyX && !_scrollKeyY) && _scrollKeyTimer->isRunning())
	{
		_scrollKeyTimer->stop();
	}
}

/**
 * Handle mouse-scrolling.
 */
void Camera::scrollMouse()
{
	scrollXY(_scrollMouseX, _scrollMouseY, true);
}

/**
 * Handle keyboard-scrolling.
 */
void Camera::scrollKey()
{
	scrollXY(_scrollKeyX, _scrollKeyY, true);
}

/**
 * Handle scrolling with given deviation.
 * @param x X deviation.
 * @param y Y deviation.
 * @param redraw Redraw map or not.
 */
void Camera::scrollXY(int x, int y, bool redraw)
{
	_mapOffset.x += x;
	_mapOffset.y += y;

	do
	{
		convertScreenToMap((_screenWidth / 2), (_visibleMapHeight / 2), &_center.x, &_center.y);

		// Handling map bounds...
		// Ok, this is a prototype, it should be optimized.
		// Actually this should be calculated instead of slow-approximation.
		if (_center.x < 0)             { _mapOffset.x -= 2; _mapOffset.y -= 1; continue; }
		if (_center.x > _mapsize_x -1) { _mapOffset.x += 2; _mapOffset.y += 1; continue; }
		if (_center.y < 0)             { _mapOffset.x += 2; _mapOffset.y -= 1; continue; }
		if (_center.y > _mapsize_y -1) { _mapOffset.x -= 2; _mapOffset.y += 1; continue; }
		break;
	}
	while (true);

	_map->refreshSelectorPosition();
	if (redraw) _map->draw();
}


/**
 * Handle jumping with given deviation.
 * @param x X deviation.
 * @param y Y deviation.
 */
void Camera::jumpXY(int x, int y)
{
	_mapOffset.x += x;
	_mapOffset.y += y;
	convertScreenToMap((_screenWidth / 2), (_visibleMapHeight / 2), &_center.x, &_center.y);
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
 * @param viewlevel New view level
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

/**
 * Get the map offset
 * @return the map offset
 */
Position Camera::getMapOffset()
{
	return _mapOffset;
}

/**
 * Set the map offset
 * @param pos the map offset
 */
void Camera::setMapOffset(Position pos)
{
	_mapOffset = pos;
}

/**
 * Toggle showing all map layers.
 * @return New layer setting.
 */
int Camera::toggleShowAllLayers()
{
	_showAllLayers = !_showAllLayers;
	return _showAllLayers?2:1;
}

/**
 * Check if camera is showing all map layers.
 * @return Current layer setting.
 */
bool Camera::getShowAllLayers() const
{
	return _showAllLayers;
}

/**
 * Check if map coordinates X,Y,Z are on screen
 * @param mapPos Coordinates to check
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
