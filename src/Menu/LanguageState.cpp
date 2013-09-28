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
#include "LanguageState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Interface/Window.h"
#include "../Interface/TextList.h"
#include "../Engine/Language.h"
#include "MainMenuState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Language window.
 * @param game Pointer to the core game.
 */
LanguageState::LanguageState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 256, 160, 32, 20, POPUP_BOTH);
	_lstLanguages = new TextList(224, 136, 40, 34);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_lstLanguages);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_lstLanguages->setColor(Palette::blockOffset(8)+10);
	_lstLanguages->setArrowColor(Palette::blockOffset(8)+5);
	_lstLanguages->setColumns(1, 224);
	_lstLanguages->setSelectable(true);
	_lstLanguages->setBackground(_window);
	_lstLanguages->setMargin(2);
	_lstLanguages->setAlign(ALIGN_CENTER);
	_lstLanguages->onMouseClick((ActionHandler)&LanguageState::lstLanguagesClick);

	_langs = Language::getList(_lstLanguages);
}

/**
 *
 */
LanguageState::~LanguageState()
{

}

/**
 * Sets the selected language and opens
 * the Main Menu window.
 * @param action Pointer to an action.
 */
void LanguageState::lstLanguagesClick(Action *)
{
	_game->loadLanguage(_langs[_lstLanguages->getSelectedRow()]);
	_game->setState(new MainMenuState(_game));
}

}
