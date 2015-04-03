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
#include <sstream>
#include "TransferConfirmState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
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
TransferConfirmState::TransferConfirmState(Base *base, TransferItemsState *state) : _base(base), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 80, 0, 60);
	_btnCancel = new TextButton(128, 16, 176, 115);
	_btnOk = new TextButton(128, 16, 16, 115);
	_txtTitle = new Text(310, 17, 5, 75);
	_txtCost = new Text(60, 17, 110, 95);
	_txtTotal = new Text(100, 17, 170, 95);

	// Set palette
	setPalette("PAL_BASESCAPE", _game->getRuleset()->getInterface("transferConfirm")->getElement("palette")->color);

	add(_window, "window", "transferConfirm");
	add(_btnCancel, "button", "transferConfirm");
	add(_btnOk, "button", "transferConfirm");
	add(_txtTitle, "text", "transferConfirm");
	add(_txtCost, "text", "transferConfirm");
	add(_txtTotal, "text", "transferConfirm");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&TransferConfirmState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&TransferConfirmState::btnCancelClick, Options::keyCancel);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&TransferConfirmState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&TransferConfirmState::btnOkClick, Options::keyOk);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_TRANSFER_ITEMS_TO").arg(_base->getName()));

	_txtCost->setBig();
	_txtCost->setText(tr("STR_COST"));

	std::wostringstream ss;
	ss << L'\x01' << Text::formatFunding(_state->getTotal());

	_txtTotal->setBig();
	_txtTotal->setText(ss.str().c_str());
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
void TransferConfirmState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Completes the transfer.
 * @param action Pointer to an action.
 */
void TransferConfirmState::btnOkClick(Action *)
{
	_state->completeTransfer();
	_game->popState();
	_game->popState();
	_game->popState();
}

}
