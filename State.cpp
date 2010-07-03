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


/**
 * Initializes a brand new state with no child elements.
 * By default states are full-screen.
 * @param game Pointer to the core game.
 */
State::State(Game *game) : _game(game), _screen(true)
{
	
}

/**
 * Deletes all the child elements contained in the state.
 */
State::~State()
{
	for (vector<Surface*>::iterator i = _surfaces.begin(); i < _surfaces.end(); i++)
		delete *i;
}

/**
 * Adds a new child surface for the state to take care of,
 * giving it the game's display palette. Once associated,
 * the state handles all of the surface's behaviour
 * and management automatically.
 * @param surface Child surface.
 * @note Since visible elements can overlap one another,
 * they have to be added in ascending Z-Order to be blitted
 * correctly onto the screen.
 */
void State::add(Surface *surface)
{
	surface->setPalette(_game->getScreen()->getPalette());
	_surfaces.push_back(surface);
}

/**
 * Returns whether this is a full-screen state.
 * This is used to optimize the state machine since full-screen
 * states automatically cover the whole screen, (whether they
 * actually use it all or not) so states behind them can be
 * safely ignored since they'd be covered up.
 */
bool State::isScreen()
{
	return _screen;
}

/**
 * Initializes the state and its child elements. This is
 * used for settings that have to be reset everytime the
 * state is returned to focus (eg. palettes), so can't
 * just be put in the constructor (remember there's a stack
 * of states, so they can be created once while being
 * repeatedly switched back into focus).
 */
void State::init()
{

}

/**
 * Runs any code the state needs to keep updating every
 * game cycle, like timers and other real-time elements.
 */
void State::think()
{

}

/**
 * Takes care of any events from the core game engine,
 * and passes them on to its InteractiveSurface child elements.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 */
void State::handle(SDL_Event *ev, int scale)
{
	for (vector<Surface*>::iterator i = _surfaces.begin(); i < _surfaces.end(); i++)
	{
		InteractiveSurface* j = dynamic_cast<InteractiveSurface*>(*i);
		if (j != 0)
			j->handle(ev, scale, this);
	}
}

/**
 * Blits all the visible Surface child elements onto the
 * display screen, by order of addition.
 */
void State::blit()
{
	for (vector<Surface*>::iterator i = _surfaces.begin(); i < _surfaces.end(); i++)
		(*i)->blit(_game->getScreen()->getSurface());
}