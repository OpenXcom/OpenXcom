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
#include "LoadState.h"
#include "../Engine/Logger.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Game.h"
#include "../Engine/Exception.h"
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
 * @param origin Game section that originated this state.
 */
LoadState::LoadState(Game *game, OptionsOrigin origin) : SavedGameState(game, origin, 0)
{
	// Set up objects
	_txtTitle->setText(tr("STR_SELECT_GAME_TO_LOAD"));
	_lstSaves->onMousePress((ActionHandler)&LoadState::lstSavesPress);
}

/**
 * Creates the Quick Load Game state.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param showMsg True if need to show messages like "Loading game" or "Saving game".
 */
LoadState::LoadState(Game *game, OptionsOrigin origin, bool showMsg) : SavedGameState(game, origin, 0,showMsg)
{
	quickLoad("autosave");
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
		quickLoad(CrossPlatform::noExt(_saves[_lstSaves->getSelectedRow()]));
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		_game->pushState(new DeleteGameState(_game, _origin, _lstSaves->getCellText(_lstSaves->getSelectedRow(), 0), this));
	}
}

/**
 * Quick load game.
 * @param filename name of file without ".sav"
 */
void LoadState::quickLoad(const std::string &filename)
{
	if (_showMsg) updateStatus("STR_LOADING_GAME");

	SavedGame *s = new SavedGame();
	try
	{
		s->load(filename, _game->getRuleset());
		_game->setSavedGame(s);
		_game->setState(new GeoscapeState(_game));
		if (_game->getSavedGame()->getSavedBattle() != 0)
		{
			_game->getSavedGame()->getSavedBattle()->loadMapResources(_game);
			BattlescapeState *bs = new BattlescapeState(_game);
			_game->pushState(bs);
			_game->getSavedGame()->getSavedBattle()->setBattleState(bs);
		}
	}
	catch (Exception &e)
	{
		Log(LOG_ERROR) << e.what();
		std::wstringstream error;
		error << tr("STR_LOAD_UNSUCCESSFUL") << L'\x02' << Language::fsToWstr(e.what());
		if (_origin != OPT_BATTLESCAPE)
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(8)+10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(0), "TAC00.SCR", -1));

		if (_game->getSavedGame() == s)
			_game->setSavedGame(0);
		else
			delete s;
	}
	catch (YAML::Exception &e)
	{
		Log(LOG_ERROR) << e.what();
		std::wstringstream error;
		error << tr("STR_LOAD_UNSUCCESSFUL") << L'\x02' << Language::fsToWstr(e.what());
		if (_origin != OPT_BATTLESCAPE)
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(8)+10, "BACK01.SCR", 6));
		else
			_game->pushState(new ErrorMessageState(_game, error.str(), Palette::blockOffset(0), "TAC00.SCR", -1));

		if (_game->getSavedGame() == s)
			_game->setSavedGame(0);
		else
			delete s;
	}
}

}
