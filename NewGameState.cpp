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
#include "NewGameState.h"

NewGameState::NewGameState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(192, 180, 64, 10);
	_buttonBeg = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 160, 18, 80, 55);
	_buttonExp = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 160, 18, 80, 80);
	_buttonVet = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 160, 18, 80, 105);
	_buttonGen = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 160, 18, 80, 130);
	_buttonSup = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 160, 18, 80, 155);
	_title = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 30);
	
	// Set palette
	_window->setPalette(_game->getScreen()->getPalette());
	_buttonBeg->setPalette(_game->getScreen()->getPalette());
	_buttonExp->setPalette(_game->getScreen()->getPalette());
	_buttonVet->setPalette(_game->getScreen()->getPalette());
	_buttonGen->setPalette(_game->getScreen()->getPalette());
	_buttonSup->setPalette(_game->getScreen()->getPalette());
	_title->setPalette(_game->getScreen()->getPalette());

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBg(game->getSurface("BACK01.SCR"));

	_buttonBeg->setColor(Palette::blockOffset(8)+8);
	_buttonBeg->setText(_game->getLanguage()->getString(783));
	_buttonBeg->onClick((EventHandler)&NewGameState::buttonBegClick);

	_buttonExp->setColor(Palette::blockOffset(8)+8);
	_buttonExp->setText(_game->getLanguage()->getString(784));
	_buttonExp->onClick((EventHandler)&NewGameState::buttonExpClick);

	_buttonVet->setColor(Palette::blockOffset(8)+8);
	_buttonVet->setText(_game->getLanguage()->getString(785));
	_buttonVet->onClick((EventHandler)&NewGameState::buttonVetClick);

	_buttonGen->setColor(Palette::blockOffset(8)+8);
	_buttonGen->setText(_game->getLanguage()->getString(786));
	_buttonGen->onClick((EventHandler)&NewGameState::buttonGenClick);

	_buttonSup->setColor(Palette::blockOffset(8)+8);
	_buttonSup->setText(_game->getLanguage()->getString(787));
	_buttonSup->onClick((EventHandler)&NewGameState::buttonSupClick);

	_title->setColor(Palette::blockOffset(8)+10);
	_title->setAlign(ALIGN_CENTER);
	_title->setSmall();
	_title->setText(_game->getLanguage()->getString(782));
}

NewGameState::~NewGameState()
{
	delete _window;
	delete _buttonBeg;
	delete _buttonExp;
	delete _buttonVet;
	delete _buttonGen;
	delete _buttonSup;
	delete _title;
	State::~State();
}

void NewGameState::handle(SDL_Event *ev, int scale)
{
	_buttonBeg->handle(ev, scale, this);
	_buttonExp->handle(ev, scale, this);
	_buttonVet->handle(ev, scale, this);
	_buttonGen->handle(ev, scale, this);
	_buttonSup->handle(ev, scale, this);
}

void NewGameState::think()
{
}

void NewGameState::blit()
{
	_window->blit(_game->getScreen()->getSurface());
	_buttonBeg->blit(_game->getScreen()->getSurface());
	_buttonExp->blit(_game->getScreen()->getSurface());
	_buttonVet->blit(_game->getScreen()->getSurface());
	_buttonGen->blit(_game->getScreen()->getSurface());
	_buttonSup->blit(_game->getScreen()->getSurface());
	_title->blit(_game->getScreen()->getSurface());
}

void NewGameState::buttonBegClick(SDL_Event *ev)
{
	_game->setState(new GeoscapeState(_game));
}

void NewGameState::buttonExpClick(SDL_Event *ev)
{
	_game->setState(new GeoscapeState(_game));
}

void NewGameState::buttonVetClick(SDL_Event *ev)
{
	_game->setState(new GeoscapeState(_game));
}

void NewGameState::buttonGenClick(SDL_Event *ev)
{
	_game->setState(new GeoscapeState(_game));
}

void NewGameState::buttonSupClick(SDL_Event *ev)
{
	_game->setState(new GeoscapeState(_game));
}