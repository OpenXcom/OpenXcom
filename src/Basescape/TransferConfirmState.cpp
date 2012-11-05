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
#include "TransferConfirmState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Base.h"
#include "TransferItemsState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Confirm Transfer window.
 * @param game Pointer to the core game.
 * @param base Pointer to the destination base.
 * @param state Pointer to the Transfer state.
 */
TransferConfirmState::TransferConfirmState(Game *game, Base *base, TransferItemsState *state) : State(game), _base(base), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 80, 0, 60);
	_btnCancel = new TextButton(128, 16, 176, 115);
	_btnOk = new TextButton(128, 16, 16, 115);
	_txtTitle = new Text(310, 16, 5, 75);
	_txtCost = new Text(60, 16, 110, 95);
	_txtTotal = new Text(100, 16, 170, 95);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnCancel);
	add(_btnOk);
	add(_txtTitle);
	add(_txtCost);
	add(_txtTotal);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnCancel->setColor(Palette::blockOffset(15)+6);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&TransferConfirmState::btnCancelClick);

	_btnOk->setColor(Palette::blockOffset(15)+6);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&TransferConfirmState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	std::wstring s = _game->getLanguage()->getString("STR_TRANSFER_ITEMS_TO");
	s += _base->getName();
	_txtTitle->setText(s);

	_txtCost->setColor(Palette::blockOffset(13)+10);
	_txtCost->setBig();
	_txtCost->setText(_game->getLanguage()->getString("STR_COST"));

	_txtTotal->setColor(Palette::blockOffset(15)+1);
	_txtTotal->setBig();
	_txtTotal->setText(Text::formatFunding(_state->getTotal()));
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
	_state->completeTransfer();
	_game->popState();
	_game->popState();
	_game->popState();
}

}
