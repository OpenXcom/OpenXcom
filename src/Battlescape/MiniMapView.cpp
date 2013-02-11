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
#include "../Interface/Cursor.h"
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
MiniMapView::MiniMapView(int w, int h, int x, int y, Game * game, Camera * camera, SavedBattleGame * battleGame) : InteractiveSurface(w, h, x, y), _game(game), _camera(camera), _battleGame(battleGame), _frame(0), isMouseScrolling(false), isMouseScrolled(false)
{
	_set = _game->getResourcePack()->getSurfaceSet("SCANG.DAT");
}

/**
 * Draw the minimap
 */
void MiniMapView::draw()
{
	int _startX = _camera->getCenterPosition().x - ((getWidth() / CELL_WIDTH) / 2);
	int _startY = _camera->getCenterPosition().y - ((getHeight() / CELL_HEIGHT) / 2);

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
	this->lock();
	for (int lvl = 0; lvl <= _camera->getCenterPosition().z; lvl++)
	{
		int py = _startY;
		for (int y = Surface::getY(); y < getHeight () + Surface::getY(); y += CELL_HEIGHT)
		{
			int px = _startX;
			for (int x = Surface::getX(); x < getWidth () + Surface::getX(); x += CELL_WIDTH)
			{
				MapData * data = 0;
				Tile * t = 0;
				Position p (px, py, lvl);
				t = _battleGame->getTile(p);
				if (!t)
				{
					px++;
					continue;
				}
				int tileShade = 16;
				if (t->isDiscovered(2))
				{
					tileShade = t->getShade();
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
						s->blitNShade(this, x, y, tileShade);
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
				// perhaps (at least one) item on this tile?
				if (t->isDiscovered(2) && !t->getInventory()->empty())
				{
					int frame = 9 + _frame;
					Surface * s = _set->getFrame(frame);
					s->blitNShade(this, x, y, 0);
				}

				px++;
			}
			py++;
		}
	}
	this->unlock();
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
int MiniMapView::up ()
{
	_camera->setViewHeight(_camera->getViewHeight()+1);
	_redraw = true;
	return _camera->getViewHeight();
}

/**
 * Decrement the displayed level
 */
int MiniMapView::down ()
{
	_camera->setViewHeight(_camera->getViewHeight()-1);
	_redraw = true;
	return _camera->getViewHeight();
}

/**
 * Handle press on the minimap. Enter to mouse-moving mode when right button is used
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
*/
void MiniMapView::mousePress(Action *action, State *state)
{
	InteractiveSurface::mousePress(action, state);

	if (-1 != _battleGame->getDragButton())
	{
		if (action->getDetails()->button.button == _battleGame->getDragButton())
		{
			isMouseScrolling = true;
			isMouseScrolled = false;
			SDL_GetMouseState(&xBeforeMouseScrolling, &yBeforeMouseScrolling);
			posBeforeMouseScrolling = _camera->getCenterPosition();
			mouseScrollX = 0; mouseScrollY = 0;
			totalMouseMoveX = 0; totalMouseMoveY = 0;
			mouseMovedOverThreshold = false;
			mouseScrollingStartTime = SDL_GetTicks();
		}
	}
}

/**
 * Handle click on the minimap. Will change the camera center to the clicked point
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
*/
void MiniMapView::mouseClick (Action *action, State *state)
{
	InteractiveSurface::mouseClick(action, state);

	// Right-button release: release mouse-scroll-mode
	if (isMouseScrolling)
	{
		// While scrolling, other buttons are ineffective
		if (action->getDetails()->button.button == _battleGame->getDragButton()) isMouseScrolling = false; else return;
		// Check if we have to revoke the scrolling, because it was too short in time, so it was a click
		if ((!mouseMovedOverThreshold) && (SDL_GetTicks() - mouseScrollingStartTime <= ((Uint32)_battleGame->getDragTimeTolerance())))
		{
			isMouseScrolled = false;
			_camera->centerOnPosition(posBeforeMouseScrolling);
			_redraw = true;
		}
		if (isMouseScrolled) return;
	}

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		int origX = action->getRelativeXMouse() / action->getXScale();
		int origY = action->getRelativeYMouse() / action->getYScale();
		// get offset (in cells) of the click relative to center of screen
		int xOff = (origX / CELL_WIDTH) - ((getWidth() / 2) / CELL_WIDTH);
		int yOff = (origY / CELL_HEIGHT) - ((getHeight() / 2) / CELL_HEIGHT);
		// center the camera on this new position
		int newX = _camera->getCenterPosition().x + xOff;
		int newY = _camera->getCenterPosition().y + yOff;
		_camera->centerOnPosition(Position(newX,newY,_camera->getViewHeight()));
		_redraw = true;
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
    // Closes the window on right-click.
		_game->popState();
	}
}

/**
 * Handle moving over the minimap.
 * Will change the camera center when mouse moved in mouse-moving mode
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
*/
void MiniMapView::mouseOver(Action *action, State *state)
{
	InteractiveSurface::mouseOver(action, state);

	if (isMouseScrolling && action->getDetails()->type == SDL_MOUSEMOTION)
	{
		isMouseScrolled = true;

		if (_battleGame->isDragInverted())
		{
			// Set the mouse cursor back
			SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
			SDL_WarpMouse(xBeforeMouseScrolling, yBeforeMouseScrolling);
			SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
		}

		// Check the threshold
		totalMouseMoveX += action->getDetails()->motion.xrel;
		totalMouseMoveY += action->getDetails()->motion.yrel;
		if (!mouseMovedOverThreshold)
			mouseMovedOverThreshold = ((std::abs(totalMouseMoveX) > _battleGame->getDragPixelTolerance()) || (std::abs(totalMouseMoveY) > _battleGame->getDragPixelTolerance()));

		// Calculate the move
		int newX;
		int newY;
		if (_battleGame->isDragInverted())
		{
			mouseScrollX += action->getDetails()->motion.xrel;
			mouseScrollY += action->getDetails()->motion.yrel;
			newX = posBeforeMouseScrolling.x + mouseScrollX / 4;
			newY = posBeforeMouseScrolling.y + mouseScrollY / 4;

			// Keep the limits...
			if (newX < -1 || _camera->getMapLength() < newX)
			{
				mouseScrollX -= action->getDetails()->motion.xrel;
				newX = posBeforeMouseScrolling.x + mouseScrollX / 4;
			}
			if (newY < -1 || _camera->getMapWidth() < newY)
			{
				mouseScrollY -= action->getDetails()->motion.yrel;
				newY = posBeforeMouseScrolling.y + mouseScrollY / 4;
			}
		}
		else
		{
			newX = posBeforeMouseScrolling.x - (int)((double)totalMouseMoveX / action->getXScale()) / 4;
			newY = posBeforeMouseScrolling.y - (int)((double)totalMouseMoveY / action->getYScale()) / 4;

			// Keep the limits...
			if (newX < -1) newX = -1;
			else if (_camera->getMapLength() < newX) newX = _camera->getMapLength();
			if (newY < -1) newY = -1;
			else if (_camera->getMapWidth() < newY) newY = _camera->getMapWidth();
		}

		// Scrolling
		_camera->centerOnPosition(Position(newX,newY,_camera->getViewHeight()));
		_redraw = true;

		if (_battleGame->isDragInverted())
		{
			// We don't want to look the mouse-cursor jumping :)
			action->getDetails()->motion.x=xBeforeMouseScrolling; action->getDetails()->motion.y=yBeforeMouseScrolling;
			_game->getCursor()->handle(action);
		}
	}
}

/**
 * Handle moving in to the minimap.
 * Stops the mouse-scrolling mode, if its left on
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
*/
void MiniMapView::mouseIn(Action *action, State *state)
{
	InteractiveSurface::mouseIn(action, state);

	isMouseScrolling = false;
	_buttonsPressed[SDL_BUTTON_RIGHT] = false;
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

}
