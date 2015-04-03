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
#include "LowFuelState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Craft.h"
#include "GeoscapeState.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Low Fuel window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to display.
 * @param state Pointer to the Geoscape.
 */
LowFuelState::LowFuelState(Craft *craft, GeoscapeState *state) : _craft(craft), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 224, 120, 16, 40, POPUP_BOTH);
	_btnOk = new TextButton(90, 18, 30, 120);
	_btnOk5Secs = new TextButton(90, 18, 136, 120);
	_txtTitle = new Text(214, 17, 21, 60);
	_txtMessage = new Text(214, 17, 21, 90);

	// Set palette
	setPalette("PAL_GEOSCAPE", _game->getRuleset()->getInterface("lowFuel")->getElement("palette")->color);

	add(_window, "window", "lowFuel");
	add(_btnOk, "button", "lowFuel");
	add(_btnOk5Secs, "button", "lowFuel");
	add(_txtTitle, "text", "lowFuel");
	add(_txtMessage, "text", "lowFuel");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&LowFuelState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&LowFuelState::btnOkClick, Options::keyCancel);

	_btnOk5Secs->setText(tr("STR_OK_5_SECONDS"));
	_btnOk5Secs->onMouseClick((ActionHandler)&LowFuelState::btnOk5SecsClick);
	_btnOk5Secs->onKeyboardPress((ActionHandler)&LowFuelState::btnOk5SecsClick, Options::keyOk);

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_craft->getName(_game->getLanguage()));

	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setText(tr("STR_IS_LOW_ON_FUEL_RETURNING_TO_BASE"));


}

/**
 *
 */
LowFuelState::~LowFuelState()
{

}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void LowFuelState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Closes the window and sets the timer to 5 Secs.
 * @param action Pointer to an action.
 */
void LowFuelState::btnOk5SecsClick(Action *)
{
	_state->timerReset();
	_game->popState();
}

}
