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
#include "Game.h"
#include "State_Interactive.h"

Game::Game(char* title, int width, int height, int bpp) : _states(), _deleted(), _quit(false)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		throw SDL_GetError();
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) != 0)
	{
		throw Mix_GetError();
	}

	// Set the window caption
	SDL_WM_SetCaption(title, 0);

	// Create display
	_screen = new Screen(width, height, bpp);

	// Create cursor
	_cursor = new Cursor(9, 13);
	_cursor->setColor(Palette::blockOffset(15)+12);
}

Game::~Game()
{
	delete _screen;

	Mix_CloseAudio();

	SDL_Quit();
}

void Game::run()
{
	while (!_quit)
	{
		// Clean up states
		while (!_deleted.empty())
		{
			delete _deleted.back();
			_deleted.pop_back();
		}

		// Process events
		while (SDL_PollEvent(&_event))
		{
			if (_event.type == SDL_QUIT)
			{
				_quit = true;
			}
			else
			{
				_cursor->handle(&_event, _screen->getScale());
				_states.back()->handle(&_event, _screen->getScale());
			}
		}
		
		// Process logic
		_states.back()->think();
		
		// Process rendering
		_screen->clear();
		list<State*>::iterator i = _states.end();
		do
			i--;
		while(i != _states.begin() && !(*i)->isScreen());

		for (; i != _states.end(); i++)
			(*i)->blit();
		_cursor->blit(_screen->getSurface());
		_screen->flip();
	}
}

void Game::quit()
{
	_quit = true;
}

Screen *Game::getScreen()
{
	return _screen;
}

void Game::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	_screen->setPalette(colors, firstcolor, ncolors);
	_cursor->setPalette(colors, firstcolor, ncolors);
	_res->setPalette(colors, firstcolor, ncolors);
}

void Game::setState(State *state)
{
	while (!_states.empty())
		popState();
	pushState(state);
}

void Game::pushState(State *state)
{
	_states.push_back(state);
}

void Game::popState()
{
	_deleted.push_back(_states.back());
	_states.pop_back();
}

ResourcePack *Game::getResourcePack()
{
	return _res;
}

void Game::setResourcePack(ResourcePack *res)
{
	_res = res;
}

SavedGame *Game::getSavedGame()
{
	return _save;
}

void Game::setSavedGame(SavedGame *save)
{
	_save = save;
}

Ruleset *Game::getRuleset()
{
	return _rules;
}

void Game::setRuleset(Ruleset *rules)
{
	_rules = rules;
}