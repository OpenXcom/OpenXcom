/*
 * Copyright 2010 Daniel Albano
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
#include "SurfaceSet.h"
#include "Base.h"
#include "BaseFacility.h"
#include "RuleBaseFacility.h"
#include "Craft.h"
#include "Palette.h"

#define MAX_BASES 8
#define MINI_SIZE 14

/**
 * Sets up a mini base view with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
MiniBaseView::MiniBaseView(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _bases(), _texture(0), _base(0), _hoverBase(0)
{
}

/**
 *
 */
MiniBaseView::~MiniBaseView()
{
}

/**
 * Changes the current list of bases to display.
 * @param bases Pointer to base list to display.
 */
void MiniBaseView::setBases(vector<Base*> *bases)
{
	_bases = bases;
	draw();
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
 * Returns the base the mouse cursor is currently over.
 * @return ID of the base.
 */
unsigned int MiniBaseView::getHoveredBase()
{
	return _hoverBase;
}

/**
 * Changes the base that is currently selected on
 * the mini base view.
 * @return ID of base.
 */
void MiniBaseView::setSelectedBase(unsigned int base)
{
	_base = base;
	draw();
}

/**
 * Draws the view of all the bases with facilities
 * in varying colors.
 */
void MiniBaseView::draw()
{
	clear();
	for (unsigned int i = 0; i < MAX_BASES; i++)
	{
		// Draw base squares
		if (i == _base)
		{
			SDL_Rect r;
			r.x = i * (MINI_SIZE + 2);
			r.y = 0;
			r.w = MINI_SIZE + 2;
			r.h = MINI_SIZE + 2;
			SDL_FillRect(this->getSurface(), &r, 1);
		}
		_texture->getFrame(41)->setX(i * (MINI_SIZE + 2));
		_texture->getFrame(41)->setY(0);
		_texture->getFrame(41)->blit(this);

		// Draw facilities
		if (i < _bases->size())
		{
			SDL_Rect r;
			lock();
			for (vector<BaseFacility*>::iterator f = _bases->at(i)->getFacilities()->begin(); f != _bases->at(i)->getFacilities()->end(); f++)
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
				SDL_FillRect(this->getSurface(), &r, Palette::blockOffset(pal)+3);
				r.x++;
				r.y++;
				r.w--;
				r.h--;
				SDL_FillRect(this->getSurface(), &r, Palette::blockOffset(pal)+5);
				r.x--;
				r.y--;
				SDL_FillRect(this->getSurface(), &r, Palette::blockOffset(pal)+2);
				r.x++;
				r.y++;
				r.w--;
				r.h--;
				SDL_FillRect(this->getSurface(), &r, Palette::blockOffset(pal)+3);
				r.x--;
				r.y--;
				setPixel(r.x, r.y, Palette::blockOffset(pal)+1);
			}
			unlock();
		}
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void MiniBaseView::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mousePress(ev, scale, state);
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void MiniBaseView::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseRelease(ev, scale, state);
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void MiniBaseView::mouseClick(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseClick(ev, scale, state);
	}
}

/**
 * Selects the base the mouse is over.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void MiniBaseView::mouseOver(SDL_Event *ev, int scale, State *state)
{
	double x = ev->button.x - _x * scale;
	_hoverBase = (int)floor(x / ((MINI_SIZE + 2) * scale));

	InteractiveSurface::mouseOver(ev, scale, state);
}
