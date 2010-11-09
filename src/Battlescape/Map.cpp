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
#include "Map.h"
#include "UnitSprite.h"
#include "../Resource/TerrainObjectSet.h"
#include "../Resource/TerrainObject.h"
#include "../Engine/Action.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Timer.h"
#include "../Engine/Font.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "Position.h"
#include "../Resource/ResourcePack.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/GameTime.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Tile.h"
#include "../Ruleset/RuleTerrain.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleUfo.h"
#include "../Interface/Text.h"

#define SCROLL_AMOUNT 8
#define SCROLL_BORDER 5
#define DEFAULT_ANIM_SPEED 100

/*
  1) Map origin is left corner. 
  2) X axis goes downright. (width of the map)
  3) Y axis goes upright. (length of the map
  4) Z axis goes up (height of the map)

          y+
			/\
	    0,0/  \
		   \  /
		    \/
          x+  
*/

/**
 * Sets up a map with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Map::Map(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _MapOffsetX(-250), _MapOffsetY(250)
{

	// hardcoded atm to the resolution of original xcom sprites
	_TileSizeY = 32;
	_TileSizeX = 16;
	_TileSizeZ = 48;

	_scrollTimer = new Timer(50);
	_scrollTimer->onTimer((SurfaceHandler)&Map::scroll);
	_animTimer = new Timer(DEFAULT_ANIM_SPEED);
	_animTimer->onTimer((SurfaceHandler)&Map::animate);
	_animTimer->start();

	_animFrame = 0;
	_ScrollX = 0;
	_ScrollY = 0;
	_RMBDragging = false;

}

/**
 * Deletes the map.
 */
Map::~Map()
{
	delete _scrollTimer;
	delete _animTimer;
}

/**
 * Changes the pack for the map to get resources for rendering.
 * @param res Pointer to the resource pack.
 */
void Map::setResourcePack(ResourcePack *res)
{
	_res = res;
}

/**
 * Changes the saved game content for the map to render.
 * @param save Pointer to the saved game.
 */
void Map::setSavedGame(SavedBattleGame *save)
{
	_save = save;
}

/**
 * initializes stuff
 */
void Map::init()
{
	
}

/**
 * Keeps the animation timers running.
 */
void Map::think()
{
	_scrollTimer->think(0, this);
	_animTimer->think(0, this);
}

/**
 * Draws the whole map, part by part.
 */
void Map::draw()
{
	clear();
	drawTerrain();
}

/**
* Gets one single surface of the compiled list of surfaces.
* @param tobID the TerrainObjectID index of the object
* @param frame the animation frame (0-7)
* @return pointer to a surface
*/
Surface *Map::getSurface(TerrainObject *tob, int frame)
{
	// UFO doors are only animated when walked through
	if (tob->isUFODoor())
	{
		frame = 0;
	}

	return tob->getSprite(frame);
}

/**
* Draw the terrain.
*/
void Map::drawTerrain()
{
	int screenX, screenY, frameNumber = 0;
	TerrainObject *object = 0;
	Surface *frame;
	Tile *tile;
	int beginX = 0, endX = _save->getWidth() - 1;
    int beginY = 0, endY = _save->getLength() - 1;
    int beginZ = 0, endZ = _save->getViewHeight() - 1;
	UnitSprite *unitSprite = new UnitSprite(32, 40, 0, 0);
	unitSprite->setResourcePack(_res);
	unitSprite->setPalette(this->getPalette());

    for (int itZ = beginZ; itZ <= endZ; itZ++) 
	{
		// Draw remaining sprites
        for (int itX = beginX; itX <= endX; itX++) 
		{
            for (int itY = endY; itY >= beginY; itY--) 
			{
				screenX = _MapOffsetX + (_TileSizeY/2) * itY + (_TileSizeY/2) * itX;
				screenY = _MapOffsetY - itY * (_TileSizeX/2) + (_TileSizeX/2) * itX - itZ * (_TileSizeZ/2);

				// only render cells that are inside the viewport
				if (screenX > -_TileSizeY && screenX < getWidth() + _TileSizeY &&
					screenY > -_TileSizeZ && screenY < getHeight() + _TileSizeZ )
				{
					Position *position = new Position(itX, itY, itZ);

					// Draw floor
					tile = _save->getTile(itX,itY,itZ);
					if (tile)
					{
						for (int part=0;part<1;part++)
						{
							object = tile->getTerrainObject(part);
							if (object)
							{
								frame = getSurface(object, _animFrame);
								frame->setX(screenX);
								frame->setY(screenY - object->getYOffset());
								frame->blit(this);
							}
						}
					}

					// Draw cursor back
					if (_selectorX==itY && _selectorY == itX)
					{
						if ((_save->getViewHeight() - 1) == itZ)
						{
							frameNumber = 0;
						}
						else if ((_save->getViewHeight() - 1) > itZ)
						{
							frameNumber = 2;
						}
						frame = _res->getSurfaceSet("CURSOR.PCK")->getFrame(frameNumber);
						frame->setX(screenX);
						frame->setY(screenY);
						frame->blit(this);
					}


					// Draw walls and objects
					if (tile)
					{
						for (int part = 1; part < 4; part++)
						{
							object = tile->getTerrainObject(part);
							if (object)
							{
								frame = getSurface(object, _animFrame);
								frame->setX(screenX);
								frame->setY(screenY - object->getYOffset());
								frame->blit(this);
							}
						}
					}

					// Draw items

					// Draw soldier
					BattleUnit *unit = _save->selectUnit(position);
					if (unit != 0)
					{
						unitSprite->setBattleUnit(unit);
						unitSprite->setX(screenX);
						unitSprite->setY(screenY);
						unitSprite->draw();
						unitSprite->blit(this);
					}

					// Draw cursor front
					if (_selectorX == itY && _selectorY == itX)
					{
						if ((_save->getViewHeight() - 1) == itZ)
						{
							frameNumber = 3;
						}
						else if ((_save->getViewHeight() - 1) > itZ)
						{
							frameNumber = 5;
						}
						frame = _res->getSurfaceSet("CURSOR.PCK")->getFrame(frameNumber);
						frame->setX(screenX);
						frame->setY(screenY);
						frame->blit(this);
					}

					// Draw smoke/fire

					delete position;
				}
			}
		}
	}

	delete unitSprite;
}

