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
#include "ManufactureState.h"

ManufactureState::ManufactureState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(320, 200, 0, 0);
	_btnNew = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 148, 16, 8, 176);
	_btnOk = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 148, 16, 164, 176);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 310, 13, 5, 8);
	_txtAvailable = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 150, 9, 8, 24);
	_txtAllocated = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 150, 9, 160, 24);
	_txtSpace = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 150, 9, 8, 34);
	_txtFunds = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 150, 9, 160, 34);
	_txtItem = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 80, 9, 10, 52);
	_txtEngineers = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 46, 18, 90, 44);
	_txtProduced = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 42, 18, 137, 44);
	_txtTotal = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 42, 18, 180, 44);
	_txtCost = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 42, 27, 223, 44);
	_txtTimeLeft = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 54, 18, 265, 44);
	_lstManufacture = new TextList(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 300, 100, 9, 80);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnNew);
	add(_btnOk);
	add(_txtTitle);
	add(_txtAvailable);
	add(_txtAllocated);
	add(_txtSpace);
	add(_txtFunds);
	add(_txtItem);
	add(_txtEngineers);
	add(_txtProduced);
	add(_txtTotal);
	add(_txtCost);
	add(_txtTimeLeft);
	add(_lstManufacture);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+9);
	_window->setBg(game->getResourcePack()->getSurface("BACK17.SCR"));
	
	_btnNew->setColor(Palette::blockOffset(13)+13);
	_btnNew->setText(_game->getResourcePack()->getLanguage()->getString(STR_NEW_PRODUCTION));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&ManufactureState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)+6);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_CURRENT_PRODUCTION));

	_txtAvailable->setColor(Palette::blockOffset(15)+6);
	_txtAvailable->setText(_game->getResourcePack()->getLanguage()->getString(STR_ENGINEERS_AVAILABLE));

	_txtAllocated->setColor(Palette::blockOffset(15)+6);
	_txtAllocated->setText(_game->getResourcePack()->getLanguage()->getString(STR_ENGINEERS_ALLOCATED_UC));

	_txtSpace->setColor(Palette::blockOffset(15)+6);
	_txtSpace->setText(_game->getResourcePack()->getLanguage()->getString(STR_WORKSHOP_SPACE_AVAILABLE));

	_txtFunds->setColor(Palette::blockOffset(15)+6);
	_txtFunds->setText(_game->getResourcePack()->getLanguage()->getString(STR_CURRENT_FUNDS));

	_txtItem->setColor(Palette::blockOffset(15)+1);
	_txtItem->setText(_game->getResourcePack()->getLanguage()->getString(STR_ITEM));

	_txtEngineers->setColor(Palette::blockOffset(15)+1);
	_txtEngineers->setText(_game->getResourcePack()->getLanguage()->getString(STR_ENGINEERS_ALLOCATED));

	_txtProduced->setColor(Palette::blockOffset(15)+1);
	_txtProduced->setText(_game->getResourcePack()->getLanguage()->getString(STR_UNITS_PRODUCED));

	_txtTotal->setColor(Palette::blockOffset(15)+1);
	_txtTotal->setText(_game->getResourcePack()->getLanguage()->getString(STR_TOTAL_TO_PRODUCE));

	_txtCost->setColor(Palette::blockOffset(15)+1);
	_txtCost->setText(_game->getResourcePack()->getLanguage()->getString(STR_COST_PER_UNIT));

	_txtTimeLeft->setColor(Palette::blockOffset(15)+1);
	_txtTimeLeft->setText(_game->getResourcePack()->getLanguage()->getString(STR_DAYS_HOURS_LEFT));
	
	_lstManufacture->setColor(Palette::blockOffset(13)+10);
	_lstManufacture->setColumns(6, 105, 39, 45, 27, 47, 37);
	_lstManufacture->addRow(6, "Laser Rifle", "30", "2", "8", "$40 000", "5/2");
}

ManufactureState::~ManufactureState()
{
	
}

void ManufactureState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->popState();
}