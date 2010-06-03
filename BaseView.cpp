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
#include "BaseView.h"

/**
 * Sets up a base view with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
BaseView::BaseView(Base *base, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _base(base)
{
}

/**
 *
 */
BaseView::~BaseView()
{
}

void BaseView::setBase(Base *base)
{
	_base = base;
	draw();
}

void BaseView::setTexture(SurfaceSet *texture)
{
	_texture = texture;
	draw();
}

void BaseView::draw()
{
	// Draw grid squares
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			_texture->getSurface()->setX(x * GRID_SIZE);
			_texture->getSurface()->setY(y * GRID_SIZE);
			_texture->getFrame(0)->blit(this);
		}
	}

	// Draw facilities
	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		int num = 0;
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); y++)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); x++)
			{				
				_texture->getSurface()->setX(x * GRID_SIZE);
				_texture->getSurface()->setY(y * GRID_SIZE);
				_texture->getFrame((*i)->getRules()->getSpriteShape() + num)->blit(this);
				_texture->getFrame((*i)->getRules()->getSpriteFacility() + num)->blit(this);
				num++;
			}
		}
	}
}