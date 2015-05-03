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
#include "StartState.h"
#include "../version.h"
#include "../Engine/Logger.h"
#include "../Engine/Game.h"
#include "../Engine/Screen.h"
#include "../Engine/Action.h"
#include "../Engine/Surface.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Sound.h"
#include "../Engine/Music.h"
#include "../Engine/Font.h"
#include "../Engine/Timer.h"
#include "../Engine/CrossPlatform.h"
#include "../Geoscape/Globe.h"
#include "../Interface/FpsCounter.h"
#include "../Interface/Cursor.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Resource/XcomResourcePack.h"
#include "MainMenuState.h"
#include "IntroState.h"
#include "ErrorMessageState.h"
#include <SDL_mixer.h>
#include <SDL_thread.h>

namespace OpenXcom
{

LoadingPhase StartState::loading;
std::string StartState::error;

static void _resetGlobalStatics()
{
	ResourcePack::DOOR_OPEN = 3;
	ResourcePack::SLIDING_DOOR_OPEN = 20;
	ResourcePack::SLIDING_DOOR_CLOSE = 21;
	ResourcePack::SMALL_EXPLOSION = 2;
	ResourcePack::LARGE_EXPLOSION = 5;
	ResourcePack::EXPLOSION_OFFSET = 0;
	ResourcePack::SMOKE_OFFSET = 8;
	ResourcePack::UNDERWATER_SMOKE_OFFSET = 0;
	ResourcePack::ITEM_DROP = 38;
	ResourcePack::ITEM_THROW = 39;
	ResourcePack::ITEM_RELOAD = 17;
	ResourcePack::WALK_OFFSET = 22;
	ResourcePack::FLYING_SOUND = 15;
	ResourcePack::MALE_SCREAM[0] = 41;
	ResourcePack::MALE_SCREAM[1] = 42;
	ResourcePack::MALE_SCREAM[2] = 43;
	ResourcePack::FEMALE_SCREAM[0] = 44;
	ResourcePack::FEMALE_SCREAM[1] = 45;
	ResourcePack::FEMALE_SCREAM[2] = 46;
	ResourcePack::BUTTON_PRESS = 0;
	ResourcePack::WINDOW_POPUP[0] = 1;
	ResourcePack::WINDOW_POPUP[1] = 2;
	ResourcePack::WINDOW_POPUP[2] = 3;
	ResourcePack::UFO_FIRE = 8;
	ResourcePack::UFO_HIT = 12;
	ResourcePack::UFO_CRASH = 10;
	ResourcePack::UFO_EXPLODE = 11;
	ResourcePack::INTERCEPTOR_HIT = 10;
	ResourcePack::INTERCEPTOR_EXPLODE = 13;
	ResourcePack::GEOSCAPE_CURSOR = 252;
	ResourcePack::BASESCAPE_CURSOR = 252;
	ResourcePack::BATTLESCAPE_CURSOR = 144;
	ResourcePack::UFOPAEDIA_CURSOR = 252;
	ResourcePack::GRAPHS_CURSOR = 252;
	ResourcePack::DEBRIEF_MUSIC_GOOD = "GMMARS";
	ResourcePack::DEBRIEF_MUSIC_BAD = "GMMARS";

	Globe::OCEAN_COLOR = Palette::blockOffset(12);
	Globe::COUNTRY_LABEL_COLOR = 239;
	Globe::LINE_COLOR = 162;
	Globe::CITY_LABEL_COLOR = 138;
	Globe::BASE_LABEL_COLOR = 133;

	TextButton::soundPress = 0;

	Window::soundPopup[0] = 0;
	Window::soundPopup[1] = 0;
	Window::soundPopup[2] = 0;
}

/**
 * Initializes all the elements in the Loading screen.
 * @param game Pointer to the core game.
 */
StartState::StartState() : _anim(0)
{
	//updateScale() uses newDisplayWidth/Height and needs to be set ahead of time
	Options::newDisplayWidth = Options::displayWidth;
	Options::newDisplayHeight = Options::displayHeight;

	Options::baseXResolution = Options::displayWidth;
	Options::baseYResolution = Options::displayHeight;
	_game->getScreen()->resetDisplay(false);

	// Create objects
	_thread = 0;
	loading = LOADING_STARTED;
	error = "";

	_font = new Font();
	_font->loadTerminal();
	_lang = new Language();

	_text = new Text(Options::baseXResolution, Options::baseYResolution, 0, 0);
	_cursor = new Text(_font->getWidth(), _font->getHeight(), 0, 0);
	_timer = new Timer(150);
	
	setPalette(_font->getSurface()->getPalette(), 0, 2);

	add(_text);
	add(_cursor);

	// Set up objects
	_text->initText(_font, _font, _lang);
	_text->setColor(0);
	_text->setWordWrap(true);

	_cursor->initText(_font, _font, _lang);
	_cursor->setColor(0);
	_cursor->setText(L"_");

	_timer->onTimer((StateHandler)&StartState::animate);
	_timer->start();

	// Hide UI
	_game->getCursor()->setVisible(false);
	_game->getFpsCounter()->setVisible(false);

	if (Options::reload)
	{
		addLine(L"Restarting...");
		addLine(L"");
	}
	else
	{
		addLine(Language::utf8ToWstr(CrossPlatform::getDosPath()) + L">openxcom");
	}
}

/**
 * Kill the thread in case the game is quit early.
 */
StartState::~StartState()
{
	if (_thread != 0)
	{
		SDL_KillThread(_thread);
	}
	delete _font;
	delete _timer;
	delete _lang;
}

/**
 * Reset and reload data.
 */
void StartState::init()
{
	State::init();

	// Silence!
	Sound::stop();
	Music::stop();
	_game->setResourcePack(0);
	if (!Options::mute && Options::reload)
	{
		Mix_CloseAudio();
		_game->initAudio();
	}

	// Load the game data in a separate thread
	_thread = SDL_CreateThread(load, (void*)_game);
	if (_thread == 0)
	{
		// If we can't create the thread, just load it as usual
		load((void*)_game);
	}
}

/**
 * If the loading fails, it shows an error, otherwise moves on to the game.
 */
void StartState::think()
{
	State::think();
	_timer->think(this, 0);

	switch (loading)
	{
	case LOADING_FAILED:
		CrossPlatform::flashWindow();
		addLine(L"");
		addLine(L"ERROR: " + Language::utf8ToWstr(error));
		addLine(L"Make sure you installed OpenXcom correctly.");
		addLine(L"Check the wiki documentation for more details.");
		addLine(L"");
		addLine(L"Press any key to continue.");
		loading = LOADING_DONE;
		break;
	case LOADING_SUCCESSFUL:
		CrossPlatform::flashWindow();
		Log(LOG_INFO) << "OpenXcom started successfully!";
		if (!Options::reload && Options::playIntro)
		{
			bool letterbox = Options::keepAspectRatio;
			Options::keepAspectRatio = true;
			Options::baseXResolution = Screen::ORIGINAL_WIDTH;
			Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
			_game->getScreen()->resetDisplay(false);
			_game->setState(new IntroState(letterbox));
		}
		else
		{
			Screen::updateScale(Options::geoscapeScale, Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, true);
			_game->getScreen()->resetDisplay(false);
			State *state = new MainMenuState;
			_game->setState(state);
			Options::reload = false;
		}
		_game->getCursor()->setVisible(true);
		_game->getFpsCounter()->setVisible(Options::fpsCounter);
		break;
	default:
		break;
	}
}

/**
 * The game quits if the player presses any key when an error
 * message is on display.
 * @param action Pointer to an action.
 */
void StartState::handle(Action *action)
{
	State::handle(action);
	if (loading == LOADING_DONE)
	{
		if (action->getDetails()->type == SDL_KEYDOWN)
		{
			_game->quit();
		}
	}
}

/**
 * Blinks the cursor and spreads out terminal output.
 */
void StartState::animate()
{
	_cursor->setVisible(!_cursor->getVisible());
	_anim++;

	if (loading == LOADING_STARTED)
	{
		std::wostringstream ss;
		ss << L"Loading OpenXcom " << Language::utf8ToWstr(OPENXCOM_VERSION_SHORT) << Language::utf8ToWstr(OPENXCOM_VERSION_GIT) << "...";
		if (Options::reload)
		{
			if (_anim == 2)
				addLine(ss.str());
		}
		else
		{
			switch (_anim)
			{
			case 1:
				addLine(L"DOS/4GW Protected Mode Run-time  Version 1.9");
				addLine(L"Copyright (c) Rational Systems, Inc. 1990-1993");
				break;
			case 6:
				addLine(L"");
				addLine(L"OpenXcom initialisation");
				break;
			case 7:
				addLine(L"");
				if (Options::mute)
				{
					addLine(L"No Sound Detected");
				}
				else
				{
					addLine(L"SoundBlaster Sound Effects");
					if (Options::preferredMusic == MUSIC_MIDI)
						addLine(L"General MIDI Music");
					else
						addLine(L"SoundBlaster Music");
					addLine(L"Base Port 220  Irq 7  Dma 1");
				}
				addLine(L"");
				break;
			case 9:
				addLine(ss.str());
				break;
			}
		}
	}
}

/**
 * Adds a line of text to the terminal and moves
 * the cursor appropriately.
 * @param str Text line to add.
 */
void StartState::addLine(const std::wstring &str)
{
	_output << L"\n" << str;
	_text->setText(_output.str());
	int y = _text->getTextHeight() - _font->getHeight();
	int x = _text->getTextWidth(y / _font->getHeight());
	_cursor->setX(x);
	_cursor->setY(y);
}

/**
 * Loads game data and updates status accordingly.
 * @param game_ptr Pointer to the game.
 * @return Thread status, 0 = ok
 */
int StartState::load(void *game_ptr)
{
	Game *game = (Game*)game_ptr;
	try
	{
		_resetGlobalStatics();
		Log(LOG_INFO) << "Loading rulesets...";
		game->loadRulesets();
		Log(LOG_INFO) << "Rulesets loaded successfully.";
		Log(LOG_INFO) << "Loading resources...";
		game->setResourcePack(new XcomResourcePack(game->getRuleset()));
		Log(LOG_INFO) << "Resources loaded successfully.";
		Log(LOG_INFO) << "Loading language...";
		game->defaultLanguage();
		Log(LOG_INFO) << "Language loaded successfully.";
		loading = LOADING_SUCCESSFUL;
	}
	catch (std::exception &e)
	{
		error = e.what();
		Log(LOG_ERROR) << error;
		loading = LOADING_FAILED;
	}

	return 0;
}

}
