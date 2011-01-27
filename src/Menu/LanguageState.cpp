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
#include "LanguageState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
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
	_btnEnglish = new TextButton(192, 20, 64, 34);
	_btnGerman = new TextButton(192, 20, 64, 62);
	_btnFrench = new TextButton(192, 20, 64, 90);
	_btnItalian = new TextButton(192, 20, 64, 118);
	_btnSpanish = new TextButton(192, 20, 64, 146);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
	
	add(_window);
	add(_btnEnglish);
	add(_btnGerman);
	add(_btnFrench);
	add(_btnItalian);
	add(_btnSpanish);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));
	
	_btnEnglish->setColor(Palette::blockOffset(8)+8);
	_btnEnglish->setText(L"ENGLISH");
	_btnEnglish->onMouseClick((ActionHandler)&LanguageState::btnEnglishClick);
	
	_btnGerman->setColor(Palette::blockOffset(8)+8);
	_btnGerman->setText(L"DEUTSCHE");
	_btnGerman->onMouseClick((ActionHandler)&LanguageState::btnGermanClick);

	_btnFrench->setColor(Palette::blockOffset(8)+8);
	_btnFrench->setText(L"FRANCAIS");
	_btnFrench->onMouseClick((ActionHandler)&LanguageState::btnFrenchClick);

	_btnItalian->setColor(Palette::blockOffset(8)+8);
	_btnItalian->setText(L"ITALIANO");
	_btnItalian->onMouseClick((ActionHandler)&LanguageState::btnItalianClick);

	_btnSpanish->setColor(Palette::blockOffset(8)+8);
	_btnSpanish->setText(L"ESPANOL");
	_btnSpanish->onMouseClick((ActionHandler)&LanguageState::btnSpanishClick);
}

/**
 *
 */
LanguageState::~LanguageState()
{
	
}

void LanguageState::changeLanguage(const std::string &lang)
{
	std::stringstream ss;
	ss << _game->getResourcePack()->getFolder() << "Language/" << lang;
	Language *l = new Language();
	l->loadLng(ss.str());
	_game->setLanguage(l);
	_game->setState(new MainMenuState(_game));
}

/**
 * Sets the language to English and opens
 * the Main Menu window.
 * @param action Pointer to an action.
 */
void LanguageState::btnEnglishClick(Action *action)
{
	changeLanguage("English.lng");
}

/**
 * Sets the language to German and opens
 * the Main Menu window.
 * @param action Pointer to an action.
 */
void LanguageState::btnGermanClick(Action *action)
{
	changeLanguage("German.lng");
}

/**
 * Sets the language to French and opens
 * the Main Menu window.
 * @param action Pointer to an action.
 */
void LanguageState::btnFrenchClick(Action *action)
{
	changeLanguage("French.lng");
}

/**
 * Sets the language to Italian and opens
 * the Main Menu window.
 * @param action Pointer to an action.
 */
void LanguageState::btnItalianClick(Action *action)
{
	changeLanguage("Italian.lng");
}

/**
 * Sets the language to Spanish and opens
 * the Main Menu window.
 * @param action Pointer to an action.
 */
void LanguageState::btnSpanishClick(Action *action)
{
	changeLanguage("Spanish.lng");
}

}
