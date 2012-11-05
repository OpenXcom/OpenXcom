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
#include "InfoboxOKState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/Cursor.h"

namespace OpenXcom
{

/**
 * Initializes all the elements.
 * @param game Pointer to the core game.
 * @param message Pointer to  the message.
 */
InfoboxOKState::InfoboxOKState(Game *game, std::wstring name, std::string message) : State(game), _name(name), _message(message)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 222, 92, 48, 48, POPUP_BOTH);
	_btnOk = new TextButton(120, 18, 98, 112);
	_txtTitle = new Text(210, 60, 60, 60);
	
	add(_window);
	add(_btnOk);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(6)+11);

	_btnOk->setColor(Palette::blockOffset(1)-1);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&InfoboxOKState::btnOkClick);
	_btnOk->setHighContrast(true);

	_txtTitle->setColor(Palette::blockOffset(1)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setHighContrast(true);
	_txtTitle->setWordWrap(true);
	std::wstring s = _name;
	s += L'\n';
	s += _game->getLanguage()->getString(_message);
	_txtTitle->setText(s);

	_game->getCursor()->setVisible(true);
}

/**
 *
 */
InfoboxOKState::~InfoboxOKState()
{

}

/**
 * Resets the palette.
 */
void InfoboxOKState::init()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void InfoboxOKState::btnOkClick(Action *action)
{
	_game->popState();
}

}
