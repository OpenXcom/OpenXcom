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
#include "LoadState.h"
#include <yaml-cpp/yaml.h>
#include "../Engine/Logger.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/Game.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Action.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Geoscape/GeoscapeState.h"
#include "ErrorMessageState.h"
#include "../Battlescape/BattlescapeState.h"
#include "DeleteGameState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Load Game screen.
 * @param game Pointer to the core game.
 * @param geo True to use Geoscape palette, false to use Battlescape palette.
 */
LoadState::LoadState(Game *game, bool geo) : SavedGameState(game, geo)
{
	_geo = geo;
	// Set up objects
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_GAME_TO_LOAD"));
	
	_lstSaves->onMousePress((ActionHandler)&LoadState::lstSavesPress);
}

/**
 *
 */
LoadState::~LoadState()
{

}

/**
 * Loads the selected save.
 * @param action Pointer to an action.
 */
void LoadState::lstSavesPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		updateStatus("STR_LOADING_GAME");
		SavedGame *s = new SavedGame();
		try
		{
#ifdef _WIN32
			std::string filename = Language::wstrToCp(_lstSaves->getCellText(_lstSaves->getSelectedRow(), 0));
#else
			std::string filename = Language::wstrToUtf8(_lstSaves->getCellText(_lstSaves->getSelectedRow(), 0));
#endif
			s->load(filename, _game->getRuleset());
			_game->setSavedGame(s);
			_game->setState(new GeoscapeState(_game));
			if (_game->getSavedGame()->getBattleGame() != 0)
			{
				_game->getSavedGame()->getBattleGame()->loadMapResources(_game->getResourcePack());
				_game->pushState(new BattlescapeState(_game));
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
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		if(_geo)
			_game->pushState(new DeleteGameState(_game, _lstSaves->getCellText(_lstSaves->getSelectedRow(),0), Palette::blockOffset(8)+10, "BACK01.SCR", 6, this));
		else
			_game->pushState(new DeleteGameState(_game, _lstSaves->getCellText(_lstSaves->getSelectedRow(),0), Palette::blockOffset(0), "TAC00.SCR", -1, this));
	}
}

}
