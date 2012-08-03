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
#include "AbandonGameState.h"
#include "../Menu/LoadGameState.h"
#include "../Menu/SaveGameState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Game Options window.
 * @param game Pointer to the core game.
 */
OptionsState::OptionsState(Game *game) : State(game)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 216, 160, 20, 20, POPUP_BOTH);
	_btnLoad = new TextButton(180, 20, 38, 60);
	_btnSave = new TextButton(180, 20, 38, 85);
	_btnAbandon = new TextButton(180, 20, 38, 110);
	_btnCancel = new TextButton(180, 20, 38, 140);
	_txtTitle = new Text(206, 15, 25, 32);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnLoad);
	add(_btnSave);
	add(_btnAbandon);
	add(_btnCancel);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnLoad->setColor(Palette::blockOffset(15)-1);
	_btnLoad->setText(_game->getLanguage()->getString("STR_LOAD_GAME"));
	_btnLoad->onMouseClick((ActionHandler)&OptionsState::btnLoadClick);

	_btnSave->setColor(Palette::blockOffset(15)-1);
	_btnSave->setText(_game->getLanguage()->getString("STR_SAVE_GAME"));
	_btnSave->onMouseClick((ActionHandler)&OptionsState::btnSaveClick);

	_btnAbandon->setColor(Palette::blockOffset(15)-1);
	_btnAbandon->setText(_game->getLanguage()->getString("STR_ABANDON_GAME"));
	_btnAbandon->onMouseClick((ActionHandler)&OptionsState::btnAbandonClick);

	_btnCancel->setColor(Palette::blockOffset(15)-1);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&OptionsState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString("STR_GAME_OPTIONS"));
}

/**
 *
 */
OptionsState::~OptionsState()
{

}

/**
 * Resets the palette
 * since it's bound to change on other screens.
 */
void OptionsState::init()
{
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
}

/**
 * Opens the Load Game screen.
 * @param action Pointer to an action.
 */
void OptionsState::btnLoadClick(Action *action)
{
	_game->pushState(new LoadGameState(_game, true));
}

/**
 * Opens the Save Game screen.
 * @param action Pointer to an action.
 */
void OptionsState::btnSaveClick(Action *action)
{
	_game->pushState(new SaveGameState(_game, true));
}

/**
 * Opens the Abandon Game window.
 * @param action Pointer to an action.
 */
void OptionsState::btnAbandonClick(Action *action)
{
	_game->pushState(new AbandonGameState(_game));
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
