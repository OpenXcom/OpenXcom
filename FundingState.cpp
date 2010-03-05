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
#include "FundingState.h"

FundingState::FundingState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(320, 200, 0, 00);
	_btnOk = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 50, 12, 135, 180);
	_txtTitle = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 280, 15, 32, 8);
	_lstCountries = new TextList(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 280, 136, 32, 40);
	
	// Set palette
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_lstCountries);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBg(game->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+2);
	_btnOk->setText(_game->getLanguage()->getString(76));
	_btnOk->onMouseClick((EventHandler)&FundingState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString(638));

	_lstCountries->setColor(Palette::blockOffset(15)-1);
	_lstCountries->setColumns(3, 108, 100, 72);
	_lstCountries->addRow(3, "TEEEEEST", "TEEEEST", "TEEEEST");
}

FundingState::~FundingState()
{
	State::~State();
}

void FundingState::think()
{
}

void FundingState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->setState(new GeoscapeState(_game));
}