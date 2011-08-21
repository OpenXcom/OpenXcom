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
#include "MainMenuState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Music.h"
#include "../Engine/Options.h"
#include "NewGameState.h"
#include "LoadGameState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Main Menu window.
 * @param game Pointer to the core game.
 */
MainMenuState::MainMenuState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 256, 160, 32, 20, POPUP_BOTH);
	_btnNew = new TextButton(192, 20, 64, 90);
	_btnLoad = new TextButton(192, 20, 64, 118);
	_btnQuit = new TextButton(192, 20, 64, 146);
	_txtTitle = new Text(256, 30, 32, 45);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnNew);
	add(_btnLoad);
	add(_btnQuit);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnNew->setColor(Palette::blockOffset(8)+8);
	_btnNew->setText(_game->getLanguage()->getString("STR_NEW_GAME"));
	_btnNew->onMouseClick((ActionHandler)&MainMenuState::btnNewClick);

	_btnLoad->setColor(Palette::blockOffset(8)+8);
	_btnLoad->setText(_game->getLanguage()->getString("STR_LOAD_SAVED_GAME"));
	_btnLoad->onMouseClick((ActionHandler)&MainMenuState::btnLoadClick);

	_btnQuit->setColor(Palette::blockOffset(8)+8);
	_btnQuit->setText(_game->getLanguage()->getString("STR_QUIT"));
	_btnQuit->onMouseClick((ActionHandler)&MainMenuState::btnQuitClick);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	std::wstring s = L"OpenXcom\x02";
	s += Language::utf8ToWstr(Options::getVersion());
	_txtTitle->setText(s);

	// Set music
	_game->getResourcePack()->getMusic("GMSTORY")->play();
}

/**
 *
 */
MainMenuState::~MainMenuState()
{
	
}

/**
 * Resets the palette
 * since it's bound to change on other screens.
 */
void MainMenuState::init()
{
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
}

/**
 * Opens the New Game window.
 * @param action Pointer to an action.
 */
void MainMenuState::btnNewClick(Action *action)
{
	_game->setState(new NewGameState(_game));
}

/**
 * Opens the Load Game screen.
 * @param action Pointer to an action.
 */
void MainMenuState::btnLoadClick(Action *action)
{
	_game->pushState(new LoadGameState(_game));
}

/**
 * Quits the game.
 * @param action Pointer to an action.
 */
void MainMenuState::btnQuitClick(Action *action)
{
	_game->quit();
}

}
