/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <SDL_syswm.h>
#endif
#include <sstream>
#include <SDL_mixer.h>
#include "State.h"
#include "Screen.h"
#include "Language.h"
#include "Logger.h"
#include "../Interface/Cursor.h"
#include "../Interface/FpsCounter.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/SavedGame.h"
#include "Palette.h"
#include "Action.h"
#include "Exception.h"
#include "InteractiveSurface.h"
#include "Options.h"
#include "CrossPlatform.h"
#include "../Menu/SaveState.h"

namespace OpenXcom
{

/**
 * Starts up SDL with all the subsystems and SDL_mixer for audio processing,
 * creates the display screen and sets up the cursor.
 * @param title Title of the game window.
 */
Game::Game(const std::string &title) : _screen(0), _cursor(0), _lang(0), _states(), _deleted(), _res(0), _save(0), _rules(0), _quit(false), _init(false), _mouseActive(true)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw Exception(SDL_GetError());
	}
	Log(LOG_INFO) << "SDL initialized successfully.";

	if (!Options::getBool("mute"))
	{
		// Initialize SDL_mixer
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		{
			Log(LOG_ERROR) << SDL_GetError();
			Log(LOG_WARNING) << "No sound device detected, audio disabled.";
			Options::setBool("mute", true);
		}
		else
		{
			Uint16 format;
			if (Options::getInt("audioBitDepth") == 8)
				format = AUDIO_S8;
			else
				format = AUDIO_S16SYS;
			if (Mix_OpenAudio(Options::getInt("audioSampleRate"), format, 2, 1024) != 0)
			{
				Log(LOG_ERROR) << Mix_GetError();
				Log(LOG_WARNING) << "No sound device detected, audio disabled.";
				Options::setBool("mute", true);
			}
			else
			{
				Mix_AllocateChannels(16);
			}
		}
		Log(LOG_INFO) << "SDL_mixer initialized successfully.";
	}
	// trap the mouse inside the window
	if (Options::getBool("captureMouse"))
	{
		SDL_WM_GrabInput( SDL_GRAB_ON );
	}
	// Set the window caption
	SDL_WM_SetCaption(title.c_str(), 0);

#ifdef _WIN32
	// Set the window icon
	HINSTANCE handle = GetModuleHandle(NULL);
	HICON icon = LoadIcon(handle, MAKEINTRESOURCE(103));

	SDL_SysWMinfo wminfo;
	SDL_VERSION(&wminfo.version)
	if (SDL_GetWMInfo(&wminfo))
	{
		HWND hwnd = wminfo.window;
		SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)icon);
	}
#endif

	SDL_EnableUNICODE(1);

	// Create display
	Screen::BASE_WIDTH = Options::getInt("baseXResolution");
	Screen::BASE_HEIGHT = Options::getInt("baseYResolution");
	_screen = new Screen(Options::getInt("displayWidth"), Options::getInt("displayHeight"), 0, Options::getBool("fullscreen"), Options::getInt("windowedModePositionX"), Options::getInt("windowedModePositionY"));

	// Create cursor
	_cursor = new Cursor(9, 13);
	_cursor->setColor(Palette::blockOffset(15)+12);

	// Create fps counter
	_fpsCounter = new FpsCounter(15, 5, 0, 0);

	// Create blank language
	_lang = new Language();

#ifdef __MORPHOS__	
	waittime = 1000.0f / Options::getInt("FPS");	//20 - FPS
	framestarttime = 0;
#endif
}

/**
 * Deletes the display screen, cursor, states and shuts down all the SDL subsystems.
 */
