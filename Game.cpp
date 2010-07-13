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
#include "SDL_mixer.h"
#include "State.h"
#include "Screen.h"
#include "Cursor.h"
#include "ResourcePack.h"
#include "Ruleset.h"
#include "SavedGame.h"
#include "Palette.h"

/**
 * Starts up SDL with all the subsystems and SDL_mixer for audio processing,
 * creates the display screen and sets up the cursor.
 * @param title Title of the game window.
 * @param width Width of the display screen.
 * @param height Height of the display screen.
 * @param bpp Bits-per-pixel of the display screen.
 * @warning Currently the game is designed for 8bpp, so there's no telling what'll
 * happen if you use a different value.
 */
Game::Game(string title, int width, int height, int bpp) : _screen(0), _cursor(0), _states(), _deleted(), _res(0), _save(0), _rules(0), _quit(false)
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
	SDL_WM_SetCaption(title.c_str(), 0);

	// Set up keyboard events
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_EnableUNICODE(1);

	// Create display
	_screen = new Screen(width, height, bpp);

	// Create cursor
	_cursor = new Cursor(9, 13);
	_cursor->setColor(Palette::blockOffset(15)+12);
}

/**
 * Shuts down all the SDL subsystems and deletes the display screen and cursor.
 */
Game::~Game()
{
	delete _screen;
	delete _cursor;
	if (_res != 0)
		delete _res;
	if (_rules != 0)
		delete _rules;
	if (_save != 0)
		delete _save;

	for (list<State*>::iterator i = _states.begin(); i != _states.end(); i++)
		delete *i;

	Mix_CloseAudio();

	SDL_Quit();
}

/**
 * The state machine takes care of passing all the events from SDL to the
 * active state, running any code within and blitting all the states and
 * cursor to the screen. This is run indefinitely until the game quits.
 */
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

		SDL_Delay(0);
	}
}

/**
 * Stops the state machine and the game is shut down.
 */
void Game::quit()
{
	_quit = true;
}

/**
 * Returns the display screen used by the game.
 * @return Pointer to the screen.
 */
Screen *Game::getScreen()
{
	return _screen;
}

/**
 * Returns the state stack currently running
 * @return Pointer to the state stack.
 */
list<State*> *Game::getStates()
{
	return &_states;
}

/**
 * Replaces a certain amount of colors in the palettes of the game's
 * screen and resources.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Game::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	_screen->setPalette(colors, firstcolor, ncolors);
	_cursor->setPalette(colors, firstcolor, ncolors);
	if (_res != 0)
		_res->setPalette(colors, firstcolor, ncolors);
}

/**
 * Pops all the states currently in stack and pushes in the new state.
 * A shortcut for cleaning up all the old states when they're not necessary
 * like in one-way transitions.
 * @param state Pointer to the new state.
 */
void Game::setState(State *state)
{
	while (!_states.empty())
		popState();
	pushState(state);
}

/**
 * Pushes a new state into the top of the stack and initializes it.
 * The new state will be used once the next game cycle starts.
 * @param state Pointer to the new state.
 */
void Game::pushState(State *state)
{
	_states.push_back(state);
	state->init();
}

/**
 * Pops the last state from the top of the stack. Since states
 * can't actually be deleted mid-cycle, it's moved into a separate queue
 * which is cleared at the start of every cycle, so the transition
 * is seamless.
 */
void Game::popState()
{
	_deleted.push_back(_states.back());
	_states.pop_back();
	if (!_states.empty())
		_states.back()->init();
}

/**
 * Returns the resource pack currently in use by the game.
 * @return Pointer to the resource pack.
 */
ResourcePack *Game::getResourcePack()
{
	return _res;
}

/**
 * Sets a new resource pack for the game to use.
 * @param res Pointer to the resource pack.
 */
void Game::setResourcePack(ResourcePack *res)
{
	_res = res;
}

/**
 * Returns the saved game currently in use by the game.
 * @return Pointer to the saved game.
 */
SavedGame *Game::getSavedGame()
{
	return _save;
}

/**
 * Sets a new saved game for the game to use.
 * @param save Pointer to the saved game.
 */
void Game::setSavedGame(SavedGame *save)
{
	_save = save;
}

/**
 * Returns the ruleset currently in use by the game.
 * @return Pointer to the ruleset.
 */
Ruleset *Game::getRuleset()
{
	return _rules;
}

/**
 * Sets a new ruleset for the game to use.
 * @param rules Pointer to the ruleset.
 */
void Game::setRuleset(Ruleset *rules)
{
	_rules = rules;
}
