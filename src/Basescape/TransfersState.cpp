/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "TransfersState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Transfer.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Transfers window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
TransfersState::TransfersState(Game *game, Base *base) : State(game), _base(base)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 184, 0, 8, POPUP_BOTH);
	_btnOk = new TextButton(288, 16, 16, 166);
	_txtTitle = new Text(278, 16, 21, 18);
	_txtItem = new Text(114, 8, 16, 34);
	_txtQuantity = new Text(54, 8, 152, 34);
	_txtArrivalTime = new Text(112, 8, 212, 34);
	_lstTransfers = new TextList(271, 112, 16, 50);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtArrivalTime);
	add(_lstTransfers);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+6);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+6);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&TransfersState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)+6);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_TRANSFERS"));

	_txtItem->setColor(Palette::blockOffset(15)+6);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtQuantity->setColor(Palette::blockOffset(15)+6);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_txtArrivalTime->setColor(Palette::blockOffset(15)+6);
	_txtArrivalTime->setText(_game->getLanguage()->getString("STR_ARRIVAL_TIME_HOURS"));

	_lstTransfers->setColor(Palette::blockOffset(13)+10);
	_lstTransfers->setArrowColor(Palette::blockOffset(15)+6);
	_lstTransfers->setColumns(3, 155, 75, 46);
	_lstTransfers->setSelectable(true);
	_lstTransfers->setBackground(_window);
	_lstTransfers->setMargin(2);

	for (std::vector<Transfer*>::iterator i = _base->getTransfers()->begin(); i != _base->getTransfers()->end(); ++i)
	{
		std::wstringstream ss, ss2;
		ss << (*i)->getQuantity();
		ss2 << (*i)->getHours();
		_lstTransfers->addRow(3, (*i)->getName(_game->getLanguage()).c_str(), ss.str().c_str(), ss2.str().c_str());
	}
}

/**
 *
 */
TransfersState::~TransfersState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void TransfersState::btnOkClick(Action *action)
{
	_game->popState();
}

}
