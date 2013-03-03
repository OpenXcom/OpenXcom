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
#include "DeleteGameState.h"
#include "../Engine/CrossPlatform.h"
#include <yaml-cpp/yaml.h>
#include "../Engine/Logger.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/Text.h"
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Resource/ResourcePack.h"
#include "SavedGameState.h"
#include "../Engine/Options.h"
#include "../Engine/Exception.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Confirmation screen.
 * @param game Pointer to the core game.
 * @param msg Text string for the save in question.
 * @param color Color of the UI controls.
 * @param bg Background image.
 * @param bgColor Background color (-1 for Battlescape).
 */
DeleteGameState::DeleteGameState(Game *game, const std::wstring &save, Uint8 color, std::string bg, Uint8 bgColor, SavedGameState *parent): State(game)
{
#ifdef _WIN32
	std::string file = Language::wstrToCp(save);
#else
	std::string file = Language::wstrToUtf8(save);
#endif
	_filename = Options::getUserFolder() + file + ".sav";
	_parent = parent;
	_screen = false;


	// Create objects
	_window = new Window(this, 256, 120, 32, 40, POPUP_BOTH);
	_btnConfirm = new TextButton(60, 18, 90, 112);
	_btnCancel = new TextButton(60, 18, 170, 112);
	_txtMessage = new Text(246, 32, 37, 70);

	// Set palette
	if (bgColor != -1)
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(bgColor)), Palette::backPos, 16);

	add(_window);
	add(_btnConfirm);
	add(_btnCancel);
	add(_txtMessage);

	// Set up objects
	_window->setColor(color);
	_window->setBackground(game->getResourcePack()->getSurface(bg));
	if (color == 0)
		_window->setHighContrast(true);

	_btnConfirm->setColor(color);
	_btnConfirm->setText(_game->getLanguage()->getString("STR_OK"));
	_btnConfirm->onMouseClick((ActionHandler)&DeleteGameState::btnConfirmClick);
	if (color == 0)
		_btnConfirm->setHighContrast(true);

	_btnCancel->setColor(color);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&DeleteGameState::btnCancelClick);
	if (color == 0)
		_btnCancel->setHighContrast(true);
	
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);
	_txtMessage->setColor(color);
	if (color == 0)
		_txtMessage->setHighContrast(true);
	_txtMessage->setText(_game->getLanguage()->getString("STR_IS_IT_OK_TO_DELETE_THE_SAVED_GAME"));
}

/**
 *
 */
DeleteGameState::~DeleteGameState()
{

}

void DeleteGameState::btnCancelClick(Action *)
{
	_game->popState();
}

void DeleteGameState::btnConfirmClick(Action *)
{
	if (!CrossPlatform::deleteFile(_filename))
	{
		throw Exception("Failed to delete save");
	}
	_parent->updateList();
	_game->popState();
}
}
