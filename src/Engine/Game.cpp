/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include <cmath>
#include <sstream>
#include <SDL_mixer.h>
#include "Adlib/adlplayer.h"
#include "State.h"
#include "Screen.h"
#include "Sound.h"
#include "Music.h"
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
#include "FileMap.h"
#include "../Menu/TestState.h"

namespace OpenXcom
{

const double Game::VOLUME_GRADIENT = 10.0;

/**
 * Starts up SDL with all the subsystems and SDL_mixer for audio processing,
 * creates the display screen and sets up the cursor.
 * @param title Title of the game window.
 */
Game::Game(const std::string &title) : _screen(0), _cursor(0), _lang(0), _res(0), _save(0), _rules(0), _quit(false), _init(false), _mouseActive(true), _timeUntilNextFrame(0)
{
	Options::reload = false;
	Options::mute = false;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw Exception(SDL_GetError());
	}
	Log(LOG_INFO) << "SDL initialized successfully.";

	// Initialize SDL_mixer
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		Log(LOG_ERROR) << SDL_GetError();
		Log(LOG_WARNING) << "No sound device detected, audio disabled.";
		Options::mute = true;
	}
	else
	{
		initAudio();
	}

	// trap the mouse inside the window
	SDL_WM_GrabInput(Options::captureMouse);
	
	// Set the window icon
	CrossPlatform::setWindowIcon(103, FileMap::getFilePath("openxcom.png"));

	// Set the window caption
	SDL_WM_SetCaption(title.c_str(), 0);

	SDL_EnableUNICODE(1);

	// Create display
	_screen = new Screen();

	// Create cursor
	_cursor = new Cursor(9, 13);
	
	// Create invisible hardware cursor to workaround bug with absolute positioning pointing devices
	SDL_ShowCursor(SDL_ENABLE);
	Uint8 cursor = 0;
	SDL_SetCursor(SDL_CreateCursor(&cursor, &cursor, 1,1,0,0));

	// Create fps counter
	_fpsCounter = new FpsCounter(15, 5, 0, 0);

	// Create blank language
	_lang = new Language();

	_timeOfLastFrame = 0;
}

/**
 * Deletes the display screen, cursor, states and shuts down all the SDL subsystems.
 */
