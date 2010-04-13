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
#include "State_Interactive.h"

State::State(Game *game) : _game(game)
{
	
}

State::~State()
{
	for (vector<Surface*>::iterator i = _surfaces.begin(); i < _surfaces.end(); i++)
		delete *i;
}

void State::add(Surface *surface)
{
	surface->setPalette(_game->getScreen()->getPalette());
	_surfaces.push_back(surface);
}

void State::handle(SDL_Event *ev, int scale)
{
	for (vector<Surface*>::iterator i = _surfaces.begin(); i < _surfaces.end(); i++)
	{
		InteractiveSurface* j = dynamic_cast<InteractiveSurface*>(*i);
		if (j != 0)
			j->handle(ev, scale, this);
	}
}

void State::blit()
{
	for (vector<Surface*>::iterator i = _surfaces.begin(); i < _surfaces.end(); i++)
		(*i)->blit(_game->getScreen()->getSurface());
}