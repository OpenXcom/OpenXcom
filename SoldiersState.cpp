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
#include "SoldiersState.h"

SoldiersState::SoldiersState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(320, 200, 0, 0);
	_btnOk = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 288, 16, 16, 176);
	_txtTitle = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 310, 13, 5, 8);
	_txtName = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 16, 32);
	_txtRank = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 102, 9, 130, 32);
	_txtCraft = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 82, 9, 232, 32);
	_lstSoldiers = new TextList(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 298, 136, 16, 40);
	
	// Set palette
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtName);
	add(_txtRank);
	add(_txtCraft);
	add(_lstSoldiers);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+4);
	_window->setBg(game->getSurface("BACK02.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getLanguage()->getString(76));
	_btnOk->onMouseClick((EventHandler)&SoldiersState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString(803));

	_txtName->setColor(Palette::blockOffset(15)+1);
	_txtName->setText(_game->getLanguage()->getString(320));

	_txtRank->setColor(Palette::blockOffset(15)+1);
	_txtRank->setText(_game->getLanguage()->getString(336));

	_txtCraft->setColor(Palette::blockOffset(15)+1);
	_txtCraft->setText(_game->getLanguage()->getString(337));

	_lstSoldiers->setColor(Palette::blockOffset(13)+10);
	_lstSoldiers->setColumns(3, 114, 102, 82);
	_lstSoldiers->addRow(3, "Some Guy", _game->getLanguage()->getString(327).c_str(), "SKYRANGER-1");
}

SoldiersState::~SoldiersState()
{
	State::~State();
}

void SoldiersState::think()
{
}

void SoldiersState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->setState(new BasescapeState(_game));
}