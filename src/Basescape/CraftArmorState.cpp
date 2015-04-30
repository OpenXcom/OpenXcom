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
#include "CraftArmorState.h"
#include <string>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Engine/Action.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/Armor.h"
#include "SoldierArmorState.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/ItemContainer.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Craft Armor screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param craft ID of the selected craft.
 */
CraftArmorState::CraftArmorState(Base *base, size_t craft) : _base(base), _craft(craft)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(288, 16, 16, 176);
	_txtTitle = new Text(300, 17, 16, 7);
	_txtName = new Text(114, 9, 16, 32);
	_txtCraft = new Text(76, 9, 130, 32);
	_txtArmor = new Text(100, 9, 204, 32);
	_lstSoldiers = new TextList(288, 128, 8, 40);

	// Set palette
	setInterface("craftArmor");

	add(_window, "window", "craftArmor");
	add(_btnOk, "button", "craftArmor");
	add(_txtTitle, "text", "craftArmor");
	add(_txtName, "text", "craftArmor");
	add(_txtCraft, "text", "craftArmor");
	add(_txtArmor, "text", "craftArmor");
	add(_lstSoldiers, "list", "craftArmor");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CraftArmorState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CraftArmorState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_SELECT_ARMOR"));

	_txtName->setText(tr("STR_NAME_UC"));

	_txtCraft->setText(tr("STR_CRAFT"));

	_txtArmor->setText(tr("STR_ARMOR"));

	_lstSoldiers->setColumns(3, 114, 74, 92);
	_lstSoldiers->setSelectable(true);
	_lstSoldiers->setBackground(_window);
	_lstSoldiers->setMargin(8);
	_lstSoldiers->onMousePress((ActionHandler)&CraftArmorState::lstSoldiersClick);

	Uint8 otherCraftColor = _game->getRuleset()->getInterface("craftArmor")->getElement("otherCraft")->color;
	int row = 0;
	Craft *c = _base->getCrafts()->at(_craft);
	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
	{
		_lstSoldiers->addRow(3, (*i)->getName(true).c_str(), (*i)->getCraftString(_game->getLanguage()).c_str(), tr((*i)->getArmor()->getType()).c_str());

		Uint8 color;
		if ((*i)->getCraft() == c)
		{
			color = _lstSoldiers->getSecondaryColor();
		}
		else if ((*i)->getCraft() != 0)
		{
			color = otherCraftColor;
		}
		else
		{
			color = _lstSoldiers->getColor();
		}
		_lstSoldiers->setRowColor(row, color);
		row++;
	}
}

/**
 *
 */
CraftArmorState::~CraftArmorState()
{
}

/**
 * The soldier armors can change
 * after going into other screens.
 */
void CraftArmorState::init()
{
	State::init();
	int row = 0;
	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
	{
		_lstSoldiers->setCellText(row, 2, tr((*i)->getArmor()->getType()));
		row++;
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CraftArmorState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Shows the Select Armor window.
 * @param action Pointer to an action.
 */
void CraftArmorState::lstSoldiersClick(Action *action)
{
	Soldier *s = _base->getSoldiers()->at(_lstSoldiers->getSelectedRow());
	if (!(s->getCraft() && s->getCraft()->getStatus() == "STR_OUT"))
	{
		if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
		{
			_game->pushState(new SoldierArmorState(_base, _lstSoldiers->getSelectedRow()));
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		{
			SavedGame *save;
			save = _game->getSavedGame();
			Armor *a = _game->getRuleset()->getArmor(save->getLastSelectedArmor());
			if (save->getMonthsPassed() != -1)
			{
				if (_base->getItems()->getItem(a->getStoreItem()) > 0 || a->getStoreItem() == "STR_NONE")
				{
					if (s->getArmor()->getStoreItem() != "STR_NONE")
					{
						_base->getItems()->addItem(s->getArmor()->getStoreItem());
					}
					if (a->getStoreItem() != "STR_NONE")
					{
						_base->getItems()->removeItem(a->getStoreItem());
					}

					s->setArmor(a);
					_lstSoldiers->setCellText(_lstSoldiers->getSelectedRow(), 2, tr(a->getType()));
				}
			}
			else
			{
				s->setArmor(a);
				_lstSoldiers->setCellText(_lstSoldiers->getSelectedRow(), 2, tr(a->getType()));
			}
		}
	}
}

}
