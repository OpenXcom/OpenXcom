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
#include "MiniMapView.h"
#include "../Savegame/Tile.h"
#include "Map.h"
#include "Camera.h"
#include "../Engine/Action.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/Game.h"
#include "../Engine/SurfaceSet.h"
#include "../Resource/ResourcePack.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/Armor.h"
#include <sstream>

namespace OpenXcom
{
const int CELL_WIDTH = 4;
const int CELL_HEIGHT = 4;
const int MAX_LEVEL = 3;
const int MAX_FRAME = 2;

/**
 * Initializes all the elements in the MiniMapView.
 * @param w the MiniMapView width
 * @param h the MiniMapView height
 * @param x the MiniMapView x origin
 * @param y the MiniMapView y origin
 * @param game Pointer to the core game.
 * @param map The Battlescape map
 * @param battleGame Pointer to the SavedBattleGame
*/
MiniMapView::MiniMapView(int w, int h, int x, int y, Game * game, Map * map, SavedBattleGame * battleGame) : InteractiveSurface(w, h, x, y), _game(game), _map(map), _startX(0),  _startY(0), _lvl(_map->getCamera()->getViewHeight()), _battleGame(battleGame), _frame(0)
{
	_startX = _map->getCamera()->getCenterX() - ((getWidth() / CELL_WIDTH) / 2);
	_startY = _map->getCamera()->getCenterY() - ((getHeight() / CELL_HEIGHT) / 2);
	_set = _game->getResourcePack()->getSurfaceSet("SCANG.DAT");
}

/**
 * Draw the minimap
 */
void MiniMapView::draw()
{
	InteractiveSurface::draw();
	if(!_set)
	{
		return;
	}
	SDL_Rect current;
	current.x = current.y = 0;
	current.w = getWidth ();
	current.h = getHeight ();
	drawRect(&current, 0);
	for (int lvl = 0; lvl <= _lvl; lvl++)
	{
		int py = _startY;
		for (int y = 0; y < getHeight (); y += CELL_HEIGHT)
		{
			int px = _startX;
			for (int x = 0; x < getWidth (); x += CELL_WIDTH)
			{
				MapData * data = 0;
				Tile * t = 0;
				Position p (px, py, lvl);
				t = _battleGame->getTile(p);
				if (!t || !t->isDiscovered(2))
				{
					px++;
					continue;
				}
				for(int i = 0; i < 4; i++)
				{
					data = t->getMapData(i);

					Surface * s = 0;
					if(data && data->getMiniMapIndex())
					{
						s = _set->getFrame (data->getMiniMapIndex()+35);
					}
					if(s)
					{
						s->blitNShade(this, x, y, t->getShade());
					}
				}

				// alive units
				if (t->getUnit() && t->getUnit()->getVisible())
				{
					int frame = t->getUnit()->getMiniMapSpriteIndex();
					int size = t->getUnit()->getArmor()->getSize();
					frame += (t->getPosition().y - t->getUnit()->getPosition().y) * size;
					frame += t->getPosition().x - t->getUnit()->getPosition().x;
					frame += _frame * size * size;
					Surface * s = _set->getFrame(frame);
					s->blitNShade(this, x, y, 0);
				}
				else
				{
					// or perhaps (at least one) corpse on this tile?
					for (std::vector<BattleItem*>::iterator it = t->getInventory()->begin(); it != t->getInventory()->end(); ++it)
					{
						if ((*it)->getUnit())
						{
							int frame = t->getUnit()->getMiniMapSpriteIndex();
							int size = t->getUnit()->getArmor()->getSize();
							frame += (t->getPosition().y - t->getUnit()->getPosition().y) * size;
							frame += t->getPosition().x - t->getUnit()->getPosition().x;
							frame += _frame * size * size;
							Surface * s = _set->getFrame(frame);
							s->blitNShade(this, x, y, 0);
						}
					}
				}

				px++;
			}
			py++;
		}
	}
	int centerX = getWidth() / 2;
	int centerY = getHeight() / 2;
	Uint8 color = 1 + _frame * 3;
	int xOffset = CELL_WIDTH / 2;
	int yOffset = CELL_HEIGHT / 2;
	drawLine(centerX - CELL_WIDTH, centerY - CELL_HEIGHT,
		 centerX - xOffset, centerY - yOffset,
		 color); // top left
	drawLine(centerX + xOffset, centerY - yOffset,
		 centerX + CELL_WIDTH, centerY - CELL_HEIGHT,
		 color); // top right
	drawLine(centerX - CELL_WIDTH, centerY + CELL_HEIGHT,
		 centerX - xOffset, centerY + yOffset,
		 color); // bottom left
	drawLine(centerX + CELL_WIDTH, centerY + CELL_HEIGHT,
		 centerX + xOffset, centerY + yOffset,
		 color); //bottom right
}

/**
 * Increment the displayed level
 */
void MiniMapView::up ()
{
	_lvl++;
	if (_lvl > MAX_LEVEL)
	{
		_lvl = 0;
	}
	_redraw = true;
}

/**
 * Decrement the displayed level
 */
void MiniMapView::down ()
{
	_lvl--;
	if (_lvl < 0)
	{
		_lvl = MAX_LEVEL;
	}
	_redraw = true;
}

/**
 * Handle click on the minimap. Will change the minimap center to the clicked point
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
*/
void MiniMapView::mouseClick (Action *action, State *state)
{
	InteractiveSurface::mouseClick(action, state);
	int origX = action->getRelativeXMouse() / action->getXScale();
	int origY = action->getRelativeYMouse() / action->getYScale();
	_startY += (origY / CELL_HEIGHT) - ((getHeight () / 2) / CELL_HEIGHT);
	_startX += (origX / CELL_HEIGHT) - ((getWidth () / 2) / CELL_HEIGHT);
	_redraw = true;
}

/**
 * Get the displayed level
 * @return the displayed level
*/
int MiniMapView::getDisplayedLevel ()
{
	return _lvl;
}

/**
 * Change the displayed level
 * @param level the new displayed level
*/
void MiniMapView::setDisplayedLevel (int level)
{
	_lvl = level;
}

/**
 * Update minimap animation
*/
void MiniMapView::animate()
{
	_frame++;
	if(_frame > MAX_FRAME)
	{
		_frame = 0;
	}
	_redraw = true;
}

/**
 * Get the minimap center position
 * @return the minimap center
*/
Position MiniMapView::getCenter()
{
	Position p(_startX + ((getWidth () / 2) / CELL_HEIGHT),
		   _startY + ((getHeight () / 2) / CELL_HEIGHT),
		   _lvl);
	return p;
}
}
