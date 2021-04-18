/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "ModPortalLoginState.h"
#include "../Engine/Game.h"
#include "../Engine/CrossPlatform.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Window.h"
#include "../Interface/TextEdit.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Engine/Options.h"
#include "../Engine/Action.h"
#include "../Engine/ModPortal.h"
#include "ModPortalState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Mod.io window.
 * @param game Pointer to the core game.
 */
ModPortalLoginState::ModPortalLoginState()
{
	_screen = false;

	// Create objects
	_window = new Window(this, 220, 88, 50, 56, POPUP_BOTH);
	_txtTitle = new Text(210, 16, 55, 64);
	_txtStatus = new Text(210, 16, 55, 84);

	_edtLogin = new TextEdit(this, 210, 9, 55, 102);

	_btnOk = new TextButton(78, 16, 80, 120);
	_btnCancel = new TextButton(78, 16, 162, 120);

	// Set palette
	setInterface("modsWindow");

	add(_window, "window", "modsWindow");
	add(_txtTitle, "heading", "modsWindow");
	add(_txtStatus, "text", "modsWindow");
	add(_edtLogin, "text", "modsWindow");
	add(_btnOk, "button1", "modsWindow");
	add(_btnCancel, "button1", "modsWindow");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_txtTitle->setText("mod.io");
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtStatus->setText("Enter your email address to login:");
	_txtStatus->setAlign(ALIGN_CENTER);
	_txtStatus->setWordWrap(true);

	_edtLogin->setText("<here>");
	_edtLogin->setAlign(ALIGN_CENTER);
	_edtLogin->onMouseClick((ActionHandler)&ModPortalLoginState::edtLoginClick);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ModPortalLoginState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ModPortalLoginState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&ModPortalLoginState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ModPortalLoginState::btnCancelClick, Options::keyCancel);

	ModPortal::logout();
}

ModPortalLoginState::~ModPortalLoginState()
{
}

void ModPortalLoginState::think()
{
	State::think();

	ModPortalLogin status = ModPortal::getLoginStatus();
	switch (status)
	{
	case LOGIN_ERROR:
		_txtStatus->setText("Login failed. Try again later.");
		break;
	case LOGIN_PENDING:
		_txtStatus->setText("Logging in...");
		break;
	case LOGIN_CODE:
		_txtStatus->setText("Enter the code sent to your email address:");
		_btnOk->setVisible(true);
		_edtLogin->setVisible(true);
		break;
	case LOGIN_SUCCESS:
		_game->popState();
		_game->pushState(new ModPortalState);
		break;
	}
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void ModPortalLoginState::btnOkClick(Action *)
{
	_btnOk->setVisible(false);
	_edtLogin->setVisible(false);
	ModPortal::login(_edtLogin->getText());
	_edtLogin->setText("<here>");
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void ModPortalLoginState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Clears the text.
 * @param action Pointer to an action.
 */
void ModPortalLoginState::edtLoginClick(Action *)
{
	_edtLogin->setText("");
}

}
