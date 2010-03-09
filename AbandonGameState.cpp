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
#include "AbandonGameState.h"

AbandonGameState::AbandonGameState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(256, 160, 32, 20);
	_btnNew = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 90);
	_btnLoad = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 118);
	_btnQuit = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 146);
	_txtTitle = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 256, 30, 32, 45);
	_txtVersion = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 160, 10, 160, 190);
	
	// Set palette
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnNew);
	add(_btnLoad);
	add(_btnQuit);
	add(_txtTitle);
	add(_txtVersion);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBg(game->getSurface("BACK01.SCR"));

	_btnNew->setColor(Palette::blockOffset(8)+8);
	_btnNew->setText(_game->getLanguage()->getString(780));
	_btnNew->onMouseClick((EventHandler)&AbandonGameState::btnNewClick);

	_btnLoad->setColor(Palette::blockOffset(8)+8);
	_btnLoad->setText(_game->getLanguage()->getString(781));
	_btnLoad->onMouseClick((EventHandler)&AbandonGameState::btnLoadClick);

	_btnQuit->setColor(Palette::blockOffset(8)+8);
	_btnQuit->setText(_game->getLanguage()->getString(801));
	_btnQuit->onMouseClick((EventHandler)&AbandonGameState::btnQuitClick);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString(779));
	
	_txtVersion->setColor(Palette::blockOffset(8)+10);
	_txtVersion->setAlign(ALIGN_RIGHT);
	_txtVersion->setSmall();
	_txtVersion->setText("OpenXcom v0.1");
}

AbandonGameState::~AbandonGameState()
{
	State::~State();
}

void AbandonGameState::think()
{
}

void AbandonGameState::btnNewClick(SDL_Event *ev, int scale)
{
	_game->setState(new NewGameState(_game));
}

void AbandonGameState::btnLoadClick(SDL_Event *ev, int scale)
{
	
}

void AbandonGameState::btnQuitClick(SDL_Event *ev, int scale)
{
	_game->quit();
}