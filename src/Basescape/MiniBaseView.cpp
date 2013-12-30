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
#include "MiniBaseView.h"
#include <cmath>
#include "../Engine/SurfaceSet.h"
#include "../Engine/Action.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Savegame/Craft.h"
#include "../Engine/Palette.h"

namespace OpenXcom
{

/**
 * Sets up a mini base view with the specified size and position.
 * @param bases Pointer to base list to display.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
MiniBaseView::MiniBaseView(std::vector<Base*> *bases, NewBaseSelectedHandler newBaseSelectedHandler, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _bases(bases), _newBaseSelectedHandler(newBaseSelectedHandler), _texture(0), _base(0)
{
}

/**
 *
 */
MiniBaseView::~MiniBaseView()
{
}

/**
 * Changes the texture to use for drawing
 * the various base elements.
 * @param texture Pointer to SurfaceSet to use.
 */
void MiniBaseView::setTexture(SurfaceSet *texture)
{
	_texture = texture;
}

/**
 * Changes the base that is currently selected on
 * the mini base view.
 * @param base the desired base.
 */
void MiniBaseView::setSelectedBase(Base *base)
{
	for (size_t i = 0; i < _bases->size(); ++i)
	{
		if (_bases->at(i) == base)
		{
			_base = i;
			break;
		}
	}
	_redraw = true;
}

/**
 * Draws the view of all the bases with facilities
 * in varying colors.
 */
void MiniBaseView::draw()
{
	Surface::draw();
	for (size_t i = 0; i < MAX_BASES; ++i)
	{
		// Draw base squares
		if (i == _base)
		{
			SDL_Rect r;
			r.x = i * (MINI_SIZE + 2);
			r.y = 0;
			r.w = MINI_SIZE + 2;
			r.h = MINI_SIZE + 2;
			drawRect(&r, 1);
		}
		_texture->getFrame(41)->setX(i * (MINI_SIZE + 2));
		_texture->getFrame(41)->setY(0);
		_texture->getFrame(41)->blit(this);

		// Draw facilities
		if (i < _bases->size())
		{
			SDL_Rect r;
			lock();
			for (std::vector<BaseFacility*>::iterator f = _bases->at(i)->getFacilities()->begin(); f != _bases->at(i)->getFacilities()->end(); ++f)
			{
				int pal;
				if ((*f)->getBuildTime() == 0)
					pal = 3;
				else
					pal = 2;

				r.x = i * (MINI_SIZE + 2) + 2 + (*f)->getX() * 2;
				r.y = 2 + (*f)->getY() * 2;
				r.w = (*f)->getRules()->getSize() * 2;
				r.h = (*f)->getRules()->getSize() * 2;
				drawRect(&r, Palette::blockOffset(pal)+3);
				r.x++;
				r.y++;
				r.w--;
				r.h--;
				drawRect(&r, Palette::blockOffset(pal)+5);
				r.x--;
				r.y--;
				drawRect(&r, Palette::blockOffset(pal)+2);
				r.x++;
				r.y++;
				r.w--;
				r.h--;
				drawRect(&r, Palette::blockOffset(pal)+3);
				r.x--;
				r.y--;
				setPixel(r.x, r.y, Palette::blockOffset(pal)+1);
			}
			unlock();
		}
	}
}

/**
 * Handles a mouse click event.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void MiniBaseView::mouseClick(Action *action, State *state)
{
	if (SDL_BUTTON_LEFT == action->getDetails()->button.button)
	{
		size_t base = (int)floor(action->getRelativeXMouse() / ((MINI_SIZE + 2) * action->getXScale()));
		if (base < _bases->size())
		{
			_base = base;
			_redraw = true;
			(state->*_newBaseSelectedHandler)(_bases->at(base));
		}
	}
	InteractiveSurface::mouseClick(action, state);
}

}
