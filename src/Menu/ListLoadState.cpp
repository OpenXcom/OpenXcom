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
#include "ListLoadState.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Action.h"
#include "../Engine/Palette.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"
#include "ConfirmLoadState.h"
#include "LoadGameState.h"
#include "ListLoadOriginalState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Load Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
ListLoadState::ListLoadState(OptionsOrigin origin) : ListGamesState(origin, 0, true)
{
	// Create objects
	_btnOld = new TextButton(80, 16, 60, 172);

	add(_btnOld, "button", "saveMenus");
	
	// Set up objects
	_txtTitle->setText(tr("STR_SELECT_GAME_TO_LOAD"));

	if (origin != OPT_MENU)
	{
		_btnOld->setVisible(false);
	}
	else
	{
		_btnCancel->setX(180);
	}

	_btnOld->setText(L"Original X-Com");
	_btnOld->onMouseClick((ActionHandler)&ListLoadState::btnOldClick);

	centerAllSurfaces();
}

/**
 *
 */
ListLoadState::~ListLoadState()
{

}

/**
 * Switches to Original X-Com saves.
 * @param action Pointer to an action.
 */
void ListLoadState::btnOldClick(Action *)
{
	_game->pushState(new ListLoadOriginalState);
}

/**
 * Loads the selected save.
 * @param action Pointer to an action.
 */
void ListLoadState::lstSavesPress(Action *action)
{
	ListGamesState::lstSavesPress(action);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		bool confirm = false;
		for (std::vector<std::string>::const_iterator i = _saves[_lstSaves->getSelectedRow()].rulesets.begin(); i != _saves[_lstSaves->getSelectedRow()].rulesets.end(); ++i)
		{
			if (std::find(Options::rulesets.begin(), Options::rulesets.end(), *i) == Options::rulesets.end())
			{
				confirm = true;
				break;
			}
		}
		if (confirm)
		{
			_game->pushState(new ConfirmLoadState(_origin, _saves[_lstSaves->getSelectedRow()].fileName));
		}
		else
		{
			_game->pushState(new LoadGameState(_origin, _saves[_lstSaves->getSelectedRow()].fileName, _palette));
		}
	}
}

}
