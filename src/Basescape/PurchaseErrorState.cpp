/*
 * Copyright 2010 OpenXcom Developers.
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
#include "PurchaseErrorState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in a Purchase error window.
 * @param game Pointer to the core game.
 * @param str Error message to display.
 */
PurchaseErrorState::PurchaseErrorState(Game *game, std::string str) : State(game)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 160, 32, 20, POPUP_BOTH);
	_btnOk = new TextButton(120, 18, 100, 154);
	_txtError = new Text(234, 80, 44, 50);

	add(_window);
	add(_btnOk);
	add(_txtError);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+1);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&PurchaseErrorState::btnOkClick);

	_txtError->setColor(Palette::blockOffset(15)+1);
	_txtError->setAlign(ALIGN_CENTER);
	_txtError->setVerticalAlign(ALIGN_MIDDLE);
	_txtError->setBig();
	_txtError->setWordWrap(true);
	_txtError->setText(_game->getLanguage()->getString(str));
}

/**
 *
 */
PurchaseErrorState::~PurchaseErrorState()
{
	
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void PurchaseErrorState::btnOkClick(Action *action)
{
	_game->popState();
}

}
