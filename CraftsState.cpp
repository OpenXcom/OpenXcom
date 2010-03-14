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
#include "CraftsState.h"

CraftsState::CraftsState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(320, 200, 0, 0);
	_btnOk = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 288, 16, 16, 176);
	_txtTitle = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 298, 13, 16, 8);
	_txtBase = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 298, 13, 16, 24);
	_txtName = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 94, 13, 16, 40);
	_txtStatus = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 50, 13, 110, 40);
	_txtWeapon = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 50, 16, 160, 40);
	_txtCrew = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 58, 13, 210, 40);
	_txtHwp = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 46, 13, 268, 40);
	_lstCrafts = new TextList(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 298, 118, 16, 58);
	
	// Set palette
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtBase);
	add(_txtName);
	add(_txtStatus);
	add(_txtWeapon);
	add(_txtCrew);
	add(_txtHwp);
	add(_lstCrafts);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+4);
	_window->setBg(game->getSurface("BACK14.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getLanguage()->getString(76));
	_btnOk->onMouseClick((EventHandler)&CraftsState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)+1);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString(318));

	_txtBase->setColor(Palette::blockOffset(15)+1);
	_txtBase->setBig();
	_txtBase->setText(_game->getLanguage()->getString(319));

	_txtName->setColor(Palette::blockOffset(15)+1);
	_txtName->setText(_game->getLanguage()->getString(320));

	_txtStatus->setColor(Palette::blockOffset(15)+1);
	_txtStatus->setText(_game->getLanguage()->getString(321));

	_txtWeapon->setColor(Palette::blockOffset(15)+1);
	_txtWeapon->setText(_game->getLanguage()->getString(642));

	_txtCrew->setColor(Palette::blockOffset(15)+1);
	_txtCrew->setText(_game->getLanguage()->getString(643));

	_txtHwp->setColor(Palette::blockOffset(15)+1);
	_txtHwp->setText(_game->getLanguage()->getString(644));

	_lstCrafts->setColor(Palette::blockOffset(13)+10);
	_lstCrafts->setColumns(5, 94, 66, 47, 46, 0);
	_lstCrafts->addRow(5, "SKYRANGER-1", _game->getLanguage()->getString(268).c_str(), "0/0", "8", "0");
}

CraftsState::~CraftsState()
{
	State::~State();
}

void CraftsState::think()
{
}

void CraftsState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->setState(new BasescapeState(_game));
}