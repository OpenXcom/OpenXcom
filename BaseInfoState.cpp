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
#include "BaseInfoState.h"

BaseInfoState::BaseInfoState(Game *game) : State(game)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_btnOk = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 30, 14, 10, 180);
	_btnTransfers = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 80, 14, 46, 180);
	_btnStores = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 80, 14, 132, 180);
	_btnMonthlyCosts = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 92, 14, 218, 180);
	_txtBase = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 170, 16, 8, 8);
	_txtPersonnel = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 300, 9, 8, 30);
	_txtSoldiers = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 41);
	_txtSpace = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 300, 9, 8, 72);
	
	add(_bg);
	add(_btnOk);
	add(_btnTransfers);
	add(_btnStores);
	add(_btnMonthlyCosts);
	add(_txtBase);
	add(_txtPersonnel);
	add(_txtSoldiers);
	add(_txtSpace);

	// Set up objects
	_game->getSurface("BACK07.SCR")->blit(_bg);

	_btnOk->setColor(Palette::blockOffset(15)+9);
	_btnOk->setText(_game->getLanguage()->getString(76));
	_btnOk->onMouseClick((EventHandler)&BaseInfoState::btnOkClick);

	_btnTransfers->setColor(Palette::blockOffset(15)+9);
	_btnTransfers->setText(_game->getLanguage()->getString(140));

	_btnStores->setColor(Palette::blockOffset(15)+9);
	_btnStores->setText(_game->getLanguage()->getString(934));

	_btnMonthlyCosts->setColor(Palette::blockOffset(15)+9);
	_btnMonthlyCosts->setText(_game->getLanguage()->getString(954));

	_txtBase->setColor(Palette::blockOffset(15)+1);
	_txtBase->setBig();
	_txtBase->setText("Some Base");

	_txtPersonnel->setColor(Palette::blockOffset(15)+1);
	_txtPersonnel->setText(_game->getLanguage()->getString(128));

	_txtSoldiers->setColor(Palette::blockOffset(13)+5);
	_txtSoldiers->setText(_game->getLanguage()->getString(129));

	_txtSpace->setColor(Palette::blockOffset(15)+1);
	_txtSpace->setText(_game->getLanguage()->getString(132));
}

BaseInfoState::~BaseInfoState()
{
	
}

void BaseInfoState::think()
{
}

void BaseInfoState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->setState(new BasescapeState(_game));
}