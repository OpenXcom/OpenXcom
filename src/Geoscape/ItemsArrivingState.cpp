/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "ItemsArrivingState.h"
#include <sstream>
#include <algorithm>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/Transfer.h"
#include "../Savegame/Craft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Savegame/Vehicle.h"
#include "../Mod/RuleItem.h"
#include "../Mod/RuleCraftWeapon.h"
#include "GeoscapeState.h"
#include "../Engine/Options.h"
#include "../Basescape/BasescapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Items Arriving window.
 * @param game Pointer to the core game.
 * @param state Pointer to the Geoscape state.
 */
ItemsArrivingState::ItemsArrivingState(GeoscapeState *state) : _state(state), _base(0)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 184, 0, 8, POPUP_BOTH);
	_btnOk = new TextButton(142, 16, 16, 166);
	_btnGotoBase = new TextButton(142, 16, 162, 166);
	_txtTitle = new Text(310, 17, 5, 18);
	_txtItem = new Text(114, 9, 16, 34);
	_txtQuantity = new Text(54, 9, 152, 34);
	_txtDestination = new Text(112, 9, 212, 34);
	_lstTransfers = new TextList(271, 112, 14, 50);

	// Set palette
	setInterface("itemsArriving");

	add(_window, "window", "itemsArriving");
	add(_btnOk, "button", "itemsArriving");
	add(_btnGotoBase, "button", "itemsArriving");
	add(_txtTitle, "text1", "itemsArriving");
	add(_txtItem, "text1", "itemsArriving");
	add(_txtQuantity, "text1", "itemsArriving");
	add(_txtDestination, "text1", "itemsArriving");
	add(_lstTransfers, "text2", "itemsArriving");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK13.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ItemsArrivingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ItemsArrivingState::btnOkClick, Options::keyCancel);

	_btnGotoBase->setText(tr("STR_GO_TO_BASE"));
	_btnGotoBase->onMouseClick((ActionHandler)&ItemsArrivingState::btnGotoBaseClick);
	_btnGotoBase->onKeyboardPress((ActionHandler)&ItemsArrivingState::btnGotoBaseClick, Options::keyOk);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_ITEMS_ARRIVING"));

	_txtItem->setText(tr("STR_ITEM"));

	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_txtDestination->setText(tr("STR_DESTINATION_UC"));

	_lstTransfers->setColumns(3, 155, 41, 98);
	_lstTransfers->setSelectable(true);
	_lstTransfers->setBackground(_window);
	_lstTransfers->setMargin(2);

	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Transfer*>::iterator j = (*i)->getTransfers()->begin(); j != (*i)->getTransfers()->end();)
		{
			if ((*j)->getHours() == 0)
			{
				_base = (*i);

				// Check if we have an automated use for an item
				if ((*j)->getType() == TRANSFER_ITEM)
				{
					RuleItem *item = _game->getMod()->getItem((*j)->getItems(), true);
					if (item->getBattleType() == BT_NONE)
					{
						for (std::vector<Craft*>::iterator c = (*i)->getCrafts()->begin(); c != (*i)->getCrafts()->end(); ++c)
						{
							(*c)->reuseItem((*j)->getItems());
						}
					}
				}

				// Remove transfer
				std::wostringstream ss;
				ss << (*j)->getQuantity();
				_lstTransfers->addRow(3, (*j)->getName(_game->getLanguage()).c_str(), ss.str().c_str(), (*i)->getName().c_str());
				delete *j;
				j = (*i)->getTransfers()->erase(j);
			}
			else
			{
				++j;
			}
		}
	}
}

/**
 *
 */
ItemsArrivingState::~ItemsArrivingState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ItemsArrivingState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Goes to the base for the respective transfer.
 * @param action Pointer to an action.
 */
void ItemsArrivingState::btnGotoBaseClick(Action *)
{
	_state->timerReset();
	_game->popState();
	_game->pushState(new BasescapeState(_base, _state->getGlobe()));
}

}