/**
 * Blits the map onto another surface. 
 * @param surface Pointer to another surface.
 */
void Map::blit(Surface *surface)
{
	Surface::blit(surface);
}

/**
 * Ignores any mouse clicks that are outside the map.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Map::mousePress(Action *action, State *state)
{

}

/**
 * Ignores any mouse clicks that are outside the map.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Map::mouseRelease(Action *action, State *state)
{

}

/**
 * Ignores any mouse clicks that are outside the map
 * and handles globe rotation and zooming.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Map::mouseClick(Action *action, State *state)
{

}

/**
 * Handles map keyboard shortcuts.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Map::keyboardPress(Action *action, State *state)
{

}

/**
 * Handles mouse over events.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Map::mouseOver(Action *action, State *state)
{
	int posX = action->getDetails()->motion.x;
	int posY = action->getDetails()->motion.y;

	// handle RMB dragging
	if (action->getDetails()->motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		_RMBDragging = true;
		_ScrollX = (int)(-(double)(_RMBClickX - posX) * (action->getXScale() * 2));
		_ScrollY = (int)(-(double)(_RMBClickY - posY) * (action->getYScale() * 2));
		_RMBClickX = posX;
		_RMBClickY = posY;
	}
	else
	{
		// handle scrolling with mouse at edge of screen
		if (posX < SCROLL_BORDER && posX > 0)
		{
			_ScrollX = SCROLL_AMOUNT;
		}
		else if (posX > (getWidth() - SCROLL_BORDER) * action->getXScale())
		{
			_ScrollX = -SCROLL_AMOUNT;
		}
		else if (posX)
		{
			_ScrollX = 0;
		}

		if (posY < SCROLL_BORDER && posY > 0)
		{
			_ScrollY = SCROLL_AMOUNT;
		}
		else if (posY > (getHeight() - SCROLL_BORDER) * action->getYScale())
		{
			_ScrollY = -SCROLL_AMOUNT;
		}
		else if (posY)
		{
			_ScrollY = 0;
		}
	}

	if ((_ScrollX || _ScrollY) && !_scrollTimer->isRunning())
	{
		_scrollTimer->start();
	}
	else if ((!_ScrollX && !_ScrollY) && _scrollTimer->isRunning())
	{
		_scrollTimer->stop();
	}

	setSelectorPosition((int)((double)posX / action->getXScale()), (int)((double)posY / action->getYScale()));
}


/**
 * Sets the value to min if it is below min, sets value to max if above max.
 * @param value pointer to the value
 * @param minimum value
 * @param maximum value
 */
void Map::minMaxInt(int *value, const int minValue, const int maxValue)
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
 * Sets the selector to a certain tile on the map.
 * @param mx mouse x position
 * @param my mouse y position
 */
void Map::setSelectorPosition(int mx, int my)
{
	if (!mx && !my) return; // cursor is offscreen

	// add half a tileheight to the mouseposition per layer we are above the floor
    my += -_TileSizeZ + _save->getViewHeight() * (_TileSizeZ / 2);

	// calculate the actual x/y pixelposition on a diamond shaped map 
	// taking the view offset into account
    _selectorX = mx - _MapOffsetX - 2 * my + 2 * _MapOffsetY;
    _selectorY = my - _MapOffsetY + _selectorX / 4;

	// to get the row&col itself, divide by the size of a tile
    _selectorY /= (_TileSizeX / 2);
	_selectorX /= _TileSizeY;

	minMaxInt(&_selectorX, 0, _save->getWidth() - 1);
	minMaxInt(&_selectorY, 0, _save->getLength() - 1);
}


/**
 * Handle scrolling.
 */
void Map::scroll()
{
	_MapOffsetX += _ScrollX;
	_MapOffsetY += _ScrollY;

	// don't scroll outside the map
	minMaxInt(&_MapOffsetX, -(_save->getWidth() - 1) * _TileSizeY, 0);
	minMaxInt(&_MapOffsetY, -(_save->getLength() - 1) * (_TileSizeX / 2), _save->getLength() * (_TileSizeX / 2));

	if (_RMBDragging)
	{
		_RMBDragging = false;
		_ScrollX = 0;
		_ScrollY = 0;
	}

	draw();
}

/**
 * Handle animating tiles. 8 Frames per animation.
 */
void Map::animate()
{
	_animFrame = _animFrame == 7 ? 0 : _animFrame + 1;
	draw();
}

/**
 * Go one level up.
 */
void Map::up()
{
	_save->setRelativeViewHeight(+1);
	draw();
}

/**
 * Go one level down.
 */
void Map::down()
{
	_save->setRelativeViewHeight(-1);
	draw();
}


