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

namespace OpenXcom
{

/**
 * Initializes all the elements in the Low Fuel window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to display.
 * @param state Pointer to the Geoscape.
 */
LowFuelState::LowFuelState(Game *game, Craft *craft, GeoscapeState *state) : State(game), _craft(craft), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 224, 120, 16, 40, POPUP_BOTH);
	_btnOk = new TextButton(90, 18, 30, 120);
	_btnOk5Secs = new TextButton(90, 18, 136, 120);
	_txtTitle = new Text(214, 16, 21, 60);
	_txtMessage = new Text(214, 16, 21, 90);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnOk5Secs);
	add(_txtTitle);
	add(_txtMessage);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&LowFuelState::btnOkClick);

	_btnOk5Secs->setColor(Palette::blockOffset(8)+5);
	_btnOk5Secs->setText(_game->getLanguage()->getString("STR_OK_5_SECS"));
	_btnOk5Secs->onMouseClick((ActionHandler)&LowFuelState::btnOk5SecsClick);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_craft->getName(_game->getLanguage()));

	_txtMessage->setColor(Palette::blockOffset(8)+10);
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setText(_game->getLanguage()->getString("STR_IS_LOW_ON_FUEL_RETURNING_TO_BASE"));


}

/**
 *
 */
LowFuelState::~LowFuelState()
{

}

/**
 * Resets the palette.
 */
void LowFuelState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void LowFuelState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Closes the window and sets the timer to 5 Secs.
 * @param action Pointer to an action.
 */
void LowFuelState::btnOk5SecsClick(Action *action)
{
	_state->timerReset();
	_game->popState();
}

}
