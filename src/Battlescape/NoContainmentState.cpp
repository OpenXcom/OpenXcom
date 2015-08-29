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
#include "NoContainmentState.h"
#include "../Engine/Game.h"
#include "../Mod/ResourcePack.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the No Containment screen.
 * @param game Pointer to the core game.
 */
NoContainmentState::NoContainmentState()
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(120, 18, 100, 174);
	_txtTitle = new Text(220, 64, 50, 8);

	// Set palette
	setInterface("noContainment");

	add(_window, "window", "noContainment");
	add(_btnOk, "button", "noContainment");
	add(_txtTitle, "text", "noContainment");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NoContainmentState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NoContainmentState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&NoContainmentState::btnOkClick, Options::keyCancel);

	_txtTitle->setText(tr("STR_ALIEN_DIES_NO_ALIEN_CONTAINMENT_FACILITY"));
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setWordWrap(true);
}

/**
 *
 */
NoContainmentState::~NoContainmentState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void NoContainmentState::btnOkClick(Action *)
{
	_game->popState();
}

}
