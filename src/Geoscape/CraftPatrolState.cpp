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
#include "CraftPatrolState.h"
#include <string>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Target.h"
#include "GeoscapeCraftState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Craft Patrol window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to display.
 * @param globe Pointer to the Geoscape globe.
 */
CraftPatrolState::CraftPatrolState(Game *game, Craft *craft, Globe *globe) : State(game), _craft(craft), _globe(globe)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 224, 168, 16, 16, POPUP_BOTH);
	_btnOk = new TextButton(140, 12, 58, 144);
	_btnRedirect = new TextButton(140, 12, 58, 160);
	_txtDestination = new Text(224, 64, 16, 48);
	_txtPatrolling = new Text(224, 16, 16, 120);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnRedirect);
	add(_txtDestination);
	add(_txtPatrolling);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CraftPatrolState::btnOkClick);

	_btnRedirect->setColor(Palette::blockOffset(8)+5);
	_btnRedirect->setText(_game->getLanguage()->getString("STR_REDIRECT_CRAFT"));
	_btnRedirect->onMouseClick((ActionHandler)&CraftPatrolState::btnRedirectClick);

	_txtDestination->setColor(Palette::blockOffset(15)-1);
	_txtDestination->setBig();
	_txtDestination->setAlign(ALIGN_CENTER);
	_txtDestination->setWordWrap(true);
	std::wstring s = _craft->getName(_game->getLanguage()) + L'\n';
	s += _game->getLanguage()->getString("STR_HAS_REACHED") + L'\n';
	s += _game->getLanguage()->getString("STR_DESTINATION") + L'\n';
	s += _craft->getDestination()->getName(_game->getLanguage());
	_txtDestination->setText(s);

	_txtPatrolling->setColor(Palette::blockOffset(15)-1);
	_txtPatrolling->setBig();
	_txtPatrolling->setAlign(ALIGN_CENTER);
	_txtPatrolling->setText(_game->getLanguage()->getString("STR_NOW_PATROLLING"));
}

/**
 *
 */
CraftPatrolState::~CraftPatrolState()
{

}

/**
 * Resets the palette.
 */
void CraftPatrolState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void CraftPatrolState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Opens up the Craft window.
 * @param action Pointer to an action.
 */
void CraftPatrolState::btnRedirectClick(Action *action)
{
	_game->popState();
	_game->pushState(new GeoscapeCraftState(_game, _craft, _globe, 0));
}

}