Game::~Game()
{
	Sound::stop();
	Music::stop();

	for (std::list<State*>::iterator i = _states.begin(); i != _states.end(); ++i)
	{
		delete *i;
	}

	SDL_FreeCursor(SDL_GetCursor());

	delete _cursor;
	delete _lang;
	delete _res;
	delete _save;
	delete _rules;
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
	// this will avoid processing SDL's resize event on startup, workaround for the heap allocation error it causes.
	bool startupEvent = Options::allowResize;
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
			Action action = Action(&ev, _screen->getXScale(), _screen->getYScale(), _screen->getCursorTopBlackBand(), _screen->getCursorLeftBlackBand());
			_states.back()->handle(&action);
		}

		// Process events
		while (SDL_PollEvent(&_event))
		{
			if (CrossPlatform::isQuitShortcut(_event))
				_event.type = SDL_QUIT;
			switch (_event.type)
			{
				case SDL_QUIT:
					quit();
					break;
				case SDL_ACTIVEEVENT:
					switch (reinterpret_cast<SDL_ActiveEvent*>(&_event)->state)
					{
						case SDL_APPACTIVE:
							runningState = reinterpret_cast<SDL_ActiveEvent*>(&_event)->gain ? RUNNING : stateRun[Options::pauseMode];
							break;
						case SDL_APPMOUSEFOCUS:
							// We consciously ignore it.
							break;
						case SDL_APPINPUTFOCUS:
							runningState = reinterpret_cast<SDL_ActiveEvent*>(&_event)->gain ? RUNNING : kbFocusRun[Options::pauseMode];
							break;
					}
					break;
				case SDL_VIDEORESIZE:
					if (Options::allowResize)
					{
						if (!startupEvent)
						{
							Options::newDisplayWidth = Options::displayWidth = std::max(Screen::ORIGINAL_WIDTH, _event.resize.w);
							Options::newDisplayHeight = Options::displayHeight = std::max(Screen::ORIGINAL_HEIGHT, _event.resize.h);
							int dX = 0, dY = 0;
							Screen::updateScale(Options::battlescapeScale, Options::battlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, false);
							Screen::updateScale(Options::geoscapeScale, Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, false);
							for (std::list<State*>::iterator i = _states.begin(); i != _states.end(); ++i)
							{
								(*i)->resize(dX, dY);
							}
							_screen->resetDisplay();
						}
						else
						{
							startupEvent = false;
						}
					}
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
					Action action = Action(&_event, _screen->getXScale(), _screen->getYScale(), _screen->getCursorTopBlackBand(), _screen->getCursorLeftBlackBand());
					_screen->handle(&action);
					_cursor->handle(&action);
					_fpsCounter->handle(&action);
					_states.back()->handle(&action);
					if (action.getDetails()->type == SDL_KEYDOWN)
					{
						// "ctrl-g" grab input
						if (action.getDetails()->key.keysym.sym == SDLK_g && (SDL_GetModState() & KMOD_CTRL) != 0)
						{
							Options::captureMouse = (SDL_GrabMode)(!Options::captureMouse);
							SDL_WM_GrabInput(Options::captureMouse);
						}
						else if (Options::debug)
						{
							if (action.getDetails()->key.keysym.sym == SDLK_t && (SDL_GetModState() & KMOD_CTRL) != 0)
							{
								setState(new TestState);
							}
							// "ctrl-u" debug UI
							else if (action.getDetails()->key.keysym.sym == SDLK_u && (SDL_GetModState() & KMOD_CTRL) != 0)
							{
								Options::debugUi = !Options::debugUi;
								_states.back()->redrawText();
							}
						}
					}
					break;
			}
		}
		
		// Process rendering
		if (runningState != PAUSED)
		{
			// Process logic
			_states.back()->think();
			_fpsCounter->think();
			if (Options::FPS > 0 && !(Options::useOpenGL && Options::vSyncForOpenGL))
			{
				// Update our FPS delay time based on the time of the last draw.
				int fps = SDL_GetAppState() & SDL_APPINPUTFOCUS ? Options::FPS : Options::FPSInactive;

				_timeUntilNextFrame = (1000.0f / fps) - (SDL_GetTicks() - _timeOfLastFrame);
			}
			else
			{
				_timeUntilNextFrame = 0;
			}

			if (_init && _timeUntilNextFrame <= 0)
			{
				// make a note of when this frame update occured.
				_timeOfLastFrame = SDL_GetTicks();
				_fpsCounter->addFrame();
				_screen->clear();
				std::list<State*>::iterator i = _states.end();
				do
				{
					--i;
				}
				while (i != _states.begin() && !(*i)->isScreen());

				for (; i != _states.end(); ++i)
				{
					(*i)->blit();
				}
				_fpsCounter->blit(_screen->getSurface());
				_cursor->blit(_screen->getSurface());
				_screen->flip();
			}
		}

		// Save on CPU
		switch (runningState)
		{
			case RUNNING: 
				SDL_Delay(1); //Save CPU from going 100%
				break;
			case SLOWED: case PAUSED:
				SDL_Delay(100); break; //More slowing down.
		}
	}

	Options::save();
}

/**
 * Stops the state machine and the game is shut down.
 */
void Game::quit()
{
	// Always save ironman
	if (_save != 0 && _save->isIronman() && !_save->getName().empty())
	{
		std::string filename = CrossPlatform::sanitizeFilename(Language::wstrToFs(_save->getName())) + ".sav";
		_save->save(filename);
	}
	_quit = true;
}

/**
 * Changes the audio volume of the music and
 * sound effect channels.
 * @param sound Sound volume, from 0 to MIX_MAX_VOLUME.
 * @param music Music volume, from 0 to MIX_MAX_VOLUME.
 * @param ui UI volume, from 0 to MIX_MAX_VOLUME.
 */
void Game::setVolume(int sound, int music, int ui)
{
	if (!Options::mute)
	{
		if (sound >= 0)
		{
			sound = volumeExponent(sound) * (double)SDL_MIX_MAXVOLUME;
			Mix_Volume(-1, sound);
			// channel 3: reserved for ambient sound effect.
			Mix_Volume(3, sound / 2);
		}
		if (music >= 0)
		{
			music = volumeExponent(music) * (double)SDL_MIX_MAXVOLUME;
			Mix_VolumeMusic(music);
		}
		if (ui >= 0)
		{
			ui = volumeExponent(ui) * (double)SDL_MIX_MAXVOLUME;
			Mix_Volume(1, ui);
			Mix_Volume(2, ui);
		}
	}
}

