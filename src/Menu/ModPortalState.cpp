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
#include "ModPortalState.h"
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
#include "StartState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Mod.io window.
 * @param game Pointer to the core game.
 */
ModPortalState::ModPortalState()
{
	_screen = false;

	// Create objects
	_window = new Window(this, 192, 140, 64, 30, POPUP_BOTH);
	_logo = new Surface(160, 50, 64, 38);
	_txtLogin = new Text(100, 9, 80, 62);
	_txtStatus = new Text(160, 9, 80, 82);
	_txtInfo = new Text(160, 16, 80, 118);

	_btnLogout = new TextButton(60, 16, 180, 58);
	_btnUpdate = new TextButton(160, 16, 80, 96);

	_btnOk = new TextButton(78, 16, 80, 144);
	_btnWebsite = new TextButton(78, 16, 162, 144);

	// Set palette
	setInterface("modsWindow");

	add(_window, "window", "modsWindow");
	add(_logo);
	add(_txtLogin, "text", "modsWindow");
	add(_txtStatus, "text", "modsWindow");
	add(_txtInfo, "text", "modsWindow");
	add(_btnLogout, "button1", "modsWindow");
	add(_btnUpdate, "button1", "modsWindow");
	add(_btnOk, "button2", "modsWindow");
	add(_btnWebsite, "button2", "modsWindow");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_game->getMod()->getSurface("modio")->blit(_logo);

	_txtInfo->setText("Manage your subscriptions from the mod.io website.");
	_txtInfo->setWordWrap(true);

	_btnLogout->setText("Logout");
	_btnLogout->onMouseClick((ActionHandler)&ModPortalState::btnLogoutClick);

	_btnUpdate->setText("Update Now");
	_btnUpdate->onMouseClick((ActionHandler)&ModPortalState::btnUpdateClick);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ModPortalState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ModPortalState::btnOkClick, Options::keyCancel);

	_btnWebsite->setText("Website");
	_btnWebsite->onMouseClick((ActionHandler)&ModPortalState::btnWebsiteClick);
}

ModPortalState::~ModPortalState()
{
}

void ModPortalState::think()
{
	State::think();

	_txtLogin->setText(ModPortal::getUsername());
	_txtStatus->setText(ModPortal::getStatus());
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void ModPortalState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Logout from mod.io.
 * @param action Pointer to an action.
 */
void ModPortalState::btnLogoutClick(Action *)
{
	ModPortal::logout();
	_game->popState();
}

/**
 * Restart to update mods.
 * @param action Pointer to an action.
 */
void ModPortalState::btnUpdateClick(Action *)
{
	Options::reload = true;
	_game->setState(new StartState);
}

/**
 * Opens the mod.io website.
 * @param action Pointer to an action.
 */
void ModPortalState::btnWebsiteClick(Action *)
{
	CrossPlatform::openExplorer("https://openxcom.mod.io");
}

}
