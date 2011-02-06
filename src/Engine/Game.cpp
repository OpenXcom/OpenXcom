/*
 * Copyright 2010 OpenXcom Developers.
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
#include "Language.h"
#include "../Interface/Cursor.h"
#include "../Interface/FpsCounter.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/SavedGame.h"
#include "Palette.h"
#include "Action.h"
#include "Exception.h"

namespace OpenXcom
{

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
Game::Game(const std::string &title, int width, int height, int bpp) : _screen(0), _cursor(0), _lang(0), _states(), _deleted(), _res(0), _save(0), _rules(0), _quit(false), _init(false)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		throw Exception(SDL_GetError());
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024) != 0)
	{
		throw Exception(Mix_GetError());
	}
	Mix_AllocateChannels(16);

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
	
	// Create fps counter
	_fpsCounter = new FpsCounter(15, 5, 0, 0);
}

/**
 * Deletes the display screen, cursor, states and shuts down all the SDL subsystems.
 */
Game::~Game()
{
	Mix_HaltChannel(-1);

	for (std::list<State*>::iterator i = _states.begin(); i != _states.end(); i++)
	{
		delete *i;
	}

	delete _cursor;
	delete _lang;
	delete _res;
	delete _rules;
	delete _save;
	delete _screen;
	delete _fpsCounter;

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

		// Initialize active state
		if (!_init)
		{
			_states.back()->init();
			_init = true;
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
				Action action = Action(&_event, _screen->getXScale(), _screen->getYScale());
				_screen->handle(&action);
				_cursor->handle(&action);
				_fpsCounter->handle(&action);
				_states.back()->handle(&action);
			}
		}
		
		// Process logic
		_fpsCounter->think();
		_states.back()->think();
		
		// Process rendering
		if (_init)
		{
			_screen->clear();
			std::list<State*>::iterator i = _states.end();
			do
			{
				i--;
			}
			while(i != _states.begin() && !(*i)->isScreen());

			for (; i != _states.end(); i++)
			{
				(*i)->blit();
			}
			_fpsCounter->blit(_screen->getSurface());
			_cursor->blit(_screen->getSurface());
		}
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
Screen *const Game::getScreen() const
{
	return _screen;
}

/**
 * Returns the mouse cursor used by the game.
 * @return Pointer to the cursor.
 */
Cursor *const Game::getCursor() const
{
	return _cursor;
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
	_cursor->draw();

	_fpsCounter->setPalette(colors, firstcolor, ncolors);
	
	if (_res != 0)
	{
		_res->setPalette(colors, firstcolor, ncolors);
	}
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
	{
		popState();
	}
	pushState(state);
	_init = false;
}

/**
 * Pushes a new state into the top of the stack and initializes it.
 * The new state will be used once the next game cycle starts.
 * @param state Pointer to the new state.
 */
void Game::pushState(State *state)
{
	_states.push_back(state);
	_init = false;
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
	_init = false;
}

/**
 * Returns the language currently in use by the game.
 * @return Pointer to the language.
 */
Language *const Game::getLanguage() const
{
	return _lang;
}

/**
 * Changes the language currently in use by the game.
 * @param lang Pointer to the language.
 */
void Game::setLanguage(Language *lang)
{
	delete _lang;
	_lang = lang;
}

/**
 * Returns the resource pack currently in use by the game.
 * @return Pointer to the resource pack.
 */
ResourcePack *const Game::getResourcePack() const
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
SavedGame *const Game::getSavedGame() const
{
	return _save;
}

/**
 * Sets a new saved game for the game to use.
 * @param save Pointer to the saved game.
 */
void Game::setSavedGame(SavedGame *save)
{
	delete _save;
	_save = save;
}

/**
 * Returns the ruleset currently in use by the game.
 * @return Pointer to the ruleset.
 */
Ruleset *const Game::getRuleset() const
{
	return _rules;
}

/**
 * Sets a new ruleset for the game to use.
 * @param rules Pointer to the ruleset.
 */
void Game::setRuleset(Ruleset *rules)
{
	delete _rules;
	_rules = rules;
}

}
