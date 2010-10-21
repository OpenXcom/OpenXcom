/*
 * Copyright 2010 Daniel Albano
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
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Resource/LangString.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Music.h"
#include "NewGameState.h"

/**
 * Initializes all the elements in the Main Menu window.
 * @param game Pointer to the core game.
 */
MainMenuState::MainMenuState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 256, 160, 32, 20, POPUP_BOTH);
	_btnNew = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 20, 64, 90);
	_btnLoad = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 20, 64, 118);
	_btnQuit = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 20, 64, 146);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 256, 30, 32, 45);
	_txtVersion = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 10, 160, 190);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnNew);
	add(_btnLoad);
	add(_btnQuit);
	add(_txtTitle);
	add(_txtVersion);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnNew->setColor(Palette::blockOffset(8)+8);
	_btnNew->setText(_game->getResourcePack()->getLanguage()->getString(STR_NEW_GAME));
	_btnNew->onMouseClick((EventHandler)&MainMenuState::btnNewClick);

	_btnLoad->setColor(Palette::blockOffset(8)+8);
	_btnLoad->setText(_game->getResourcePack()->getLanguage()->getString(STR_LOAD_SAVED_GAME));
	_btnLoad->onMouseClick((EventHandler)&MainMenuState::btnLoadClick);

	_btnQuit->setColor(Palette::blockOffset(8)+8);
	_btnQuit->setText(_game->getResourcePack()->getLanguage()->getString(STR_QUIT));
	_btnQuit->onMouseClick((EventHandler)&MainMenuState::btnQuitClick);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_UFO_ENEMY_UNKNOWN));
	
	_txtVersion->setColor(Palette::blockOffset(8)+10);
	_txtVersion->setAlign(ALIGN_RIGHT);
	_txtVersion->setSmall();
	_txtVersion->setText("OpenXcom v0.1");

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
 * Opens the New Game window.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void MainMenuState::btnNewClick(SDL_Event *ev, int scale)
{
	_game->setState(new NewGameState(_game));
}

void MainMenuState::btnLoadClick(SDL_Event *ev, int scale)
{
	
}

/**
 * Quits the game.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void MainMenuState::btnQuitClick(SDL_Event *ev, int scale)
{
	_game->quit();
}
