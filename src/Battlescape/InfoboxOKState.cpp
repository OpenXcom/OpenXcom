/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "InfoboxOKState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/Cursor.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements.
 * @param game Pointer to the core game.
 * @param msg Message string.
 */
InfoboxOKState::InfoboxOKState(Game *game, const std::wstring &msg) : State(game)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 261, 89, 30, 48);
	_btnOk = new TextButton(120, 18, 100, 112);
	_txtTitle = new Text(255, 61, 33, 51);
	
	add(_window);
	add(_btnOk);
	add(_txtTitle);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(1)-1);
	_window->setHighContrast(true);

	_btnOk->setColor(Palette::blockOffset(1)-1);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&InfoboxOKState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&InfoboxOKState::btnOkClick, (SDLKey)Options::getInt("keyOk"));
	_btnOk->onKeyboardPress((ActionHandler)&InfoboxOKState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));
	_btnOk->setHighContrast(true);

	_txtTitle->setColor(Palette::blockOffset(1)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setVerticalAlign(ALIGN_MIDDLE);
	_txtTitle->setHighContrast(true);
	_txtTitle->setWordWrap(true);
	_txtTitle->setText(msg);

	_game->getCursor()->setVisible(true);
}

/**
 *
 */
InfoboxOKState::~InfoboxOKState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void InfoboxOKState::btnOkClick(Action *)
{
	_game->popState();
}

}
