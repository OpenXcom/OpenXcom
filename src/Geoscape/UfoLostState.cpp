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
#include "UfoLostState.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Ufo Lost window.
 * @param game Pointer to the core game.
 * @param id Name of the UFO.
 */
UfoLostState::UfoLostState(const std::string &id) : _id(id)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 192, 104, 32, 48, POPUP_BOTH);
	_btnOk = new TextButton(60, 12, 98, 112);
	_txtTitle = new Text(160, 32, 48, 72);

	// Set palette
	setInterface("UFOLost");

	add(_window, "window", "UFOLost");
	add(_btnOk, "button", "UFOLost");
	add(_txtTitle, "text", "UFOLost");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK15.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&UfoLostState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&UfoLostState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&UfoLostState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	std::string s = _id;
	s += '\n';
	s += tr("STR_TRACKING_LOST");
	_txtTitle->setText(s);
}

/**
 *
 */
UfoLostState::~UfoLostState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void UfoLostState::btnOkClick(Action *)
{
	_game->popState();
}

}
