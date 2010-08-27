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
#include "StartState.h"
#include <iostream>
#include "SDL.h"
#include "SDL_gfxPrimitives.h"
#include "Game.h"
#include "XcomResourcePack.h"
#include "XcomRuleset.h"
#include "Surface.h"
#include "TestState.h"
#include "LanguageState.h"

#define DATA_FOLDER "./DATA/"

/**
 * Initializes all the elements in the Loading screen.
 * @param game Pointer to the core game.
 */
StartState::StartState(Game *game) : State(game), _load(LOADING_NONE)
{
	// Create objects
	_surface = new Surface(320, 200, 0, 0);

	// Set palette
	SDL_Color bnw[2];

	bnw[0].r = 0;
	bnw[0].g = 0;
	bnw[0].b = 0;
	bnw[1].r = 255;
	bnw[1].g = 255;
	bnw[1].b = 255;

	_game->setPalette(bnw, 0, 2);

	add(_surface);

	// Set up objects
	stringColor(_surface->getSurface(), 120, 96, "Loading...", 0xFFFFFFFF);
}

/**
 *
 */
StartState::~StartState()
{
	
}

/**
 * Waits a cycle to load the resources so the screen is blitted first.
 * If the loading fails, it shows an error, otherwise moves on to the game.
 */
void StartState::think()
{
	State::think();

	switch (_load)
	{
	case LOADING_STARTED:
		try
		{
			_game->setResourcePack(new XcomResourcePack(DATA_FOLDER));
			_game->setRuleset(new XcomRuleset());
			_load = LOADING_SUCCESSFUL;
		}
		catch (const char* c)
		{
			cout << c << endl;
			_load = LOADING_FAILED;
			_surface->clear();
			stringColor(_surface->getSurface(), 0, 0, c, 0xFFFFFFFF);
			stringColor(_surface->getSurface(), 0, 16, "Make sure X-Com is in the DATA subfolder", 0xFFFFFFFF);
			stringColor(_surface->getSurface(), 0, 192, "Press any key to quit", 0xFFFFFFFF);
		}
		break;
	case LOADING_NONE:
		_load = LOADING_STARTED;
		break;
	case LOADING_SUCCESSFUL:
		//_game->setState(new TestState(_game));
		_game->setState(new LanguageState(_game));
		break;
	default:
		break;
	}
}

/**
 * The game quits if the player presses any key when an error
 * message is on display.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 */
void StartState::handle(SDL_Event *ev, int scale)
{
	if (_load == LOADING_FAILED)
	{
		if (ev->type == SDL_KEYDOWN)
			_game->quit();
	}
}
