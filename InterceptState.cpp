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
	_window = new Window(320, 140, 0, 30);
	_btnCancel = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 288, 16, 16, 146);
	_txtTitle = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 300, 16, 10, 46);
	_lstCrafts = new TextList(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 300, 74, 14, 70);
	
	// Set palette
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_lstCrafts);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBg(game->getSurface("BACK12.SCR"));

	_btnCancel->setColor(Palette::blockOffset(8)+8);
	_btnCancel->setText(_game->getLanguage()->getString(71));
	_btnCancel->onMouseClick((EventHandler)&InterceptState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString(264));

	_lstCrafts->setColor(Palette::blockOffset(15)-1);
	_lstCrafts->setColumns(4, 86, 65, 85, 64);
	_lstCrafts->addRow(4, "TEEEEEST", "TEEEEST", "TEEEEST", "TEEEEEST");
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
	_game->setState(new GeoscapeState(_game));
}