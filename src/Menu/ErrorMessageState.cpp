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
#include "ErrorMessageState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in an error window.
 * @param game Pointer to the core game.
 * @param id Language ID for the message to display.
 * @param color Color of the UI controls.
 * @param bg Background image.
 * @param bgColor Background color (-1 for Battlescape).
 */
ErrorMessageState::ErrorMessageState(Game *game, const std::string &id, Uint8 color, std::string bg, Uint8 bgColor) : State(game)
{
	create(id, L"", color, bg, bgColor);
}

/**
 * Initializes all the elements in an error window.
 * @param game Pointer to the core game.
 * @param msg Text string for the message to display.
 * @param color Color of the UI controls.
 * @param bg Background image.
 * @param bgColor Background color (-1 for Battlescape).
 */
ErrorMessageState::ErrorMessageState(Game *game, const std::wstring &msg, Uint8 color, std::string bg, Uint8 bgColor) : State(game)
{
	create("", msg, color, bg, bgColor);
}

/**
 *
 */
ErrorMessageState::~ErrorMessageState()
{

}

void ErrorMessageState::create(const std::string &str, const std::wstring &wstr, Uint8 color, std::string bg, Uint8 bgColor)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 160, 32, 20, POPUP_BOTH);
	_btnOk = new TextButton(120, 18, 100, 154);
	_txtMessage = new Text(246, 80, 37, 50);

	// Set palette
	if (bgColor != ((Uint8)-1))
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(bgColor)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtMessage);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(color);
	_window->setBackground(_game->getResourcePack()->getSurface(bg));

	_btnOk->setColor(color);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ErrorMessageState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ErrorMessageState::btnOkClick, (SDLKey)Options::getInt("keyOk"));
	_btnOk->onKeyboardPress((ActionHandler)&ErrorMessageState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_txtMessage->setColor(color);
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setVerticalAlign(ALIGN_MIDDLE);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);
	if (str.empty())
		_txtMessage->setText(wstr);
	else
		_txtMessage->setText(_game->getLanguage()->getString(str));

	if (bgColor == ((Uint8)-1))
	{
		_window->setHighContrast(true);
		_btnOk->setHighContrast(true);
		_txtMessage->setHighContrast(true);
	}
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void ErrorMessageState::btnOkClick(Action *)
{
	_game->popState();
}

}
