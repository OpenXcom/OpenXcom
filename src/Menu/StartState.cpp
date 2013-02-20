/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include <SDL.h>
#include "../Engine/Logger.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Surface.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/Language.h"
#include "TestState.h"
#include "NoteState.h"
#include "LanguageState.h"
#include "MainMenuState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Loading screen.
 * @param game Pointer to the core game.
 */
StartState::StartState(Game *game) : State(game), _load(LOADING_NONE)
{
	// Create objects
	_surface = new Surface(320, 200, 0, 0);

	// Set palette
	SDL_Color bnw[3];

	bnw[0].r = 0;
	bnw[0].g = 0;
	bnw[0].b = 0;
	bnw[1].r = 255;
	bnw[1].g = 255;
	bnw[1].b = 255;
	bnw[2].r = 255;
	bnw[2].g = 255;
	bnw[2].b = 0;

	_game->setPalette(bnw, 0, 3);

	add(_surface);

	// Set up objects
	_surface->drawString(120, 96, "Loading...", 1);
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
			Log(LOG_INFO) << "Loading resources...";
			_game->setResourcePack(makeModifications(new XcomResourcePack()));
			Log(LOG_INFO) << "Resources loaded successfully.";
			Log(LOG_INFO) << "Loading ruleset...";
			_game->loadRuleset();
			Log(LOG_INFO) << "Ruleset loaded successfully.";
			std::vector<std::string> langs = Language::getList(0);
			if (langs.empty())
			{
				throw Exception("No languages available");
			}
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
		_load = LOADING_STARTED;
		break;
	case LOADING_SUCCESSFUL:
		Log(LOG_INFO) << "OpenXcom started successfully!";
		if (Options::getString("language") == "" || Options::getString("language") == "~")
		{
			_game->setState(new NoteState(_game));
		}
		else
		{
			try
			{
				_game->loadLanguage(Options::getString("language"));
				_game->setState(new MainMenuState(_game));
			}
			catch (Exception &e)
			{
				_game->setState(new LanguageState(_game));
			}
		}
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
	if (_load == LOADING_FAILED)
	{
		if (action->getDetails()->type == SDL_KEYDOWN)
			_game->quit();
	}
}

/**
 * This method makes our modifications to the XcomResourcePack.
 * @param pack XcomResourcePack which is already read in from files.
 * @return The modified XcomResourcePack.
 */
XcomResourcePack *StartState::makeModifications(XcomResourcePack *pack)
{
	if (_game->getAlienContainmentHasUpperLimit())
	{
		Surface *surface = pack->getSurface("BACK07.SCR");
		for (int y = 172; y >= 152; --y)
			for (int x = 5; x <= 314; ++x)
				surface->setPixel(x, y+4, surface->getPixel(x,y));
		for (int y = 147; y >= 134; --y)
			for (int x = 5; x <= 314; ++x)
				surface->setPixel(x, y+9, surface->getPixel(x,y));
		for (int y = 132; y >= 109; --y)
			for (int x = 5; x <= 314; ++x)
				surface->setPixel(x, y+10, surface->getPixel(x,y));
	}
	return pack;
}

}
