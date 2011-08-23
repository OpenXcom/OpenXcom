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
#include "BattlescapeOptionsState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Action.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Battlescape Options screen.
 * @param game Pointer to the core game.
 */
BattlescapeOptionsState::BattlescapeOptionsState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(320, 16, 0, 16);

	_btnOk = new TextButton(120, 16, 16, 174);
	_btnSave = new TextButton(120, 16, 184, 174);

	add(_window);
	add(_txtTitle);
	add(_btnOk);
	add(_btnSave);

	// Set up objects
	_window->setColor(Palette::blockOffset(0)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));

	_txtTitle->setColor(Palette::blockOffset(0));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setHighContrast(true);
	_txtTitle->setText(_game->getLanguage()->getString("STR_GAME_OPTIONS"));

	_btnOk->setColor(Palette::blockOffset(0)+3);
	_btnOk->setHighContrast(true);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BattlescapeOptionsState::btnOkClick);

	_btnSave->setColor(Palette::blockOffset(0)+3);
	_btnSave->setHighContrast(true);
	_btnSave->setText(_game->getLanguage()->getString("STR_NOT_AVAILABLE"));
	_btnSave->onMouseClick((ActionHandler)&BattlescapeOptionsState::btnSaveClick);
}

/**
 *
 */
BattlescapeOptionsState::~BattlescapeOptionsState()
{
	
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BattlescapeOptionsState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Opens the Save Game screen.
 * @param action Pointer to an action.
 */
void BattlescapeOptionsState::btnSaveClick(Action *action)
{
	;
}

}
