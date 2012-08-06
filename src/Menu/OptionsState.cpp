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
#include "OptionsState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Options window.
 * @param game Pointer to the core game.
 */
OptionsState::OptionsState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 16, 0, 8);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);

	add(_window);
	add(_txtTitle);
	add(_btnOk);
	add(_btnCancel);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString("STR_OPTIONS"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&OptionsState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&OptionsState::btnCancelClick);
}

/**
 *
 */
OptionsState::~OptionsState()
{
	
}

/**
 * Saves the options.
 * @param action Pointer to an action.
 */
void OptionsState::btnOkClick(Action *action)
{
	
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void OptionsState::btnCancelClick(Action *action)
{
	_game->popState();
}

}
