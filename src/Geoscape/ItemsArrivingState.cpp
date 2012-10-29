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
#include "ItemsArrivingState.h"
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
#include "../Savegame/Craft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "GeoscapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Items Arriving window.
 * @param game Pointer to the core game.
 * @param state Pointer to the Geoscape state.
 */
ItemsArrivingState::ItemsArrivingState(Game *game, GeoscapeState *state) : State(game), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 184, 0, 8, POPUP_BOTH);
	_btnOk = new TextButton(142, 16, 16, 166);
	_btnOk5Secs = new TextButton(142, 16, 162, 166);
	_txtTitle = new Text(310, 16, 5, 18);
	_txtItem = new Text(114, 8, 16, 34);
	_txtQuantity = new Text(54, 8, 152, 34);
	_txtDestination = new Text(112, 8, 212, 34);
	_lstTransfers = new TextList(271, 112, 14, 50);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnOk5Secs);
	add(_txtTitle);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtDestination);
	add(_lstTransfers);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ItemsArrivingState::btnOkClick);

	_btnOk5Secs->setColor(Palette::blockOffset(8)+5);
	_btnOk5Secs->setText(_game->getLanguage()->getString("STR_OK_5_SECS"));
	_btnOk5Secs->onMouseClick((ActionHandler)&ItemsArrivingState::btnOk5SecsClick);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_ITEMS_ARRIVING"));

	_txtItem->setColor(Palette::blockOffset(8)+5);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtQuantity->setColor(Palette::blockOffset(8)+5);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_txtDestination->setColor(Palette::blockOffset(8)+5);
	_txtDestination->setText(_game->getLanguage()->getString("STR_DESTINATION_UC"));

	_lstTransfers->setColor(Palette::blockOffset(8)+10);
	_lstTransfers->setColumns(3, 155, 35, 98);
	_lstTransfers->setSelectable(true);
	_lstTransfers->setBackground(_window);
	_lstTransfers->setMargin(2);

	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Transfer*>::iterator j = (*i)->getTransfers()->begin(); j != (*i)->getTransfers()->end();)
		{
			if ((*j)->getHours() == 0)
			{
				// Check if it's ammo to reload a craft
				if ((*j)->getType() == TRANSFER_ITEM && _game->getRuleset()->getItem((*j)->getItems())->getBattleType() == BT_NONE)
				{
					for (std::vector<Craft*>::iterator c = (*i)->getCrafts()->begin(); c != (*i)->getCrafts()->end(); ++c)
					{
						if ((*c)->getStatus() != "STR_READY")
							continue;
						for (std::vector<CraftWeapon*>::iterator w = (*c)->getWeapons()->begin(); w != (*c)->getWeapons()->end(); ++w)
						{
						
							if ((*w) != 0 && (*w)->getAmmo() < (*w)->getRules()->getAmmoMax())
							{
								(*w)->setRearming(true);
								(*c)->setStatus("STR_REARMING");
							}
						}
					}
				}

				// Remove transfer
				std::wstringstream ss;
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
 * Resets the palette.
 */
void ItemsArrivingState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ItemsArrivingState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Reduces the speed to 5 Secs and returns to the previous screen.
 * @param action Pointer to an action.
 */
void ItemsArrivingState::btnOk5SecsClick(Action *action)
{
	_state->timerReset();
	_game->popState();
}

}
