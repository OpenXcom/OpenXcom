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
#include "StartState.h"

StartState::StartState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(256, 160, 32, 20);
	_buttonEn = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 90);
	_buttonFr = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 150);
	_buttonDe = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 120);
	
	// Set palette
	_game->setPalette(_game->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	_window->setPalette(_game->getScreen()->getPalette());
	_buttonEn->setPalette(_game->getScreen()->getPalette());
	_buttonFr->setPalette(_game->getScreen()->getPalette());
	_buttonDe->setPalette(_game->getScreen()->getPalette());

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBg(game->getSurface("BACK01.SCR"));

	_buttonEn->setColor(Palette::blockOffset(8)+8);
	_buttonEn->setText("ENGLISH");
	_buttonEn->onClick((EventHandler)&StartState::buttonEnClick);

	_buttonFr->setColor(Palette::blockOffset(8)+8);
	_buttonFr->setText("FRANCAIS");
	_buttonFr->onClick((EventHandler)&StartState::buttonFrClick);

	_buttonDe->setColor(Palette::blockOffset(8)+8);
	_buttonDe->setText("DEUTSCHE");
	_buttonDe->onClick((EventHandler)&StartState::buttonDeClick);
}

StartState::~StartState()
{
	delete _window;
	delete _buttonEn;
	delete _buttonFr;
	delete _buttonDe;
	State::~State();
}

void StartState::handle(SDL_Event *ev, int scale)
{
	_buttonEn->handle(ev, scale, this);
	_buttonFr->handle(ev, scale, this);
	_buttonDe->handle(ev, scale, this);
}

void StartState::think()
{
}

void StartState::blit()
{
	_window->blit(_game->getScreen()->getSurface());
	_buttonEn->blit(_game->getScreen()->getSurface());
	_buttonFr->blit(_game->getScreen()->getSurface());
	_buttonDe->blit(_game->getScreen()->getSurface());
}

void StartState::buttonEnClick(SDL_Event *ev)
{
	_game->setLanguage("ENGLISH.DAT");
	_game->setState(new MainMenuState(_game));
}

void StartState::buttonFrClick(SDL_Event *ev)
{
	_game->setLanguage("FRENCH.DAT");
	_game->setState(new MainMenuState(_game));
}

void StartState::buttonDeClick(SDL_Event *ev)
{
	_game->setLanguage("GERMAN.DAT");
	_game->setState(new MainMenuState(_game));
}