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
	_screen = false;

	// Create objects
	_window = new Window(216, 160, 20, 20);
	_btnYes = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 50, 20, 38, 140);
	_btnNo = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 50, 20, 168, 140);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 206, 15, 25, 70);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnYes);
	add(_btnNo);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBg(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnYes->setColor(Palette::blockOffset(15)+2);
	_btnYes->setText(_game->getResourcePack()->getLanguage()->getString(STR_YES));
	_btnYes->onMouseClick((EventHandler)&AbandonGameState::btnYesClick);

	_btnNo->setColor(Palette::blockOffset(15)+2);
	_btnNo->setText(_game->getResourcePack()->getLanguage()->getString(STR_NO));
	_btnNo->onMouseClick((EventHandler)&AbandonGameState::btnNoClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	stringstream ss;
	ss << _game->getResourcePack()->getLanguage()->getString(STR_ABANDON_GAME) << "?";
	_txtTitle->setText(ss.str());
}

AbandonGameState::~AbandonGameState()
{
	
}

void AbandonGameState::think()
{
}

void AbandonGameState::btnYesClick(SDL_Event *ev, int scale)
{
	delete _game->getSavedGame();
	_game->setState(new MainMenuState(_game));
}

void AbandonGameState::btnNoClick(SDL_Event *ev, int scale)
{
	_game->popState();
}