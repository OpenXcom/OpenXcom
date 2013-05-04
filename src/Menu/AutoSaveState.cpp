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
#include "AutoSaveState.h"
#include <yaml-cpp/yaml.h>
#include "../Engine/Logger.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Game.h"
#include "../Engine/Exception.h"
#include "../Engine/Language.h"
#include "../Interface/Text.h"
#include "ErrorMessageState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Save Game screen.
 * @param game Pointer to the core game.
 * @param geo True to use Geoscape palette, false to use Battlescape palette.
 * @param load True for load without UI, False (default) for save game.
 */
AutoSaveState::AutoSaveState(Game *game, bool geo, bool load) : State(game), _geo(geo), _needToLoad(load)
{
	_txtStatus = new Text(320, 16, 0, 92);
	add(_txtStatus);
	_txtStatus->setColor(Palette::blockOffset(8)+5);
	_txtStatus->setBig();
	_txtStatus->setAlign(ALIGN_CENTER);
}

/**
 *
 */
AutoSaveState::~AutoSaveState()
{

}

/**
 * init AutoSaveState.
 */
void AutoSaveState::init()
{
	if (_needToLoad)
		quickLoad();
	else
		quickSave();
}

/**
 * Quick save game.
 */
void AutoSaveState::quickSave()
{
#ifdef _WIN32
		std::string filename = Language::wstrToCp(L"autosave");
#else
		std::string filename = Language::wstrToUtf8(L"autosave");
#endif

	try
	{
		_game->getSavedGame()->save(filename);
		_game->popState();
	}
	catch (Exception &e)
	{
		_game->popState();
		Log(LOG_ERROR) << e.what();
		std::wstringstream error;
		error << _game->getLanguage()->getString("STR_SAVE_UNSUCCESSFUL") << L'\x02' << Language::utf8ToWstr(e.what());
		if (_geo)
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(8)+10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(0), "TAC00.SCR", -1));
	}
	catch (YAML::Exception &e)
	{
		_game->popState();
		Log(LOG_ERROR) << e.what();
		std::wstringstream error;
		error << _game->getLanguage()->getString("STR_SAVE_UNSUCCESSFUL") << L'\x02' << Language::utf8ToWstr(e.what());
		if (_geo)
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(8)+10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(0), "TAC00.SCR", -1));
	}
}

/**
 * Quick load game.
 */
void AutoSaveState::quickLoad()
{
//	updateStatus("STR_LOADING_GAME");
	_txtStatus->setText(_game->getLanguage()->getString("STR_LOADING_GAME"));
	blit();
	_game->getScreen()->flip();

#ifdef _WIN32
		std::string filename = Language::wstrToCp(L"autosave");
#else
		std::string filename = Language::wstrToUtf8(L"autosave");
#endif

	SavedGame *s = new SavedGame();
	try
	{
		s->load(filename, _game->getRuleset());
		_game->setSavedGame(s);
		_game->setState(new GeoscapeState(_game));
		if (_game->getSavedGame()->getBattleGame() != 0)
		{
			_game->getSavedGame()->getBattleGame()->loadMapResources(_game->getResourcePack());
			BattlescapeState *bs = new BattlescapeState(_game);
			_game->pushState(bs);
			_game->getSavedGame()->getBattleGame()->setBattleState(bs);
		}
	}
	catch (Exception &e)
	{
		Log(LOG_ERROR) << e.what();
		std::wstringstream error;
		error << _game->getLanguage()->getString("STR_LOAD_UNSUCCESSFUL") << L'\x02' << Language::utf8ToWstr(e.what());
		if (_geo)
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(8)+10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(0), "TAC00.SCR", -1));
		delete s;
		_game->setSavedGame(0);
	}
	catch (YAML::Exception &e)
	{
		Log(LOG_ERROR) << e.what();
		std::wstringstream error;
		error << _game->getLanguage()->getString("STR_LOAD_UNSUCCESSFUL") << L'\x02' << Language::utf8ToWstr(e.what());
		if (_geo)
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(8)+10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(0), "TAC00.SCR", -1));
		delete s;
		_game->setSavedGame(0);
	}
}

}
