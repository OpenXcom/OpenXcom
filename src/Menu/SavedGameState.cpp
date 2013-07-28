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
#include "SavedGameState.h"
#include "../Engine/Logger.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Game.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Saved Game screen.
 * @param game Pointer to the core game.
 * @param geo True to use Geoscape palette, false to use Battlescape palette.
 */
SavedGameState::SavedGameState(Game *game, bool geo) : State(game), _geo(geo), _showMsg(true), _noUI(false)
{
	_screen = false;
	// Create objects
	WindowPopup p = POPUP_BOTH;
	if (!geo)
		p = POPUP_NONE;
	_window = new Window(this, 320, 200, 0, 0, p);
	_btnCancel = new TextButton(80, 16, 120, 172);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtDelete = new Text(310, 8, 5, 24);
	_txtName = new Text(150, 9, 16, 32);
	_txtTime = new Text(30, 9, 184, 32);
	_txtDate = new Text(38, 9, 214, 32);
	_txtStatus = new Text(320, 16, 0, 92);
	_lstSaves = new TextList(288, 120, 8, 40);

	// Set palette
	if (_geo)
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
	}

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtDelete);
	add(_txtName);
	add(_txtTime);
	add(_txtDate);
	add(_lstSaves);
	add(_txtStatus);

	centerAllSurfaces();

	// Set up objects
	if (_geo)
	{
		_window->setColor(Palette::blockOffset(8)+5);
		_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));

		_btnCancel->setColor(Palette::blockOffset(8)+5);

		_txtTitle->setColor(Palette::blockOffset(15)-1);

		_txtDelete->setColor(Palette::blockOffset(15)-1);

		_txtName->setColor(Palette::blockOffset(15)-1);

		_txtTime->setColor(Palette::blockOffset(15)-1);

		_txtDate->setColor(Palette::blockOffset(15)-1);

		_txtStatus->setColor(Palette::blockOffset(8)+5);

		_lstSaves->setColor(Palette::blockOffset(8)+10);
		_lstSaves->setArrowColor(Palette::blockOffset(8)+5);
	}
	else
	{
		_window->setColor(Palette::blockOffset(0));
		_window->setHighContrast(true);
		_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));

		_btnCancel->setColor(Palette::blockOffset(0));
		_btnCancel->setHighContrast(true);

		_txtTitle->setColor(Palette::blockOffset(0));
		_txtTitle->setHighContrast(true);

		_txtDelete->setColor(Palette::blockOffset(0));
		_txtDelete->setHighContrast(true);

		_txtName->setColor(Palette::blockOffset(0));
		_txtName->setHighContrast(true);

		_txtTime->setColor(Palette::blockOffset(0));
		_txtTime->setHighContrast(true);

		_txtDate->setColor(Palette::blockOffset(0));
		_txtDate->setHighContrast(true);

		_txtStatus->setColor(Palette::blockOffset(0));
		_txtStatus->setHighContrast(true);

		_lstSaves->setColor(Palette::blockOffset(0));
		_lstSaves->setHighContrast(true);
	}

	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&SavedGameState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&SavedGameState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	
	_txtDelete->setAlign(ALIGN_CENTER);
	_txtDelete->setText(_game->getLanguage()->getString("STR_RIGHT_CLICK_TO_DELETE"));

	_txtName->setText(_game->getLanguage()->getString("STR_NAME"));

	_txtTime->setText(_game->getLanguage()->getString("STR_TIME"));

	_txtDate->setText(_game->getLanguage()->getString("STR_DATE"));

	_txtStatus->setBig();
	_txtStatus->setAlign(ALIGN_CENTER);

	_lstSaves->setColumns(5, 168, 30, 30, 30, 30);
	_lstSaves->setSelectable(true);
	_lstSaves->setBackground(_window);
	_lstSaves->setMargin(8);
}

/**
 * Initializes all the elements in the Saved Game screen.
 * @param game Pointer to the core game.
 * @param geo True to use Geoscape palette, false to use Battlescape palette.
 * @param showMsg True if need to show messages like "Loading game" or "Saving game".
 */
SavedGameState::SavedGameState(Game *game, bool geo, bool showMsg) : State(game), _geo(geo), _showMsg(showMsg), _noUI(true)
{
	if (_showMsg)
	{
		_txtStatus = new Text(320, 16, 0, 92);
		add(_txtStatus);

		_txtStatus->setBig();
		_txtStatus->setAlign(ALIGN_CENTER);
		if (_geo)
			_txtStatus->setColor(Palette::blockOffset(8)+5);
		else
		{
			_txtStatus->setColor(Palette::blockOffset(5));
			_txtStatus->setHighContrast(true);
		}
	}
}

/**
 *
 */
SavedGameState::~SavedGameState()
{

}

/**
 * Resets the palette and refreshes saves.
 */
void SavedGameState::init()
{
	if (_noUI)
	{
		_game->popState();
		return;
	}

	_txtStatus->setText(L"");

	if (_geo)
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
	}

	try
	{
		updateList();
	}
	catch (Exception &e)
	{
		Log(LOG_ERROR) << e.what();
	}
}

/**
 * Updates the save game list with a current list
 * of available savegames.
 */
void SavedGameState::updateList()
{
	_lstSaves->clearList();
	SavedGame::getList(_lstSaves, _game->getLanguage());
	_lstSaves->draw();
}

/**
 * Updates the status message in the center of the screen.
 * @param msg New message ID.
 */
void SavedGameState::updateStatus(const std::string &msg)
{
	_txtStatus->setText(_game->getLanguage()->getString(msg));
	blit();
	_game->getScreen()->flip();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SavedGameState::btnCancelClick(Action *)
{
	_game->popState();
}

}
