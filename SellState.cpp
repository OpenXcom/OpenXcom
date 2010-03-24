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
#include "SellState.h"

SellState::SellState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(320, 200, 0, 0);
	_btnOk = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 148, 16, 8, 176);
	_btnCancel = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 148, 16, 164, 176);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 310, 13, 5, 8);
	_txtSales = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 190, 9, 10, 24);
	_txtFunds = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 200, 24);
	_txtItem = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 130, 9, 10, 32);
	_txtQuantity = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 44, 9, 140, 32);
	_txtSell = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 96, 9, 184, 32);
	_txtValue = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 34, 9, 280, 32);
	_lstItems = new TextList(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 300, 120, 10, 44);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtSales);
	add(_txtFunds);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtSell);
	add(_txtValue);
	add(_lstItems);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+13);
	_window->setBg(game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_SELL_SACK));
	_btnOk->onMouseClick((EventHandler)&SellState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(13)+13);
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString(STR_CANCEL));
	_btnCancel->onMouseClick((EventHandler)&SellState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_SELL_ITEMS_SACK_PERSONNEL));

	_txtSales->setColor(Palette::blockOffset(13)+10);
	_txtSales->setText(_game->getResourcePack()->getLanguage()->getString(STR_VALUE_OF_SALES));

	_txtFunds->setColor(Palette::blockOffset(13)+10);
	_txtFunds->setText(_game->getResourcePack()->getLanguage()->getString(STR_FUNDS));

	_txtItem->setColor(Palette::blockOffset(13)+10);
	_txtItem->setText(_game->getResourcePack()->getLanguage()->getString(STR_ITEM));

	_txtQuantity->setColor(Palette::blockOffset(13)+10);
	_txtQuantity->setText(_game->getResourcePack()->getLanguage()->getString(STR_QUANTITY));

	_txtSell->setColor(Palette::blockOffset(13)+10);
	_txtSell->setText(_game->getResourcePack()->getLanguage()->getString(STR_SELL_SACK));

	_txtValue->setColor(Palette::blockOffset(13)+10);
	_txtValue->setText(_game->getResourcePack()->getLanguage()->getString(STR_VALUE));

	_lstItems->setColor(Palette::blockOffset(13)+10);
	_lstItems->setColumns(4, 156, 62, 28, 54);
	_lstItems->addRow(4, "Soldier", "1", "0", "$40 000");
}

SellState::~SellState()
{
	
}

void SellState::think()
{
}

void SellState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->setState(new BasescapeState(_game));
}

void SellState::btnCancelClick(SDL_Event *ev, int scale)
{
	_game->setState(new BasescapeState(_game));
}