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
#include "StartState.h"
#include <SDL_mixer.h>
#include "../Engine/Logger.h"
#include "../Engine/Game.h"
#include "../Engine/Screen.h"
#include "../Engine/Action.h"
#include "../Engine/Surface.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/Language.h"
#include "../Ruleset/Ruleset.h"
#include "../Interface/FpsCounter.h"
#include "../Interface/Cursor.h"
#include "../Resource/XcomResourcePack.h"
#include "MainMenuState.h"
#include "IntroState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Loading screen.
 * @param game Pointer to the core game.
 */
StartState::StartState(Game *game) : State(game), _load(LOADING_NONE)
{
	// Create objects
	int dx = (Options::baseXResolution - 320) / 2;
	int dy = (Options::baseYResolution - 200) / 2;
	_wasLetterBoxed = Options::keepAspectRatio;


	if (!Options::useOpenGL)
	{
		Options::keepAspectRatio = false;
		game->getScreen()->resetDisplay(false);
	}

	_surface = new Surface(320, 200, dx, dy);

	// Set palette (set to {0} here to ensure all fields are initialized)
	SDL_Color bnw[3] = { {0} };

	bnw[0].r = 0;
	bnw[0].g = 0;
	bnw[0].b = 0;
	bnw[1].r = 255;
	bnw[1].g = 255;
	bnw[1].b = 255;
	bnw[2].r = 255;
	bnw[2].g = 255;
	bnw[2].b = 0;

	setPalette(bnw, 0, 3);

	add(_surface);

	// Set up objects
	_surface->drawString(120, 96, "Loading...", 1);

	_game->getCursor()->setVisible(false);
	_game->getFpsCounter()->setVisible(false);
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
			Log(LOG_INFO) << "Loading ruleset...";
			_game->loadRuleset();
			Log(LOG_INFO) << "Ruleset loaded successfully.";
			Log(LOG_INFO) << "Loading resources...";
			_game->setResourcePack(new XcomResourcePack(_game->getRuleset()->getExtraSprites(), _game->getRuleset()->getExtraSounds()));
			Log(LOG_INFO) << "Resources loaded successfully.";
			Log(LOG_INFO) << "Loading language...";
			_game->defaultLanguage();
			Log(LOG_INFO) << "Language loaded successfully.";
			_load = LOADING_SUCCESSFUL;
		}
		catch (Exception &e)
		{
			_load = LOADING_FAILED;
			_surface->clear();
			_surface->drawString(1, 9, "ERROR:", 2);
			_surface->drawString(1, 17, e.what(), 2);
			_surface->drawString(1, 49, "Make sure you installed OpenXcom", 1);
			_surface->drawString(1, 57, "correctly.", 1);
			_surface->drawString(1, 73, "Check the requirements and", 1);
			_surface->drawString(1, 81, "documentation for more details.", 1);
			_surface->drawString(75, 183, "Press any key to quit", 1);
			Log(LOG_ERROR) << e.what();
		}
		break;
	case LOADING_NONE:
		Mix_HaltChannel(-1);
		Mix_HaltMusic();
		_game->getScreen()->clear();
		blit();
		_load = LOADING_STARTED;
		break;
	case LOADING_SUCCESSFUL:
		Log(LOG_INFO) << "OpenXcom started successfully!";
		if (!Options::reload && Options::playIntro)
		{
			Options::keepAspectRatio = true;
			_game->getScreen()->resetDisplay(false);
			_game->setState(new IntroState(_game, _wasLetterBoxed));
		}
		else
		{
			Options::keepAspectRatio = _wasLetterBoxed;
			_game->setState(new MainMenuState(_game));
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
	if (_load == LOADING_FAILED)
	{
		if (action->getDetails()->type == SDL_KEYDOWN)
			_game->quit();
	}
}

}
