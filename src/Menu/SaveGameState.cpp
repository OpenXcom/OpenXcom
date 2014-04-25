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
#include "../Savegame/SavedGame.h"
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

namespace OpenXcom
{

/**
 * Initializes all the elements in the Save Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param filename Name of the save file without extension.
 * @param showMsg Show a message while saving the game.
 */
SaveGameState::SaveGameState(Game *game, OptionsOrigin origin, const std::string &filename, bool showMsg) : State(game), _origin(origin), _filename(filename)
{
	_screen = false;

	if (showMsg)
	{
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

		// Set up objects
		_txtStatus->setColor(Palette::blockOffset(8)+5);
		_txtStatus->setBig();
		_txtStatus->setAlign(ALIGN_CENTER);
		_txtStatus->setText(tr("STR_SAVING_GAME"));
		
		if (_origin == OPT_BATTLESCAPE)
		{
			applyBattlescapeTheme();
		}
	}
}

/**
 *
 */
SaveGameState::~SaveGameState()
{

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
	_game->popState(); // only if manual save

	// Save the game
	_filename += ".sav";
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
