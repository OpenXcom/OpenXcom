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
#include "MainMenuState.h"
#include "../version.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Music.h"
#include "NewGameState.h"
#include "NewBattleState.h"
#include "LoadState.h"
#include "OptionsState.h"

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
	_btnNewGame = new TextButton(92, 20, 64, 90);
	_btnNewBattle = new TextButton(92, 20, 164, 90);
	_btnLoad = new TextButton(92, 20, 64, 118);
	_btnOptions = new TextButton(92, 20, 164, 118);
	_btnQuit = new TextButton(192, 20, 64, 146);
	_txtTitle = new Text(256, 30, 32, 45);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnNewGame);
	add(_btnNewBattle);
	add(_btnLoad);
	add(_btnOptions);
	add(_btnQuit);
	add(_txtTitle);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnNewGame->setColor(Palette::blockOffset(8)+5);
	_btnNewGame->setText(_game->getLanguage()->getString("STR_NEW_GAME"));
	_btnNewGame->onMouseClick((ActionHandler)&MainMenuState::btnNewGameClick);

	_btnNewBattle->setColor(Palette::blockOffset(8)+5);
	_btnNewBattle->setText(_game->getLanguage()->getString("STR_NEW_BATTLE"));
	_btnNewBattle->onMouseClick((ActionHandler)&MainMenuState::btnNewBattleClick);

	_btnLoad->setColor(Palette::blockOffset(8)+5);
	_btnLoad->setText(_game->getLanguage()->getString("STR_LOAD_SAVED_GAME"));
	_btnLoad->onMouseClick((ActionHandler)&MainMenuState::btnLoadClick);

	_btnOptions->setColor(Palette::blockOffset(8)+5);
	_btnOptions->setText(_game->getLanguage()->getString("STR_OPTIONS"));
	_btnOptions->onMouseClick((ActionHandler)&MainMenuState::btnOptionsClick);

	_btnQuit->setColor(Palette::blockOffset(8)+5);
	_btnQuit->setText(_game->getLanguage()->getString("STR_QUIT"));
	_btnQuit->onMouseClick((ActionHandler)&MainMenuState::btnQuitClick);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	std::wstringstream title;
	title << _game->getLanguage()->getString("STR_OPENXCOM") << L"\x02";
	title << Language::utf8ToWstr(OPENXCOM_VERSION_SHORT) << Language::utf8ToWstr(OPENXCOM_VERSION_GIT);
	_txtTitle->setText(title.str());

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
void MainMenuState::btnNewGameClick(Action *)
{
	_game->pushState(new NewGameState(_game));
}

/**
 * Opens the New Battle screen.
 * @param action Pointer to an action.
 */
void MainMenuState::btnNewBattleClick(Action *)
{
	_game->pushState(new NewBattleState(_game));
}

/**
 * Opens the Load Game screen.
 * @param action Pointer to an action.
 */
void MainMenuState::btnLoadClick(Action *)
{
	_game->pushState(new LoadState(_game, true));
}

/**
 * Opens the Options screen.
 * @param action Pointer to an action.
 */
void MainMenuState::btnOptionsClick(Action *)
{
	_game->pushState(new OptionsState(_game));
}

/**
 * Quits the game.
 * @param action Pointer to an action.
 */
void MainMenuState::btnQuitClick(Action *)
{
	_game->quit();
}

}
