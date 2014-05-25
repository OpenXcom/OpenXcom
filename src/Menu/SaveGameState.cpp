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
#include "SaveGameState.h"
#include <sstream>
#include "../Engine/Logger.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/Text.h"
#include "ErrorMessageState.h"
#include "MainMenuState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Save Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param filename Name of the save file without extension.
 */
	SaveGameState::SaveGameState(Game *game, OptionsOrigin origin, const std::string &filename) : State(game), _origin(origin), _filename(filename), _type(SAVE_DEFAULT)
{
	buildUi();
}

/**
 * Initializes all the elements in the Save Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param type Type of auto-load being used.
 */
SaveGameState::SaveGameState(Game *game, OptionsOrigin origin, SaveType type) : State(game), _origin(origin), _type(type)
{
	switch (type)
	{
	case SAVE_QUICK:
		_filename = SavedGame::QUICKSAVE;
		break;
	case SAVE_AUTO_GEOSCAPE:
		_filename = SavedGame::AUTOSAVE_GEOSCAPE;
		break;
	case SAVE_AUTO_BATTLESCAPE:
		_filename = SavedGame::AUTOSAVE_BATTLESCAPE;
		break;
	case SAVE_IRONMAN:
	case SAVE_IRONMAN_END:
		_filename = CrossPlatform::sanitizeFilename(Language::wstrToFs(_game->getSavedGame()->getName())) + ".sav";
		break;
	default:
		break;
	}

	buildUi();
}

/**
 *
 */
SaveGameState::~SaveGameState()
{

}

/**
 * Builds the interface.
 */
void SaveGameState::buildUi()
{
	_screen = false;

	// Create objects
	_txtStatus = new Text(320, 17, 0, 92);

	// Set palette
	if (_origin == OPT_BATTLESCAPE)
	{
		setPalette("PAL_BATTLESCAPE");
	}
	else
	{
		setPalette("PAL_GEOSCAPE", 6);
	}

	add(_txtStatus);

	centerAllSurfaces();
	// Set up objects
	_txtStatus->setColor(Palette::blockOffset(8) + 5);
	_txtStatus->setBig();
	_txtStatus->setAlign(ALIGN_CENTER);
	_txtStatus->setText(tr("STR_SAVING_GAME"));

	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}
}

/**
 * Saves the current save.
 */
void SaveGameState::init()
{
	// Make sure message is shown (if any)
	State::init();
	blit();
	_game->getScreen()->flip();
	_game->popState();
	
	switch (_type)
	{
	case SAVE_DEFAULT:
		// manual save, close the save screen
		_game->popState();
		if (!_game->getSavedGame()->isIronman())
		{
			// and pause screen too
			_game->popState();
		}
		break;
	case SAVE_QUICK:
	case SAVE_AUTO_GEOSCAPE:
	case SAVE_AUTO_BATTLESCAPE:
		// automatic save, give it a default name
		_game->getSavedGame()->setName(Language::fsToWstr(_filename));
	default:
		break;
	}

	// Save the game
	try
	{
		std::string backup = _filename + ".bak";
		_game->getSavedGame()->save(backup);
		std::string fullPath = Options::getUserFolder() + _filename;
		std::string bakPath = Options::getUserFolder() + backup;
		if (!CrossPlatform::moveFile(bakPath, fullPath))
		{
			throw Exception("Save backed up in " + backup);
		}

		if (_type == SAVE_IRONMAN_END)
		{
			Screen::updateScale(Options::geoscapeScale, Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, true);
			_game->getScreen()->resetDisplay(false);

			_game->setState(new MainMenuState(_game));
			_game->setSavedGame(0);
		}
	}
	catch (Exception &e)
	{
		Log(LOG_ERROR) << e.what();
		std::wostringstream error;
		error << tr("STR_SAVE_UNSUCCESSFUL") << L'\x02' << Language::fsToWstr(e.what());
		if (_origin != OPT_BATTLESCAPE)
			_game->pushState(new ErrorMessageState(_game, error.str(), _palette, Palette::blockOffset(8) + 10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), _palette, Palette::blockOffset(0), "TAC00.SCR", -1));
	}
	catch (YAML::Exception &e)
	{
		Log(LOG_ERROR) << e.what();
		std::wostringstream error;
		error << tr("STR_SAVE_UNSUCCESSFUL") << L'\x02' << Language::fsToWstr(e.what());
		if (_origin != OPT_BATTLESCAPE)
			_game->pushState(new ErrorMessageState(_game, error.str(), _palette, Palette::blockOffset(8) + 10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), _palette, Palette::blockOffset(0), "TAC00.SCR", -1));
	}
}

}
