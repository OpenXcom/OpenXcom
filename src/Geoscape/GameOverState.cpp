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
#include "GameOverState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/InteractiveSurface.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the GameOver screen.
 * @param game Pointer to the core game.
 */
GameOverState::GameOverState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtText = new Text(320, 16, 0, 8);
	_screen = new InteractiveSurface(320, 200, 0, 0);

	_game->setPalette(_game->getResourcePack()->getSurface("PICT1.LBM")->getPalette());

	add(_window);
	add(_txtText);
	add(_screen);

	// Set up objects
	_window->setColor(Palette::blockOffset(0));
	_window->setBackground(_game->getResourcePack()->getSurface("PICT1.LBM"));

	_screen->onMouseClick((ActionHandler)&GameOverState::windowClick);

	_txtText->setColor(Palette::blockOffset(1));
	_txtText->setText(_game->getLanguage()->getString("STR_VICTORY_1"));
}

/**
 *
 */
GameOverState::~GameOverState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void GameOverState::windowClick(Action *action)
{
	_game->popState();
}

}
