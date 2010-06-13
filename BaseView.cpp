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
 * @param big Pointer to the big-size font.
 * @param small Pointer to the small-size font.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
BaseView::BaseView(Font *big, Font *small, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _base(0), _texture(0), _selFacility(0), _big(big), _small(small)
{
	for (int x = 0; x < BASE_SIZE; x++)
		for (int y = 0; y < BASE_SIZE; y++)
			_facilities[x][y] = 0;
}

/**
 *
 */
BaseView::~BaseView()
{
}

/**
 * Changes the current base to display and
 * initalizes the internal base grid.
 * @param base Pointer to base to display.
 */
void BaseView::setBase(Base *base)
{
	_base = base;
	_selFacility = 0;

	// Clear grid
	for (int x = 0; x < BASE_SIZE; x++)
		for (int y = 0; y < BASE_SIZE; y++)
			_facilities[x][y] = 0;

	// Fill grid with base facilities
	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); y++)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); x++)
			{				
				_facilities[x][y] = *i;
			}
		}
	}

	draw();
}

/**
 * Changes the texture to use for drawing
 * the various base elements.
 * @param texture Pointer to SurfaceSet to use.
 */
void BaseView::setTexture(SurfaceSet *texture)
{
	_texture = texture;
}

/**
 * Returns the facility the mouse is currently over.
 * @return Pointer to base facility (NULL if none).
 */
BaseFacility *BaseView::getSelectedFacility()
{
	return _selFacility;
}

/**
 * Draws the view of all the facilities in the base, connectors
 * between them and crafts landed in hangars.
 */
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

	vector<Craft*>::iterator craft = _base->getCrafts()->begin();
	
	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		// Draw facility shape
		int num = 0;
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); y++)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); x++)
			{				
				_texture->getSurface()->setX(x * GRID_SIZE);
				_texture->getSurface()->setY(y * GRID_SIZE);

				if ((*i)->getBuildTime() == 0)
					_texture->getFrame((*i)->getRules()->getSpriteShape() + num)->blit(this);
				else
					_texture->getFrame((*i)->getRules()->getSpriteShape() + num + 2 + (*i)->getRules()->getSize())->blit(this);

				num++;
			}
		}
	}

	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		// Draw connectors
		if ((*i)->getBuildTime() == 0)
		{
			// Facilities to the right
			int x = (*i)->getX() + (*i)->getRules()->getSize();
			if (x < BASE_SIZE)
			{
				for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); y++)
				{
					if (_facilities[x][y] != 0 && _facilities[x][y]->getBuildTime() == 0)
					{
						_texture->getSurface()->setX(x * GRID_SIZE - GRID_SIZE / 2);
						_texture->getSurface()->setY(y * GRID_SIZE);
						_texture->getFrame(7)->blit(this);
					}
				}
			}

			// Facilities to the bottom
			int y = (*i)->getY() + (*i)->getRules()->getSize();
			if (y < BASE_SIZE)
			{
				for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); x++)
				{
					if (_facilities[x][y] != 0 && _facilities[x][y]->getBuildTime() == 0)
					{
						_texture->getSurface()->setX(x * GRID_SIZE);
						_texture->getSurface()->setY(y * GRID_SIZE - GRID_SIZE / 2);
						_texture->getFrame(8)->blit(this);
					}
				}
			}
		}
	}

	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		// Draw facility graphic
		int num = 0;
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); y++)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); x++)
			{				
				_texture->getSurface()->setX(x * GRID_SIZE);
				_texture->getSurface()->setY(y * GRID_SIZE);

				if ((*i)->getRules()->getSize() == 1)
					_texture->getFrame((*i)->getRules()->getSpriteFacility() + num)->blit(this);

				num++;
			}
		}

		// Draw crafts
		if ((*i)->getBuildTime() == 0 && (*i)->getRules()->getCrafts() > 0 && craft != _base->getCrafts()->end())
		{
			_texture->getSurface()->setX((*i)->getX() * GRID_SIZE + ((*i)->getRules()->getSize() - 1) * GRID_SIZE / 2);
			_texture->getSurface()->setY((*i)->getY() * GRID_SIZE + ((*i)->getRules()->getSize() - 1) * GRID_SIZE / 2);

			_texture->getFrame((*craft)->getRules()->getSprite())->blit(this);
			craft++;
		}

		// Draw time remaining
		if ((*i)->getBuildTime() > 0)
		{
			Text *text = new Text(_big, _small, GRID_SIZE * (*i)->getRules()->getSize(), 16, 0, 0);
			text->setPalette(getPalette());
			text->setX((*i)->getX() * GRID_SIZE);
			text->setY((*i)->getY() * GRID_SIZE + (GRID_SIZE * (*i)->getRules()->getSize() - 16) / 2);
			text->setBig();
			stringstream ss;
			ss << (*i)->getBuildTime();
			text->setAlign(ALIGN_CENTER);
			text->setColor(Palette::blockOffset(13)+5);
			text->setText(ss.str());
			text->blit(this);
		}
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void BaseView::mousePress(SDL_Event *ev, int scale, State *state)
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
void BaseView::mouseRelease(SDL_Event *ev, int scale, State *state)
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
void BaseView::mouseClick(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseClick(ev, scale, state);
	}
}

/**
 * Selects the facility the mouse is over.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void BaseView::mouseOver(SDL_Event *ev, int scale, State *state)
{
	double x = ev->button.x - _x * scale;
	double y = ev->button.y - _y * scale;
	int xx = (int)floor(x / (GRID_SIZE * scale));
	int yy = (int)floor(y / (GRID_SIZE * scale));
	if (xx >= 0 && xx < BASE_SIZE && yy >= 0 && yy < BASE_SIZE)
		_selFacility = _facilities[xx][yy];
	else
		_selFacility = 0;

	InteractiveSurface::mouseOver(ev, scale, state);
}
	