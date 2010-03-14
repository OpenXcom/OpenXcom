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
	_window = new Window(320, 200, 0, 0);
	_btnOk = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 50, 12, 135, 180);
	_txtTitle = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 280, 16, 32, 8);
	_txtCountry = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 100, 16, 32, 24);
	_txtFunding = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 60, 16, 140, 24);
	_txtChange = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 120, 16, 200, 24);
	_lstCountries = new TextList(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 280, 136, 32, 40);
	
	// Set palette
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtCountry);
	add(_txtFunding);
	add(_txtChange);
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

	_txtCountry->setColor(Palette::blockOffset(15)-1);
	_txtCountry->setBig();
	_txtCountry->setText(_game->getLanguage()->getString(639));

	_txtFunding->setColor(Palette::blockOffset(15)-1);
	_txtFunding->setBig();
	_txtFunding->setText(_game->getLanguage()->getString(640));

	_txtChange->setColor(Palette::blockOffset(15)-1);
	_txtChange->setBig();
	_txtChange->setText(_game->getLanguage()->getString(641));

	_lstCountries->setColor(Palette::blockOffset(15)-1);
	_lstCountries->setColumns(3, 108, 100, 72);
	_lstCountries->setDot(true);
	_lstCountries->addRow(3, _game->getLanguage()->getString(601).c_str(), "$800 000", "0");
	_lstCountries->addRow(3, _game->getLanguage()->getString(602).c_str(), "$300 000", "0");
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