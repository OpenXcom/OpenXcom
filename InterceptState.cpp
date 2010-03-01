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
#include "InterceptState.h"

InterceptState::InterceptState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(256, 160, 32, 20);
	_btnCancel = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 192, 20, 64, 90);
	_txtTitle = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 256, 30, 32, 45);
	
	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBg(game->getSurface("BACK12.SCR"));

	_btnCancel->setColor(Palette::blockOffset(8)+8);
	_btnCancel->setText(_game->getLanguage()->getString(71));
	_btnCancel->onMouseClick((EventHandler)&InterceptState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString(264));
}

InterceptState::~InterceptState()
{
	State::~State();
}

void InterceptState::think()
{
}

void InterceptState::btnCancelClick(SDL_Event *ev, int scale)
{
	
}