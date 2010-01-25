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

MainMenuState::MainMenuState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(256, 160, 32, 20);
	_buttonNew = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 90);
	_buttonLoad = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 120);
	_buttonQuit = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 150);
	_title = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 256, 40, 32, 45);
	
	// Set palette
	_window->setPalette(_game->getScreen()->getPalette());
	_buttonNew->setPalette(_game->getScreen()->getPalette());
	_buttonLoad->setPalette(_game->getScreen()->getPalette());
	_buttonQuit->setPalette(_game->getScreen()->getPalette());
	_title->setPalette(_game->getScreen()->getPalette());

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBg(game->getSurface("BACK01.SCR"));

	_buttonNew->setColor(Palette::blockOffset(8)+8);
	_buttonNew->setText(_game->getLanguage()->getString(780));
	_buttonNew->onClick((EventHandler)&MainMenuState::buttonNewClick);

	_buttonLoad->setColor(Palette::blockOffset(8)+8);
	_buttonLoad->setText(_game->getLanguage()->getString(781));
	_buttonLoad->onClick((EventHandler)&MainMenuState::buttonLoadClick);

	_buttonQuit->setColor(Palette::blockOffset(8)+8);
	_buttonQuit->setText(_game->getLanguage()->getString(801));
	_buttonQuit->onClick((EventHandler)&MainMenuState::buttonQuitClick);

	_title->setColor(Palette::blockOffset(8)+10);
	_title->setAlign(ALIGN_CENTER);
	_title->setBig();
	_title->setText(_game->getLanguage()->getString(779));
}

MainMenuState::~MainMenuState()
{
	delete _window;
	delete _buttonNew;
	delete _buttonLoad;
	delete _buttonQuit;
	delete _title;
	State::~State();
}

void MainMenuState::handle(SDL_Event *ev, int scale)
{
	_buttonNew->handle(ev, scale, this);
	_buttonLoad->handle(ev, scale, this);
	_buttonQuit->handle(ev, scale, this);
}

void MainMenuState::think()
{
}

void MainMenuState::blit()
{
	_window->blit(_game->getScreen()->getSurface());
	_buttonNew->blit(_game->getScreen()->getSurface());
	_buttonLoad->blit(_game->getScreen()->getSurface());
	_buttonQuit->blit(_game->getScreen()->getSurface());
	_title->blit(_game->getScreen()->getSurface());
}

void MainMenuState::buttonNewClick(SDL_Event *ev)
{
	_game->setState(new NewGameState(_game));
}

void MainMenuState::buttonLoadClick(SDL_Event *ev)
{
	
}

void MainMenuState::buttonQuitClick(SDL_Event *ev)
{
	_game->quit();
}