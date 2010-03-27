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
	_btnBeginner = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 55);
	_btnExperienced = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 80);
	_btnVeteran = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 105);
	_btnGenius = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 130);
	_btnSuperhuman = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 155);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 10, 64, 30);
	
	add(_window);
	add(_btnBeginner);
	add(_btnExperienced);
	add(_btnVeteran);
	add(_btnGenius);
	add(_btnSuperhuman);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBg(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnBeginner->setColor(Palette::blockOffset(8)+8);
	_btnBeginner->setText(_game->getResourcePack()->getLanguage()->getString(STR_BEGINNER));
	_btnBeginner->onMouseClick((EventHandler)&NewGameState::btnBeginnerClick);

	_btnExperienced->setColor(Palette::blockOffset(8)+8);
	_btnExperienced->setText(_game->getResourcePack()->getLanguage()->getString(STR_EXPERIENCED));
	_btnExperienced->onMouseClick((EventHandler)&NewGameState::btnExperiencedClick);

	_btnVeteran->setColor(Palette::blockOffset(8)+8);
	_btnVeteran->setText(_game->getResourcePack()->getLanguage()->getString(STR_VETERAN));
	_btnVeteran->onMouseClick((EventHandler)&NewGameState::btnVeteranClick);

	_btnGenius->setColor(Palette::blockOffset(8)+8);
	_btnGenius->setText(_game->getResourcePack()->getLanguage()->getString(STR_GENIUS));
	_btnGenius->onMouseClick((EventHandler)&NewGameState::btnGeniusClick);

	_btnSuperhuman->setColor(Palette::blockOffset(8)+8);
	_btnSuperhuman->setText(_game->getResourcePack()->getLanguage()->getString(STR_SUPERHUMAN));
	_btnSuperhuman->onMouseClick((EventHandler)&NewGameState::btnSuperhumanClick);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setSmall();
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_SELECT_DIFFICULTY_LEVEL));
}

NewGameState::~NewGameState()
{
	
}

void NewGameState::think()
{
}

void NewGameState::btnBeginnerClick(SDL_Event *ev, int scale)
{
	_game->setSavedGame(new SavedGame());
	_game->setState(new GeoscapeState(_game));
}

void NewGameState::btnExperiencedClick(SDL_Event *ev, int scale)
{
	_game->setSavedGame(new SavedGame());
	_game->setState(new GeoscapeState(_game));
}

void NewGameState::btnVeteranClick(SDL_Event *ev, int scale)
{
	_game->setSavedGame(new SavedGame());
	_game->setState(new GeoscapeState(_game));
}

void NewGameState::btnGeniusClick(SDL_Event *ev, int scale)
{
	_game->setSavedGame(new SavedGame());
	_game->setState(new GeoscapeState(_game));
}

void NewGameState::btnSuperhumanClick(SDL_Event *ev, int scale)
{
	_game->setSavedGame(new SavedGame());
	_game->setState(new GeoscapeState(_game));
}