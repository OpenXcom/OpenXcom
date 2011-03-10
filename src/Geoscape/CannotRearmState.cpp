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
#include "CannotRearmState.h"
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
 * Initializes all the elements in a Cannot Rearm window.
 * @param game Pointer to the core game.
 * @param ammo Ammo missing.
 * @param craft Craft rearming.
 * @param base Base the craft belongs to.
 */
CannotRearmState::CannotRearmState(Game *game, const std::wstring &ammo, const std::wstring &craft, const std::wstring &base) : State(game)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 160, 32, 20, POPUP_BOTH);
	_btnOk = new TextButton(100, 18, 48, 150);
	_btnOk5Secs = new TextButton(100, 18, 172, 150);
	_txtMessage = new Text(246, 80, 37, 50);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnOk5Secs);
	add(_txtMessage);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+8);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CannotRearmState::btnOkClick);

	_btnOk->setColor(Palette::blockOffset(8)+8);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK_5_SECS"));
	_btnOk->onMouseClick((ActionHandler)&CannotRearmState::btnOk5SecsClick);

	_txtMessage->setColor(Palette::blockOffset(15)-1);
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setVerticalAlign(ALIGN_MIDDLE);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);
	std::wstring s = _game->getLanguage()->getString("STR_NOT_ENOUGH");
	s += ammo;
	s += _game->getLanguage()->getString("STR_TO_REARM");
	s += base;
	s += _game->getLanguage()->getString("STR_AT");
	s += craft;
	_txtMessage->setText(s);
}

/**
 *
 */
CannotRearmState::~CannotRearmState()
{
	
}

/**
 * Resets the palette.
 */
void CannotRearmState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void CannotRearmState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void CannotRearmState::btnOk5SecsClick(Action *action)
{
	_game->popState();
}

}
