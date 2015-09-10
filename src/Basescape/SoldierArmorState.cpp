/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "SoldierArmorState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Mod/Armor.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"
#include "../Mod/Mod.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Armor window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param soldier ID of the selected soldier.
 */
SoldierArmorState::SoldierArmorState(Base *base, size_t soldier) : _base(base), _soldier(soldier)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 192, 120, 64, 40, POPUP_BOTH);
	_btnCancel = new TextButton(140, 16, 90, 136);
	_txtTitle = new Text(182, 16, 69, 48);
	_txtType = new Text(90, 9, 80, 72);
	_txtQuantity = new Text(70, 9, 190, 72);
	_lstArmor = new TextList(160, 40, 73, 88);

	// Set palette
	setInterface("soldierArmor");

	add(_window, "window", "soldierArmor");
	add(_btnCancel, "button", "soldierArmor");
	add(_txtTitle, "text", "soldierArmor");
	add(_txtType, "text", "soldierArmor");
	add(_txtQuantity, "text", "soldierArmor");
	add(_lstArmor, "list", "soldierArmor");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK14.SCR"));

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&SoldierArmorState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&SoldierArmorState::btnCancelClick, Options::keyCancel);

	Soldier *s = _base->getSoldiers()->at(_soldier);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_SELECT_ARMOR_FOR_SOLDIER").arg(s->getName()));

	_txtType->setText(tr("STR_TYPE"));

	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_lstArmor->setColumns(2, 132, 21);
	_lstArmor->setSelectable(true);
	_lstArmor->setBackground(_window);
	_lstArmor->setMargin(8);

	const std::vector<std::string> &armors = _game->getMod()->getArmorsList();
	for (std::vector<std::string>::const_iterator i = armors.begin(); i != armors.end(); ++i)
	{
		Armor *a = _game->getMod()->getArmor(*i);
		if (_base->getItems()->getItem(a->getStoreItem()) > 0)
		{
			_armors.push_back(a);
			std::wostringstream ss;
			if (_game->getSavedGame()->getMonthsPassed() > -1)
			{
				ss << _base->getItems()->getItem(a->getStoreItem());
			}
			else
			{
				ss << "-";
			}
			_lstArmor->addRow(2, tr(a->getType()).c_str(), ss.str().c_str());
		}
		else if (a->getStoreItem() == "STR_NONE")
		{
			_armors.push_back(a);
			_lstArmor->addRow(1, tr(a->getType()).c_str());
		}
	}
	_lstArmor->onMouseClick((ActionHandler)&SoldierArmorState::lstArmorClick);
}

/**
 *
 */
SoldierArmorState::~SoldierArmorState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierArmorState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Equips the armor on the soldier and returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierArmorState::lstArmorClick(Action *)
{
	Soldier *soldier = _base->getSoldiers()->at(_soldier);
	if (_game->getSavedGame()->getMonthsPassed() != -1)
	{
		if (soldier->getArmor()->getStoreItem() != "STR_NONE")
		{
			_base->getItems()->addItem(soldier->getArmor()->getStoreItem());
		}
		if (_armors[_lstArmor->getSelectedRow()]->getStoreItem() != "STR_NONE")
		{
			_base->getItems()->removeItem(_armors[_lstArmor->getSelectedRow()]->getStoreItem());
		}
	}
	soldier->setArmor(_armors[_lstArmor->getSelectedRow()]);
	SavedGame *_save;
	_save = _game->getSavedGame();
	_save->setLastSelectedArmor(_armors[_lstArmor->getSelectedRow()]->getType());

	_game->popState();
}

}
