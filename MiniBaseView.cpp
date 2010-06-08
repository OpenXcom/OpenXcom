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

/**
 * Sets up a mini base view with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
MiniBaseView::MiniBaseView(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _bases(), _texture(0), _selBase(0)
{
}

/**
 *
 */
MiniBaseView::~MiniBaseView()
{
}

void MiniBaseView::setBases(vector<Base*> *bases)
{
	_bases = bases;

	draw();
}

void MiniBaseView::setTexture(SurfaceSet *texture)
{
	_texture = texture;
}

void MiniBaseView::draw()
{
	for (int i = 0; i < MAX_BASES; i++)
	{
		// Draw base squares
		if (i == _selBase)
		{
			SDL_Rect r;
			r.x = i * (MINI_SIZE + 2);
			r.y = 0;
			r.w = MINI_SIZE + 2;
			r.h = MINI_SIZE + 2;
			SDL_FillRect(this->getSurface(), &r, 1);
		}
		_texture->getSurface()->setX(i * (MINI_SIZE + 2));
		_texture->getSurface()->setY(0);
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
				setPixel(r.x, r.y, Palette::blockOffset(3)+1);
			}
			unlock();
		}
	}
}

int MiniBaseView::getSelectedBase()
{
	return _selBase;
}

void MiniBaseView::handle(SDL_Event *ev, int scale, State *state)
{

}