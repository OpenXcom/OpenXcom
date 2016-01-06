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
#include "CraftPatrolState.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Target.h"
#include "GeoscapeCraftState.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Craft Patrol window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to display.
 * @param globe Pointer to the Geoscape globe.
 */
CraftPatrolState::CraftPatrolState(Craft *craft, Globe *globe) : _craft(craft), _globe(globe)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 224, 168, 16, 16, POPUP_BOTH);
	_btnOk = new TextButton(140, 12, 58, 144);
	_btnRedirect = new TextButton(140, 12, 58, 160);
	_txtDestination = new Text(224, 64, 16, 48);
	_txtPatrolling = new Text(224, 17, 16, 120);

	// Set palette
	setInterface("geoCraftScreens");

	add(_window, "window", "geoCraftScreens");
	add(_btnOk, "button", "geoCraftScreens");
	add(_btnRedirect, "button", "geoCraftScreens");
	add(_txtDestination, "text1", "geoCraftScreens");
	add(_txtPatrolling, "text1", "geoCraftScreens");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK12.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CraftPatrolState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CraftPatrolState::btnOkClick, Options::keyCancel);

	_btnRedirect->setText(tr("STR_REDIRECT_CRAFT"));
	_btnRedirect->onMouseClick((ActionHandler)&CraftPatrolState::btnRedirectClick);
	_btnRedirect->onKeyboardPress((ActionHandler)&CraftPatrolState::btnRedirectClick, Options::keyOk);

	_txtDestination->setBig();
	_txtDestination->setAlign(ALIGN_CENTER);
	_txtDestination->setWordWrap(true);
	_txtDestination->setText(tr("STR_CRAFT_HAS_REACHED_DESTINATION")
							 .arg(_craft->getName(_game->getLanguage()))
							 .arg(_craft->getDestination()->getName(_game->getLanguage())));

	_txtPatrolling->setBig();
	_txtPatrolling->setAlign(ALIGN_CENTER);
	_txtPatrolling->setText(tr("STR_NOW_PATROLLING"));
}

/**
 *
 */
CraftPatrolState::~CraftPatrolState()
{

}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void CraftPatrolState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Opens up the Craft window.
 * @param action Pointer to an action.
 */
void CraftPatrolState::btnRedirectClick(Action *)
{
	_game->popState();
	_game->pushState(new GeoscapeCraftState(_craft, _globe, 0));
}

}
