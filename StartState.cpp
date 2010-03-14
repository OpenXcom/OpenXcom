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
	_btnEnglish = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 90);
	_btnDeutsche = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 118);
	_btnFrench = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 146);
	
	// Set palette
	_game->setPalette(_game->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
	
	add(_window);
	add(_btnEnglish);
	add(_btnDeutsche);
	add(_btnFrench);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBg(game->getSurface("BACK01.SCR"));

	_btnEnglish->setColor(Palette::blockOffset(8)+8);
	_btnEnglish->setText("ENGLISH");
	_btnEnglish->onMouseClick((EventHandler)&StartState::btnEnglishClick);
	
	_btnDeutsche->setColor(Palette::blockOffset(8)+8);
	_btnDeutsche->setText("DEUTSCHE");
	_btnDeutsche->onMouseClick((EventHandler)&StartState::btnDeutscheClick);

	_btnFrench->setColor(Palette::blockOffset(8)+8);
	_btnFrench->setText("FRANCAIS");
	_btnFrench->onMouseClick((EventHandler)&StartState::btnFrenchClick);
}

StartState::~StartState()
{
	
}

void StartState::think()
{
}

void StartState::btnEnglishClick(SDL_Event *ev, int scale)
{
	_game->setLanguage("ENGLISH.DAT");
	_game->setState(new MainMenuState(_game));
}

void StartState::btnDeutscheClick(SDL_Event *ev, int scale)
{
	_game->setLanguage("GERMAN.DAT");
	_game->setState(new MainMenuState(_game));
}

void StartState::btnFrenchClick(SDL_Event *ev, int scale)
{
	_game->setLanguage("FRENCH.DAT");
	_game->setState(new MainMenuState(_game));
}