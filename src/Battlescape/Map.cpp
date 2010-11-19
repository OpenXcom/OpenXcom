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
#include "Position.h"
#include "Pathfinding.h"
#include "../Resource/TerrainObjectSet.h"
#include "../Resource/TerrainObject.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Action.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Timer.h"
#include "../Engine/Font.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Game.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/GameTime.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleSoldier.h"
#include "../Ruleset/RuleTerrain.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleUfo.h"
#include "../Interface/Text.h"
#include "../Interface/Cursor.h"

#define SCROLL_AMOUNT 8
#define SCROLL_BORDER 5
#define SCROLL_DIAGONAL_EDGE 60
#define DEFAULT_ANIM_SPEED 100
#define DEFAULT_WALK_SPEED 50
#define DEFAULT_BULLET_SPEED 20
#define RMB_SCROLL false

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
Map::Map(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _MapOffsetX(-250), _MapOffsetY(250), _viewHeight(0), _hideCursor(false)
{

	_scrollTimer = new Timer(50);
	_scrollTimer->onTimer((SurfaceHandler)&Map::scroll);

	_animTimer = new Timer(DEFAULT_ANIM_SPEED);
	_animTimer->onTimer((SurfaceHandler)&Map::animate);
	_animTimer->start();

	_walkingTimer = new Timer(DEFAULT_WALK_SPEED);
	_walkingTimer->onTimer((SurfaceHandler)&Map::moveUnit);
	_walkingTimer->start();

	_bulletTimer = new Timer(DEFAULT_BULLET_SPEED);
	_bulletTimer->onTimer((SurfaceHandler)&Map::moveBullet);
	_bulletTimer->start();

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
	delete _walkingTimer;
	delete _bulletTimer;
}

/**
 * Changes the pack for the map to get resources for rendering.
 * @param res Pointer to the resource pack.
 */
void Map::setResourcePack(ResourcePack *res)
{
	_res = res;

	_SpriteWidth = res->getSurfaceSet("BLANKS.PCK")->getFrame(0)->getWidth();
	_SpriteHeight = res->getSurfaceSet("BLANKS.PCK")->getFrame(0)->getHeight();
}

/**
 * Changes the saved game content for the map to render.
 * @param save Pointer to the saved game.
 * @param game Pointer to the Game.
 */
void Map::setSavedGame(SavedBattleGame *save, Game *game)
{
	_save = save;
	_game = game;
}

/**
 * initializes stuff
 */
void Map::init()
{
	// load the tiny arrow into a surface
	int f = Palette::blockOffset(1)+1; // yellow
	int b = 15; // black
	int pixels[81] = { 0, 0, b, b, b, b, b, 0, 0, 
					   0, 0, b, f, f, f, b, 0, 0,
				       0, 0, b, f, f, f, b, 0, 0, 
					   b, b, b, f, f, f, b, b, b,
					   b, f, f, f, f, f, f, f, b,
					   0, b, f, f, f, f, f, b, 0,
					   0, 0, b, f, f, f, b, 0, 0,
					   0, 0, 0, b, f, b, 0, 0, 0,
					   0, 0, 0, 0, b, 0, 0, 0, 0 };

	_arrow = new Surface(9, 9);
	_arrow->setPalette(this->getPalette());
	_arrow->lock();
	for (int y = 0; y < 9;y++)
		for (int x = 0; x < 9; x++)
			_arrow->setPixel(x, y, pixels[x+(y*9)]);
	_arrow->unlock();

}

/**
 * Keeps the animation timers running.
 */
void Map::think()
{
	_scrollTimer->think(0, this);
	_animTimer->think(0, this);
	_walkingTimer->think(0, this);
	_bulletTimer->think(0, this);
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
	int frameNumber = 0;
	TerrainObject *object = 0;
	Surface *frame;
	Tile *tile;
	int beginX = 0, endX = _save->getWidth() - 1;
    int beginY = 0, endY = _save->getLength() - 1;
    int beginZ = 0, endZ = _viewHeight;
	UnitSprite *unitSprite = new UnitSprite(32, 40, 0, 0);
	unitSprite->setResourcePack(_res);
	unitSprite->setPalette(this->getPalette());
	Position mapPosition, screenPosition;

    for (int itZ = beginZ; itZ <= endZ; itZ++) 
	{
		// Draw remaining sprites
        for (int itX = beginX; itX <= endX; itX++) 
		{
            for (int itY = endY; itY >= beginY; itY--) 
			{
				mapPosition = Position(itX, itY, itZ);
				convertMapToScreen(mapPosition, &screenPosition);
				screenPosition.x = _MapOffsetX + screenPosition.x;
				screenPosition.y = _MapOffsetY + screenPosition.y;

				// only render cells that are inside the viewport
				if (screenPosition.x > -_SpriteWidth && screenPosition.x < getWidth() + _SpriteWidth &&
					screenPosition.y > -_SpriteHeight && screenPosition.y < getHeight() + _SpriteHeight )
				{
					BattleUnit *unit = _save->selectUnit(mapPosition);

					// Draw floor
					tile = _save->getTile(mapPosition);
					if (tile)
					{
						for (int part=0;part<1;part++)
						{
							object = tile->getTerrainObject(part);
							if (object)
							{
								frame = getSurface(object, _animFrame);
								frame->setX(screenPosition.x);
								frame->setY(screenPosition.y - object->getYOffset());
								frame->blit(this);
							}
						}
					}

					// Draw cursor back
					if (_selectorX == itY && _selectorY == itX && !_hideCursor)
					{
						if (_viewHeight == itZ)
						{
							if (unit)
								frameNumber = 1; // yellow box
							else
								frameNumber = 0; // red box
						}
						else if (_viewHeight > itZ)
						{
							frameNumber = 2; // blue box
						}
						frame = _res->getSurfaceSet("CURSOR.PCK")->getFrame(frameNumber);
						frame->setX(screenPosition.x);
						frame->setY(screenPosition.y);
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
								frame->setX(screenPosition.x);
								frame->setY(screenPosition.y - object->getYOffset());
								frame->blit(this);
							}
						}
					}

					// Draw items

					// Draw soldier
					if (unit)
					{
						Position offset;
						calculateWalkingOffset(unit, &offset);
						unitSprite->setBattleUnit(unit);
						unitSprite->setX(screenPosition.x + offset.x);
						unitSprite->setY(screenPosition.y + offset.y);
						unitSprite->draw();
						unitSprite->blit(this);
						if (unit == (BattleUnit*)_save->getSelectedSoldier() && !_hideCursor)
						{
							drawArrow(screenPosition + offset);
						}
					}
					// if we can see through the floor, draw the soldier below it if it is on stairs
					if (itZ > 0 && tile->hasNoFloor()) 
					{
						unit = _save->selectUnit(mapPosition + Position(0, 0, -1));
						tile = _save->getTile(mapPosition + Position(0, 0, -1));
						if (unit && tile->getTerrainLevel() < 0)
						{
							Position offset;
							calculateWalkingOffset(unit, &offset);
							offset.y += 24;
							unitSprite->setBattleUnit(unit);
							unitSprite->setX(screenPosition.x + offset.x);
							unitSprite->setY(screenPosition.y + offset.y);
							unitSprite->draw();
							unitSprite->blit(this);
							if (unit == (BattleUnit*)_save->getSelectedSoldier() && !_hideCursor)
							{
								drawArrow(screenPosition + offset);
							}
						}
					}


					// Draw cursor front
					if (_selectorX == itY && _selectorY == itX && !_hideCursor)
					{
						if (_viewHeight == itZ)
						{
							if (unit)
								frameNumber = 4; // yellow box
							else
								frameNumber = 3; // red box
						}
						else if (_viewHeight > itZ)
						{
							frameNumber = 5; // blue box
						}
						frame = _res->getSurfaceSet("CURSOR.PCK")->getFrame(frameNumber);
						frame->setX(screenPosition.x);
						frame->setY(screenPosition.y);
						frame->blit(this);
					}

					// Draw smoke/fire

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
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
/*void Map::mouseClick(Action *action, State *state)
{
	int test=0;
}*/

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
	if ((action->getDetails()->motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) && RMB_SCROLL)
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
		if (posX < (SCROLL_BORDER * action->getXScale()) && posX > 0)
		{
			_ScrollX = SCROLL_AMOUNT;
			// if close to top or bottom, also scroll diagonally
			if (posY < (SCROLL_DIAGONAL_EDGE * action->getYScale()) && posY > 0)
			{
				_ScrollY = SCROLL_AMOUNT;
			}
			else if (posY > (getHeight() - SCROLL_DIAGONAL_EDGE) * action->getYScale())
			{
				_ScrollY = -SCROLL_AMOUNT;
			}
		}
		else if (posX > (getWidth() - SCROLL_BORDER) * action->getXScale())
		{
			_ScrollX = -SCROLL_AMOUNT;
			// if close to top or bottom, also scroll diagonally
			if (posY < (SCROLL_DIAGONAL_EDGE * action->getYScale()) && posY > 0)
			{
				_ScrollY = SCROLL_AMOUNT;
			}
			else if (posY > (getHeight() - SCROLL_DIAGONAL_EDGE) * action->getYScale())
			{
				_ScrollY = -SCROLL_AMOUNT;
			}
		}
		else if (posX)
		{
			_ScrollX = 0;
		}

		if (posY < (SCROLL_BORDER * action->getYScale()) && posY > 0)
		{
			_ScrollY = SCROLL_AMOUNT;
			// if close to left or right edge, also scroll diagonally
			if (posX < (SCROLL_DIAGONAL_EDGE * action->getXScale()) && posX > 0)
			{
				_ScrollX = SCROLL_AMOUNT;
			}
			else if (posX > (getWidth() - SCROLL_DIAGONAL_EDGE) * action->getXScale())
			{
				_ScrollX = -SCROLL_AMOUNT;
			}
		}
		else if (posY > (getHeight() - SCROLL_BORDER) * action->getYScale())
		{
			_ScrollY = -SCROLL_AMOUNT;
			// if close to left or right edge, also scroll diagonally
			if (posX < (SCROLL_DIAGONAL_EDGE * action->getXScale()) && posX > 0)
			{
				_ScrollX = SCROLL_AMOUNT;
			}
			else if (posX > (getWidth() - SCROLL_DIAGONAL_EDGE) * action->getXScale())
			{
				_ScrollX = -SCROLL_AMOUNT;
			}
		}
		else if (posY && _ScrollX == 0)
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
    my += -_SpriteHeight + (_viewHeight + 1) * (_SpriteHeight / 2);

	// calculate the actual x/y pixelposition on a diamond shaped map 
	// taking the view offset into account
    _selectorX = mx - _MapOffsetX - 2 * my + 2 * _MapOffsetY;
    _selectorY = my - _MapOffsetY + _selectorX / 4;

	// to get the row&col itself, divide by the size of a tile
    _selectorY /= (_SpriteWidth/4);
	_selectorX /= _SpriteWidth;

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
	minMaxInt(&_MapOffsetX, -(_save->getWidth() - 1) * _SpriteWidth, 0);
	minMaxInt(&_MapOffsetY, -(_save->getLength() - 1) * (_SpriteWidth/4), _save->getLength() * (_SpriteWidth/4));

	if (_RMBDragging)
	{
		_RMBDragging = false;
		_ScrollX = 0;
		_ScrollY = 0;
	}

	draw();
}

/**
 * Handle animating tiles/units/bullets. 8 Frames per animation.
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
	_viewHeight++;
	minMaxInt(&_viewHeight, 0, _save->getHeight()-1);
	draw();
}

/**
 * Go one level down.
 */
void Map::down()
{
	_viewHeight--;
	minMaxInt(&_viewHeight, 0, _save->getHeight()-1);
	draw();
}

/**
 * Center map on a certain position.
 * @param mapPos Position to center on.
 */
void Map::centerOnPosition(const Position &mapPos)
{
	Position screenPos;

	convertMapToScreen(mapPos, &screenPos);

	_MapOffsetX = -(screenPos.x - (getWidth() / 2));
	_MapOffsetY = -(screenPos.y - (BUTTONS_AREA / 2));

	_viewHeight = mapPos.z;
}

/**
 * Convert map coordinates X,Y,Z to screen positions X, Y.
 * @param mapPos X,Y,Z coordinates on the map.
 * @param screenPos to screen position.
 */
void Map::convertMapToScreen(const Position &mapPos, Position *screenPos)
{
	screenPos->z = 0; // not used
	screenPos->x = mapPos.x * (_SpriteWidth / 2) + mapPos.y * (_SpriteWidth / 2);
	screenPos->y = mapPos.x * (_SpriteWidth / 4) - mapPos.y * (_SpriteWidth / 4) - mapPos.z * ((_SpriteHeight + _SpriteWidth / 4) / 2);
}


/**
 * Draws the small arrow above the selected soldier.
 * @param screenPos
 */
void Map::drawArrow(const Position &screenPos)
{
	_arrow->setX(screenPos.x + (_SpriteWidth / 2) - (_arrow->getWidth() / 2));
	_arrow->setY(screenPos.y - _arrow->getHeight() + _animFrame);
	_arrow->blit(this);

}

/**
 * Draws the small arrow above the selected soldier.
 * @param pos pointer to a position
 */
void Map::getSelectorPosition(Position *pos)
{
	// don't know why X and Y are inverted here...
	pos->x = _selectorY;
	pos->y = _selectorX;
	pos->z = _viewHeight;
}

/**
 * Calculate the offset of a soldier, when it is walking in the middle of 2 tiles.
 * @param unit pointer to BattleUnit
 * @param offset pointer to the offset to return the calculation.
 */
void Map::calculateWalkingOffset(BattleUnit *unit, Position *offset)
{
	int offsetX[8] = { 1, 2, 1, 0, -1, -2, -1, 0 };
	int offsetY[8] = { 1, 0, -1, -2, -1, 0, 1, 2 };
	int phase = unit->getWalkingPhase();
	int dir = unit->getDirection();

	if (phase)
	{
		if (phase < 4)
		{
			offset->x = phase * 2 * offsetX[dir];
			offset->y = - phase * offsetY[dir];
		}
		else
		{
			offset->x = (phase - 8) * 2 * offsetX[dir];
			offset->y = - (phase - 8) * offsetY[dir];
		}

	}

	// If we are walking in between tiles, interpolate it's terrain level.
	if (phase)
	{
		if (phase < 4)
		{
			int fromLevel = _save->getTile(unit->getPosition())->getTerrainLevel();
			int toLevel = _save->getTile(unit->getDestination())->getTerrainLevel();
			if (unit->getPosition().z > unit->getDestination().z)
			{
				// going down a level, so toLevel 0 becomes +24, -8 becomes  16
				toLevel += 24*(unit->getPosition().z - unit->getDestination().z);
			}else if (unit->getPosition().z < unit->getDestination().z)
			{
				// going up a level, so toLevel 0 becomes -24, -8 becomes -16
				toLevel = -24*(unit->getDestination().z - unit->getPosition().z) + abs(toLevel);
			}
			offset->y += ((fromLevel * (8 - phase)) / 8) + ((toLevel * (phase)) / 8);
		}
		else
		{
			// from phase 4 onwards the unit behind the scenes already is on the destination tile
			// we have to get it's last position to calculate the correct offset
			int fromLevel = _save->getTile(unit->getLastPosition())->getTerrainLevel();
			int toLevel = _save->getTile(unit->getDestination())->getTerrainLevel();
			if (unit->getLastPosition().z > unit->getDestination().z)
			{
				// going down a level, so fromLevel 0 becomes -24, -8 becomes -32
				fromLevel -= 24*(unit->getLastPosition().z - unit->getDestination().z);
			}else if (unit->getLastPosition().z < unit->getDestination().z)
			{
				// going up a level, so fromLevel 0 becomes +24, -8 becomes 16
				fromLevel = -24*(unit->getDestination().z - unit->getLastPosition().z) + abs(fromLevel);
			}
			offset->y += ((fromLevel * (8 - phase)) / 8) + ((toLevel * (phase)) / 8);
		}
	}
	else
	{
		offset->y += _save->getTile(unit->getPosition())->getTerrainLevel();
	}

}

/**
 * Animate walking unit.
 */
void Map::moveUnit()
{
	int tu = 0;

	if (_save->getSelectedSoldier()->getStatus() == STATUS_WALKING)
	{
		_save->getSelectedSoldier()->keepWalking();
	}

	if (_save->getSelectedSoldier()->getStatus() == STATUS_TURNING)
	{
		_save->getSelectedSoldier()->turn();
	}

	if (_save->getSelectedSoldier()->getStatus() == STATUS_STANDING)
	{
		int dir = _save->getPathfinding()->dequeuePath();
		if (dir != -1)
		{
			Position destination;
			tu = _save->getPathfinding()->getTUCost(_save->getSelectedSoldier()->getPosition(), dir, &destination, (BattleUnit*)_save->getSelectedSoldier());
			_save->getSelectedSoldier()->startWalking(dir, destination);
			_hideCursor = true; // hide cursor while walking
			_game->getCursor()->setVisible(false);
		}
		else if (_hideCursor)
		{
			_viewHeight = _save->getSelectedSoldier()->getPosition().z;
			_hideCursor = false; // show cursor again
			_game->getCursor()->setVisible(true);
		}
	}

	draw();
}

/**
 * Animate flying bullet.
 */
void Map::moveBullet()
{

}