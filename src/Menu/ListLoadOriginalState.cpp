/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "ListLoadOriginalState.h"
#include <sstream>
#include "../Engine/Logger.h"
#include "../Savegame/SaveConverter.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/Game.h"
#include "../Engine/Screen.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Geoscape/GeoscapeState.h"
#include "../Battlescape/BattlescapeState.h"
#include "ErrorMessageState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Saved Game screen.
 * @param game Pointer to the core game.
 */
ListLoadOriginalState::ListLoadOriginalState()
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnNew = new TextButton(80, 16, 60, 172);
	_btnCancel = new TextButton(80, 16, 180, 172);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtName = new Text(160, 9, 36, 24);
	_txtTime = new Text(30, 9, 195, 24);
	_txtDate = new Text(90, 9, 225, 24);

	// Set palette
	setPalette("PAL_GEOSCAPE", 6);

	add(_window);
	add(_btnNew);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtName);
	add(_txtTime);
	add(_txtDate);

	int y = 34;
	for (int i = 0; i < SaveConverter::NUM_SAVES; ++i)
	{
		_btnSlot[i] = new TextButton(24, 12, 10, y-2);
		_txtSlotName[i] = new Text(160, 9, 36, y);
		_txtSlotTime[i] = new Text(30, 9, 195, y);
		_txtSlotDate[i] = new Text(90, 9, 225, y);

		add(_btnSlot[i]);
		add(_txtSlotName[i]);
		add(_txtSlotTime[i]);
		add(_txtSlotDate[i]);

		y += 14;
	}

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnNew->setColor(Palette::blockOffset(8)+5);
	_btnNew->setText(tr("STR_OPENXCOM"));
	_btnNew->onMouseClick((ActionHandler)&ListLoadOriginalState::btnNewClick);
	_btnNew->onKeyboardPress((ActionHandler)&ListLoadOriginalState::btnNewClick, Options::keyCancel);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&ListLoadOriginalState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ListLoadOriginalState::btnCancelClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_SELECT_GAME_TO_LOAD"));

	_txtName->setColor(Palette::blockOffset(15)-1);
	_txtName->setText(tr("STR_NAME"));

	_txtTime->setColor(Palette::blockOffset(15)-1);
	_txtTime->setText(tr("STR_TIME"));

	_txtDate->setColor(Palette::blockOffset(15)-1);
	_txtDate->setText(tr("STR_DATE"));

	std::wstring dots(80, '.');
	SaveConverter::getList(_game->getLanguage(), _saves);
	for (int i = 0; i < SaveConverter::NUM_SAVES; ++i)
	{
		std::wstringstream ss;
		ss << (i + 1);
		_btnSlot[i]->setColor(Palette::blockOffset(8) + 5);
		_btnSlot[i]->setText(ss.str());
		_btnSlot[i]->onMouseClick((ActionHandler)&ListLoadOriginalState::btnSlotClick);

		_txtSlotName[i]->setColor(Palette::blockOffset(8)+10);
		_txtSlotTime[i]->setColor(Palette::blockOffset(8)+10);
		_txtSlotDate[i]->setColor(Palette::blockOffset(8)+10);

		_txtSlotName[i]->setText(_saves[i].name + dots);
		_txtSlotTime[i]->setText(_saves[i].time);
		_txtSlotDate[i]->setText(_saves[i].date);
	}
}

/**
 *
 */
ListLoadOriginalState::~ListLoadOriginalState()
{

}

/**
 * Switches to OpenXcom saves.
 * @param action Pointer to an action.
 */
void ListLoadOriginalState::btnNewClick(Action *)
{
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ListLoadOriginalState::btnCancelClick(Action *)
{
	_game->popState();
	_game->popState();
}

/**
 * Loads the specified save.
 * @param action Pointer to an action.
 */
void ListLoadOriginalState::btnSlotClick(Action *action)
{
	int n = 0;
	for (int i = 0; i < SaveConverter::NUM_SAVES; ++i)
	{
		if (action->getSender() == _btnSlot[i])
		{
			n = i;
			break;
		}
	}
	if (_saves[n].id > 0)
	{
		if (_saves[n].tactical)
		{
			std::wostringstream error;
			error << tr("STR_LOAD_UNSUCCESSFUL") << L'\x02' << L"Battlescape saves aren't supported yet.";
			_game->pushState(new ErrorMessageState(error.str(), _palette, Palette::blockOffset(8) + 10, "BACK01.SCR", 6));
		}
		else
		{
			SaveConverter converter(_saves[n].id, _game->getRuleset());
			_game->setSavedGame(converter.loadXcom1());
			Options::baseXResolution = Options::baseXGeoscape;
			Options::baseYResolution = Options::baseYGeoscape;
			_game->getScreen()->resetDisplay(false);
			_game->setState(new GeoscapeState);
			if (_game->getSavedGame()->getSavedBattle() != 0)
			{
				_game->getSavedGame()->getSavedBattle()->loadMapResources(_game);
				Options::baseXResolution = Options::baseXBattlescape;
				Options::baseYResolution = Options::baseYBattlescape;
				_game->getScreen()->resetDisplay(false);
				BattlescapeState *bs = new BattlescapeState;
				_game->pushState(bs);
				_game->getSavedGame()->getSavedBattle()->setBattleState(bs);
			}
		}
	}
}

}