Game::~Game()
{
	if (_save != 0 && _save->getMonthsPassed() >= 0 && Options::getInt("autosave") == 3)
	{
		SaveState *ss = new SaveState(this, true, false);
		delete ss;
	}

	Mix_HaltChannel(-1);

	for (std::list<State*>::iterator i = _states.begin(); i != _states.end(); ++i)
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
	enum ApplicationState { RUNNING = 0, SLOWED = 1, PAUSED = 2 } runningState = RUNNING;
	static const ApplicationState kbFocusRun[4] = { RUNNING, RUNNING, SLOWED, PAUSED };
	static const ApplicationState stateRun[4] = { SLOWED, PAUSED, PAUSED, PAUSED };
	int pauseMode = Options::getInt("pauseMode");
	if (pauseMode > 3)
		pauseMode = 3;
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
			_init = true;
			_states.back()->init();

			// Unpress buttons
			_states.back()->resetAll();

			// Refresh mouse position
			SDL_Event ev;
			int x, y;
			SDL_GetMouseState(&x, &y);
			ev.type = SDL_MOUSEMOTION;
			ev.motion.x = x;
			ev.motion.y = y;
			Action action = Action(&ev, _screen->getXScale(), _screen->getYScale());
			_states.back()->handle(&action);
		}

		// Process events
		while (SDL_PollEvent(&_event))
		{
			switch (_event.type)
			{
				case SDL_QUIT: _quit = true; break;
				case SDL_ACTIVEEVENT:
					switch (reinterpret_cast<SDL_ActiveEvent*>(&_event)->state)
					{
						case SDL_APPACTIVE:
							runningState = reinterpret_cast<SDL_ActiveEvent*>(&_event)->gain ? RUNNING : stateRun[pauseMode];
							break;
						case SDL_APPMOUSEFOCUS:
							// We consciously ignore it.
							break;
						case SDL_APPINPUTFOCUS:
							runningState = reinterpret_cast<SDL_ActiveEvent*>(&_event)->gain ? RUNNING : kbFocusRun[pauseMode];
							break;
					}
					break;
				case SDL_VIDEORESIZE:
					Options::setInt("displayWidth", _event.resize.w);
					Options::setInt("displayHeight", _event.resize.h);
					_screen->setResolution(_event.resize.w, _event.resize.h);
					break;
				case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					// Skip mouse events if they're disabled
					if (!_mouseActive) continue;
					// re-gain focus on mouse-over or keypress.
					runningState = RUNNING;
					// Go on, feed the event to others
				default:
					Action action = Action(&_event, _screen->getXScale(), _screen->getYScale());
					_screen->handle(&action);
					_cursor->handle(&action);
					_fpsCounter->handle(&action);
					_states.back()->handle(&action);
					break;
			}
		}

		// Process rendering
		if (runningState != PAUSED)
		{
			// Process logic
			_fpsCounter->think();
			_states.back()->think();

			if (_init)
			{
				_screen->clear();
				std::list<State*>::iterator i = _states.end();
				do
				{
					--i;
				}
				while(i != _states.begin() && !(*i)->isScreen());

				for (; i != _states.end(); ++i)
				{
					(*i)->blit();
				}
				_fpsCounter->blit(_screen->getSurface());
				_cursor->blit(_screen->getSurface());
			}
			_screen->flip();
		}

		// Save on CPU
		switch (runningState)
		{
			case RUNNING: 
#ifdef __MORPHOS__
				delaytime = waittime - (SDL_GetTicks() - framestarttime);
				if(delaytime > 0)
					SDL_Delay((Uint32)delaytime);
				framestarttime = SDL_GetTicks();
#else
				SDL_Delay(1); 
#endif
				
				break; //Save CPU from going 100%
			case SLOWED: case PAUSED:
				SDL_Delay(100); break; //More slowing down.
		}
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
 * Changes the audio volume of the music and
 * sound effect channels.
 * @param sound Sound volume, from 0 to MIX_MAX_VOLUME.
 * @param music Music volume, from 0 to MIX_MAX_VOLUME.
 */
void Game::setVolume(int sound, int music)
{
	if (!Options::getBool("mute"))
	{
		if (sound >= 0)
			Mix_Volume(-1, sound);
		if (music >= 0)
			Mix_VolumeMusic(music);
	}
}

/**
 * Returns the display screen used by the game.
 * @return Pointer to the screen.
 */
Screen *Game::getScreen() const
{
	return _screen;
}

/**
 * Returns the mouse cursor used by the game.
 * @return Pointer to the cursor.
 */
Cursor *Game::getCursor() const
{
	return _cursor;
}

/**
 * Returns the FpsCounter used by the game.
 * @return Pointer to the FpsCounter.
 */
FpsCounter *Game::getFpsCounter() const
{
	return _fpsCounter;
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
Language *Game::getLanguage() const
{
	return _lang;
}

/**
 * Changes the language currently in use by the game.
 * @param filename Filename of the language file.
 */
void Game::loadLanguage(const std::string &filename)
{
	std::stringstream ss, ss2;
	ss << "Language/" << filename << ".lng";
	ss2 << "Language/" << filename << ".geo";

	_lang->loadLng(CrossPlatform::getDataFile(ss.str()), _rules->getExtraStrings()[filename]);

	std::auto_ptr<Surface> sidebar(new Surface(64, 154));
	if (CrossPlatform::getDataFile(ss2.str()) != "")
	{
		sidebar->setPalette(_res->getSurface("GEOBORD.SCR")->getPalette());
		sidebar->loadScr(CrossPlatform::getDataFile(ss2.str()));
	}
	sidebar->setX(256);
	sidebar->setY(0);
	sidebar->blit(_res->getSurface("GEOBORD.SCR"));

	Options::setString("language", filename);
}

/**
 * Returns the resource pack currently in use by the game.
 * @return Pointer to the resource pack.
 */
ResourcePack *Game::getResourcePack() const
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
SavedGame *Game::getSavedGame() const
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
Ruleset *Game::getRuleset() const
{
	return _rules;
}

/**
 * Changes the ruleset currently in use by the game.
 * @param filename Filename of the language file.
 */
void Game::loadRuleset()
{
	_rules = new Ruleset();
	std::vector<std::string> rulesets = Options::getRulesets();
	for (std::vector<std::string>::iterator i = rulesets.begin(); i != rulesets.end(); ++i)
	{
		_rules->load(*i);
	}
	_rules->sortLists();
}

/**
 * Sets whether the mouse is activated.
 * If it is, mouse events are processed, otherwise
 * they are ignored and the cursor is hidden.
 * @param active Is mouse activated?
 */
void Game::setMouseActive(bool active)
{
	_mouseActive = active;
	_cursor->setVisible(active);
}

/**
 * @brief Returns whether current state is *state
 * @param state The state to test against the stack state
 */
bool Game::isState(State *state) const
{
	return _states.size() > 0 && _states.back() == state;
}

/**
 * @return whether the game is shutting down or not.
 */
bool Game::isQuitting() const
{
	return _quit;
}


}
