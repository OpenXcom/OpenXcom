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
#include "NoteState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "LanguageState.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Main Menu window.
 * @param game Pointer to the core game.
 */
NoteState::NoteState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 256, 130, 32, 35, POPUP_BOTH);
	_btnOk = new TextButton(192, 20, 64, 136);
	_txtTitle = new Text(240, 70, 40, 45);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	
	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(L"OK");
	_btnOk->onMouseClick((ActionHandler)&NoteState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NoteState::btnOkClick, (SDLKey)Options::getInt("keyOk"));
	_btnOk->onKeyboardPress((ActionHandler)&NoteState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setWordWrap(true);
	_txtTitle->setText(L"NOTE\x02This is an early development build of OpenXcom!\n\nThe project is still work-in-progress, so a lot of features are incomplete or completely missing.\n\nDo not be alarmed, this is completely normal!");
}

/**
 *
 */
NoteState::~NoteState()
{

}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void NoteState::btnOkClick(Action *)
{
	_game->setState(new LanguageState(_game));
}

}