double Game::volumeExponent(int volume)
{
	return (exp(log(Game::VOLUME_GRADIENT + 1.0) * volume / (double)SDL_MIX_MAXVOLUME) -1.0 ) / Game::VOLUME_GRADIENT;
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
	std::ostringstream ss;
	ss << "/Language/" << filename << ".yml";

	_lang->load(CrossPlatform::searchDataFile("common" + ss.str()));

	for (std::vector< std::pair<std::string, bool> >::const_iterator i = Options::mods.begin(); i != Options::mods.end(); ++i)
	{
		if (i->second)
		{
			std::string modId = i->first;
			ModInfo modInfo = Options::getModInfos().find(modId)->second;
			std::string file = modInfo.getPath() + ss.str();
			if (CrossPlatform::fileExists(file))
			{
				_lang->load(file);				
			}
		}
	}

	ExtraStrings *strings = 0;
	std::map<std::string, ExtraStrings *> extraStrings = _rules->getExtraStrings();
	if (!extraStrings.empty())
	{
		if (extraStrings.find(filename) != extraStrings.end())
		{
			strings = extraStrings[filename];
		}
	}
	_lang->load(strings);
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
	delete _res;
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
 * Loads the rulesets specified in the game options.
 */
void Game::loadRulesets()
{
	Ruleset::resetGlobalStatics();
	delete _rules;
	_rules = new Ruleset();
	const std::vector<std::pair<std::string, std::vector<std::string> > > &rulesets(FileMap::getRulesets());
	for (size_t i = 0; rulesets.size() > i; ++i)
	{
		try
		{
			_rules->loadModRulesets(rulesets[i].second, i);
		}
		catch (YAML::Exception &e)
		{
			const std::string &modId = rulesets[i].first;
			Log(LOG_WARNING) << "disabling mod with invalid ruleset: " << modId;
			std::vector<std::pair<std::string, bool> >::iterator it =
				std::find(Options::mods.begin(), Options::mods.end(),
					  std::pair<std::string, bool>(modId, true));
			if (it == Options::mods.end())
			{
				Log(LOG_ERROR) << "cannot find broken mod in mods list: " << modId;
				Log(LOG_ERROR) << "clearing mods list";
				Options::mods.clear();
			}
			else
			{
				it->second = false;
			}
			Options::save();

			throw Exception("failed to load ruleset from mod '" +
				Options::getModInfos().at(modId).getName() +
				"' (" + std::string(e.what()) +
				"); disabling mod for next startup");
		}
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
 * Returns whether current state is *state
 * @param state The state to test against the stack state
 * @return Is state the current state?
 */
bool Game::isState(State *state) const
{
	return !_states.empty() && _states.back() == state;
}

/**
 * Checks if the game is currently quitting.
 * @return whether the game is shutting down or not.
 */
bool Game::isQuitting() const
{
	return _quit;
}

/**
 * Loads the most appropriate language
 * given current system and game options.
 */
void Game::defaultLanguage()
{
	const std::string defaultLang = "en-US";
	std::string currentLang = defaultLang;

	delete _lang;
	_lang = new Language();

	std::ostringstream ss;
	ss << "common/Language/" << defaultLang << ".yml";
	std::string defaultPath = CrossPlatform::searchDataFile(ss.str());
	std::string path = defaultPath;

	// No language set, detect based on system
	if (Options::language.empty())
	{
		std::string locale = CrossPlatform::getLocale();
		std::string lang = locale.substr(0, locale.find_first_of('-'));
		// Try to load full locale
		Language::replace(path, defaultLang, locale);
		if (CrossPlatform::fileExists(path))
		{
			currentLang = locale;
		}
		else
		{
			// Try to load language locale
			Language::replace(path, locale, lang);
			if (CrossPlatform::fileExists(path))
			{
				currentLang = lang;
			}
			// Give up, use default
			else
			{
				currentLang = defaultLang;
			}
		}
	}
	else
	{
		// Use options language
		Language::replace(path, defaultLang, Options::language);
		if (CrossPlatform::fileExists(path))
		{
			currentLang = Options::language;
		}
		// Language not found, use default
		else
		{
			currentLang = defaultLang;
		}
	}

	loadLanguage(defaultLang);
	if (currentLang != defaultLang)
	{
		loadLanguage(currentLang);
	}
	Options::language = currentLang;
}

/**
 * Initializes the audio subsystem.
 */
void Game::initAudio()
{
	Uint16 format;
	if (Options::audioBitDepth == 8)
		format = AUDIO_S8;
	else
		format = AUDIO_S16SYS;
	if (Mix_OpenAudio(Options::audioSampleRate, format, 2, 1024) != 0)
	{
		Log(LOG_ERROR) << Mix_GetError();
		Log(LOG_WARNING) << "No sound device detected, audio disabled.";
		Options::mute = true;
	}
	else
	{
		Mix_AllocateChannels(16);
		// Set up UI channels
		Mix_ReserveChannels(4);
		Mix_GroupChannels(1, 2, 0);
		Log(LOG_INFO) << "SDL_mixer initialized successfully.";
		setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
	}
}

}
