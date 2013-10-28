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
#include "OptionsLanguageState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/TextButton.h"
#include "../Engine/Language.h"
#include "../Engine/Options.h"
#include "MainMenuState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Language window.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsLanguageState::OptionsLanguageState(Game *game, OptionsOrigin origin) : OptionsBaseState(game, origin)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 17, 0, 8);
	_lstLanguages = new TextList(272, 144, 24, 26);
	_btnCancel = new TextButton(100, 16, 110, 176);

	add(_window);
	add(_txtTitle);
	add(_lstLanguages);
	add(_btnCancel);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_LANGUAGE"));

	_lstLanguages->setColor(Palette::blockOffset(8)+10);
	_lstLanguages->setArrowColor(Palette::blockOffset(8)+5);
	_lstLanguages->setColumns(1, 272);
	_lstLanguages->setSelectable(true);
	_lstLanguages->setBackground(_window);
	_lstLanguages->setAlign(ALIGN_CENTER);
	_lstLanguages->onMouseClick((ActionHandler)&OptionsLanguageState::lstLanguagesClick);

	_btnCancel->setColor(Palette::blockOffset(8) + 5);
	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler) &OptionsLanguageState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler) &OptionsLanguageState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_langs = Language::getList(_lstLanguages);
}

/**
 *
 */
OptionsLanguageState::~OptionsLanguageState()
{

}

/**
 * Sets the selected language and opens
 * the Main Menu window.
 * @param action Pointer to an action.
 */
void OptionsLanguageState::lstLanguagesClick(Action *)
{
	_game->loadLanguage(_langs[_lstLanguages->getSelectedRow()]);
	saveOptions();
}

/**
* Returns to the previous screen.
* @param action Pointer to an action.
*/
void OptionsLanguageState::btnCancelClick(Action *)
{
	_game->popState();
}

}
