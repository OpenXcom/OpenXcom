/*
 * Copyright 2010 OpenXcom Developers.
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
#include "TransferConfirmState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/SavedGame.h"
#include "TransferItemsState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Confirm Transfer window.
 * @param game Pointer to the core game.
 * @param state Pointer to the Transfer state.
 */
TransferConfirmState::TransferConfirmState(Game *game, TransferItemsState *state) : State(game), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 80, 0, 60);
	_btnCancel = new TextButton(264, 16, 28, 146);
	_btnOk = new TextButton(264, 16, 28, 146);
	_txtTitle = new Text(270, 16, 25, 38);
	_txtCost = new Text(250, 9, 30, 54);
	_txtTotal = new Text(130, 16, 28, 64);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnCancel);
	add(_btnOk);
	add(_txtTitle);
	add(_txtCost);
	add(_txtTotal);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnCancel->setColor(Palette::blockOffset(13)+8);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&TransferConfirmState::btnCancelClick);

	_btnOk->setColor(Palette::blockOffset(13)+8);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&TransferConfirmState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(13)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_DESTINATION_BASE"));

	_txtCost->setColor(Palette::blockOffset(13)+5);
	_txtCost->setSecondaryColor(Palette::blockOffset(13));
	std::wstring s = _game->getLanguage()->getString("STR_CURRENT_FUNDS");
	s += L'\x01' + Text::formatFunding(_game->getSavedGame()->getFunds());
	_txtCost->setText(s);

	_txtTotal->setColor(Palette::blockOffset(13)+5);
	_txtTotal->setText(_game->getLanguage()->getString("STR_NAME"));
	_txtTotal->setBig();
}

/**
 *
 */
TransferConfirmState::~TransferConfirmState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void TransferConfirmState::btnCancelClick(Action *action)
{
	_game->popState();
}

/**
 * Completes the transfer.
 * @param action Pointer to an action.
 */
void TransferConfirmState::btnOkClick(Action *action)
{
	;
}

}
