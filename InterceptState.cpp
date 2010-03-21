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
	_txtCraft = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 86, 9, 14, 70);
	_txtStatus = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 65, 9, 100, 70);
	_txtBase = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 85, 9, 165, 70);
	_txtWeapons = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 64, 16, 242, 62);
	_lstCrafts = new TextList(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 300, 64, 14, 78);
	
	// Set palette
	_game->setPalette(_game->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtCraft);
	add(_txtStatus);
	add(_txtBase);
	add(_txtWeapons);
	add(_lstCrafts);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBg(game->getSurface("BACK12.SCR"));

	_btnCancel->setColor(Palette::blockOffset(8)+8);
	_btnCancel->setText(_game->getLanguage()->getString(STR_CANCEL));
	_btnCancel->onMouseClick((EventHandler)&InterceptState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString(STR_LAUNCH_INTERCEPTION));

	_txtCraft->setColor(Palette::blockOffset(8)+5);
	_txtCraft->setText(_game->getLanguage()->getString(STR_CRAFT));

	_txtStatus->setColor(Palette::blockOffset(8)+5);
	_txtStatus->setText(_game->getLanguage()->getString(STR_STATUS));

	_txtBase->setColor(Palette::blockOffset(8)+5);
	_txtBase->setText(_game->getLanguage()->getString(STR_BASE));

	_txtWeapons->setColor(Palette::blockOffset(8)+5);
	_txtWeapons->setText(_game->getLanguage()->getString(STR_WEAPONS_CREW_HWPS));

	_lstCrafts->setColor(Palette::blockOffset(15)-1);
	_lstCrafts->setColumns(4, 86, 65, 85, 64);
	_lstCrafts->addRow(4, "SKYRANGER-1", _game->getLanguage()->getString(268).c_str(), "Base", "8/0/0");
	_lstCrafts->getCell(0, 1)->setColor(Palette::blockOffset(8)+10);
}

InterceptState::~InterceptState()
{
	
}

void InterceptState::think()
{
}

void InterceptState::btnCancelClick(SDL_Event *ev, int scale)
{
	_game->setState(new GeoscapeState(_game));
}