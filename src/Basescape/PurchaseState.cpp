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
#include "PurchaseState.h"
#include <algorithm>
#include <sstream>
#include <climits>
#include <iomanip>
#include "../fmath.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
#include "../Engine/Unicode.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/ComboBox.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Mod/RuleCraft.h"
#include "../Mod/RuleItem.h"
#include "../Savegame/Base.h"
#include "../Engine/Action.h"
#include "../Savegame/Craft.h"
#include "../Savegame/ItemContainer.h"
#include "../Menu/ErrorMessageState.h"
#include "../Mod/RuleInterface.h"
#include "../Mod/RuleSoldier.h"
#include "../Mod/RuleCraftWeapon.h"
#include "../Mod/Armor.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Purchase/Hire screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
PurchaseState::PurchaseState(Base *base) : _base(base), _sel(0), _total(0), _pQty(0), _cQty(0), _iQty(0.0), _ammoColor(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtFunds = new Text(150, 9, 10, 24);
	_txtPurchases = new Text(150, 9, 160, 24);
	_txtSpaceUsed = new Text(150, 9, 160, 34);
	_txtCost = new Text(102, 9, 152, 44);
	_txtQuantity = new Text(60, 9, 256, 44);
	_cbxCategory = new ComboBox(this, 120, 16, 10, 36);
	_lstItems = new TextList(287, 120, 8, 54);

	// Set palette
	setInterface("buyMenu");

	_ammoColor = _game->getMod()->getInterface("buyMenu")->getElement("ammoColor")->color;

	add(_window, "window", "buyMenu");
	add(_btnOk, "button", "buyMenu");
	add(_btnCancel, "button", "buyMenu");
	add(_txtTitle, "text", "buyMenu");
	add(_txtFunds, "text", "buyMenu");
	add(_txtPurchases, "text", "buyMenu");
	add(_txtSpaceUsed, "text", "buyMenu");
	add(_txtCost, "text", "buyMenu");
	add(_txtQuantity, "text", "buyMenu");
	add(_lstItems, "list", "buyMenu");
	add(_cbxCategory, "text", "buyMenu");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK13.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&PurchaseState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&PurchaseState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&PurchaseState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&PurchaseState::btnCancelClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_PURCHASE_HIRE_PERSONNEL"));

	_txtFunds->setText(tr("STR_CURRENT_FUNDS").arg(Unicode::formatFunding(_game->getSavedGame()->getFunds())));

	_txtPurchases->setText(tr("STR_COST_OF_PURCHASES").arg(Unicode::formatFunding(_total)));

	_txtSpaceUsed->setVisible(Options::storageLimitsEnforced);
	std::ostringstream ss;
	ss << _base->getUsedStores() << ":" << _base->getAvailableStores();
	_txtSpaceUsed->setText(tr("STR_SPACE_USED").arg(ss.str()));

	_txtCost->setText(tr("STR_COST_PER_UNIT_UC"));

	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_lstItems->setArrowColumn(227, ARROW_VERTICAL);
	_lstItems->setColumns(4, 150, 55, 50, 32);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);
	_lstItems->onLeftArrowPress((ActionHandler)&PurchaseState::lstItemsLeftArrowPress);
	_lstItems->onLeftArrowRelease((ActionHandler)&PurchaseState::lstItemsLeftArrowRelease);
	_lstItems->onLeftArrowClick((ActionHandler)&PurchaseState::lstItemsLeftArrowClick);
	_lstItems->onRightArrowPress((ActionHandler)&PurchaseState::lstItemsRightArrowPress);
	_lstItems->onRightArrowRelease((ActionHandler)&PurchaseState::lstItemsRightArrowRelease);
	_lstItems->onRightArrowClick((ActionHandler)&PurchaseState::lstItemsRightArrowClick);
	_lstItems->onMouseWheel((ActionHandler)&PurchaseState::lstItemsMousePress);

	_cats.push_back("STR_ALL_ITEMS");

	const std::vector<std::string> &cw = _game->getMod()->getCraftWeaponsList();
	for (std::vector<std::string>::const_iterator i = cw.begin(); i != cw.end(); ++i)
	{
		RuleCraftWeapon *rule = _game->getMod()->getCraftWeapon(*i);
		_craftWeapons.insert(rule->getLauncherItem());
		_craftWeapons.insert(rule->getClipItem());
	}
	const std::vector<std::string> &ar = _game->getMod()->getArmorsList();
	for (std::vector<std::string>::const_iterator i = ar.begin(); i != ar.end(); ++i)
	{
		Armor *rule = _game->getMod()->getArmor(*i);
		_armors.insert(rule->getStoreItem());
	}

	const std::vector<std::string> &soldiers = _game->getMod()->getSoldiersList();
	for (std::vector<std::string>::const_iterator i = soldiers.begin(); i != soldiers.end(); ++i)
	{
		RuleSoldier *rule = _game->getMod()->getSoldier(*i);
		if (rule->getBuyCost() != 0 && _game->getSavedGame()->isResearched(rule->getRequirements()))
		{
			TransferRow row = { TRANSFER_SOLDIER, rule, tr(rule->getType()), rule->getBuyCost(), _base->getSoldierCount(rule->getType()), 0, 0 };
			_items.push_back(row);
			std::string cat = getCategory(_items.size() - 1);
			if (std::find(_cats.begin(), _cats.end(), cat) == _cats.end())
			{
				_cats.push_back(cat);
			}
		}
	}
	{
		TransferRow row = { TRANSFER_SCIENTIST, 0, tr("STR_SCIENTIST"), _game->getMod()->getScientistCost() * 2, _base->getTotalScientists(), 0, 0 };
		_items.push_back(row);
		std::string cat = getCategory(_items.size() - 1);
		if (std::find(_cats.begin(), _cats.end(), cat) == _cats.end())
		{
			_cats.push_back(cat);
		}
	}
	{
		TransferRow row = { TRANSFER_ENGINEER, 0, tr("STR_ENGINEER"), _game->getMod()->getEngineerCost() * 2, _base->getTotalEngineers(), 0, 0 };
		_items.push_back(row);
		std::string cat = getCategory(_items.size() - 1);
		if (std::find(_cats.begin(), _cats.end(), cat) == _cats.end())
		{
			_cats.push_back(cat);
		}
	}
	const std::vector<std::string> &crafts = _game->getMod()->getCraftsList();
	for (std::vector<std::string>::const_iterator i = crafts.begin(); i != crafts.end(); ++i)
	{
		RuleCraft *rule = _game->getMod()->getCraft(*i);
		if (rule->getBuyCost() != 0 && _game->getSavedGame()->isResearched(rule->getRequirements()))
		{
			TransferRow row = { TRANSFER_CRAFT, rule, tr(rule->getType()), rule->getBuyCost(), _base->getCraftCount(rule->getType()), 0, 0 };
			_items.push_back(row);
			std::string cat = getCategory(_items.size() - 1);
			if (std::find(_cats.begin(), _cats.end(), cat) == _cats.end())
			{
				_cats.push_back(cat);
			}
		}
	}
	const std::vector<std::string> &items = _game->getMod()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		RuleItem *rule = _game->getMod()->getItem(*i);
		if (rule->getBuyCost() != 0 && _game->getSavedGame()->isResearched(rule->getRequirements()))
		{
			TransferRow row = { TRANSFER_ITEM, rule, tr(rule->getType()), rule->getBuyCost(), _base->getStorageItems()->getItem(rule->getType()), 0, 0 };
			_items.push_back(row);
			std::string cat = getCategory(_items.size() - 1);
			if (std::find(_cats.begin(), _cats.end(), cat) == _cats.end())
			{
				_cats.push_back(cat);
			}
		}
	}

	_cbxCategory->setOptions(_cats, true);
	_cbxCategory->onChange((ActionHandler)&PurchaseState::cbxCategoryChange);

	updateList();

	_timerInc = new Timer(250);
	_timerInc->onTimer((StateHandler)&PurchaseState::increase);
	_timerDec = new Timer(250);
	_timerDec->onTimer((StateHandler)&PurchaseState::decrease);
}

/**
 *
 */
PurchaseState::~PurchaseState()
{
	delete _timerInc;
	delete _timerDec;
}

/**
* Runs the arrow timers.
*/
void PurchaseState::think()
{
	State::think();

	_timerInc->think(this, 0);
	_timerDec->think(this, 0);
}

/**
 * Determines the category a row item belongs in.
 * @param sel Selected row.
 * @returns Item category.
 */
std::string PurchaseState::getCategory(int sel) const
{
	RuleItem *rule = 0;
	switch (_items[sel].type)
	{
	case TRANSFER_SOLDIER:
	case TRANSFER_SCIENTIST:
	case TRANSFER_ENGINEER:
		return "STR_PERSONNEL";
	case TRANSFER_CRAFT:
		return "STR_CRAFT_ARMAMENT";
	case TRANSFER_ITEM:
		rule = (RuleItem*)_items[sel].rule;
		if (rule->getBattleType() == BT_CORPSE || rule->isAlien())
		{
			return "STR_ALIENS";
		}
		if (rule->getBattleType() == BT_NONE)
		{
			if (_craftWeapons.find(rule->getType()) != _craftWeapons.end())
			{
				return "STR_CRAFT_ARMAMENT";
			}
			if (_armors.find(rule->getType()) != _armors.end())
			{
				return "STR_EQUIPMENT";
			}
			return "STR_COMPONENTS";
		}
		return "STR_EQUIPMENT";
	}
	return "STR_ALL_ITEMS";
}

/**
 * Filters the current list of items.
 */
void PurchaseState::updateList()
{
	_lstItems->clearList();
	_rows.clear();
	for (size_t i = 0; i < _items.size(); ++i)
	{
		std::string cat = _cats[_cbxCategory->getSelected()];
		if (cat != "STR_ALL_ITEMS" && cat != getCategory(i))
		{
			continue;
		}
		std::string name = _items[i].name;
		bool ammo = false;
		if (_items[i].type == TRANSFER_ITEM)
		{
			RuleItem *rule = (RuleItem*)_items[i].rule;
			ammo = (rule->getBattleType() == BT_AMMO || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0));
			if (ammo)
			{
				name.insert(0, "  ");
			}
		}
		std::ostringstream ssQty, ssAmount;
		ssQty << _items[i].qtySrc;
		ssAmount << _items[i].amount;
		_lstItems->addRow(4, name.c_str(), Unicode::formatFunding(_items[i].cost).c_str(), ssQty.str().c_str(), ssAmount.str().c_str());
		_rows.push_back(i);
		if (_items[i].amount > 0)
		{
			_lstItems->setRowColor(_rows.size() - 1, _lstItems->getSecondaryColor());
		}
		else if (ammo)
		{
			_lstItems->setRowColor(_rows.size() - 1, _ammoColor);
		}
	}
}

/**
 * Purchases the selected items.
 * @param action Pointer to an action.
 */
void PurchaseState::btnOkClick(Action *)
{
	_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() - _total);
	for (std::vector<TransferRow>::const_iterator i = _items.begin(); i != _items.end(); ++i)
	{
		if (i->amount > 0)
		{
			Transfer *t = 0;
			switch (i->type)
			{
			case TRANSFER_SOLDIER:
				for (int s = 0; s < i->amount; s++)
				{
					RuleSoldier *rule = (RuleSoldier*)i->rule;
					int time = rule->getTransferTime();
					if (time == 0)
						time = _game->getMod()->getPersonnelTime();
					t = new Transfer(time);
					t->setSoldier(_game->getMod()->genSoldier(_game->getSavedGame(), rule->getType()));
					_base->getTransfers()->push_back(t);
				}
				break;
			case TRANSFER_SCIENTIST:
				t = new Transfer(_game->getMod()->getPersonnelTime());
				t->setScientists(i->amount);
				_base->getTransfers()->push_back(t);
				break;
			case TRANSFER_ENGINEER:
				t = new Transfer(_game->getMod()->getPersonnelTime());
				t->setEngineers(i->amount);
				_base->getTransfers()->push_back(t);
				break;
			case TRANSFER_CRAFT:
				for (int c = 0; c < i->amount; c++)
				{
					RuleCraft *rule = (RuleCraft*)i->rule;
					t = new Transfer(rule->getTransferTime());
					Craft *craft = new Craft(rule, _base, _game->getSavedGame()->getId(rule->getType()));
					craft->setStatus("STR_REFUELLING");
					t->setCraft(craft);
					_base->getTransfers()->push_back(t);
				}
				break;
			case TRANSFER_ITEM:
				{
					RuleItem *rule = (RuleItem*)i->rule;
					t = new Transfer(rule->getTransferTime());
					t->setItems(rule->getType(), i->amount);
					_base->getTransfers()->push_back(t);
				}
				break;
			}
		}
	}
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void PurchaseState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Starts increasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsLeftArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && !_timerInc->isRunning()) _timerInc->start();
}

/**
 * Stops increasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsLeftArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerInc->stop();
	}
}

/**
 * Increases the item by one on left-click,
 * to max on right-click.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsLeftArrowClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) increaseByValue(INT_MAX);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		increaseByValue(1);
		_timerInc->setInterval(250);
		_timerDec->setInterval(250);
	}
}

/**
 * Starts decreasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && !_timerDec->isRunning()) _timerDec->start();
}

/**
 * Stops decreasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerDec->stop();
	}
}

/**
 * Decreases the item by one on left-click,
 * to 0 on right-click.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) decreaseByValue(INT_MAX);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		decreaseByValue(1);
		_timerInc->setInterval(250);
		_timerDec->setInterval(250);
	}
}

/**
 * Handles the mouse-wheels on the arrow-buttons.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsMousePress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->wheel.y > 0)
	{
		_timerInc->stop();
		_timerDec->stop();
		if (action->getAbsoluteXMouse() >= _lstItems->getArrowsLeftEdge() &&
			action->getAbsoluteXMouse() <= _lstItems->getArrowsRightEdge())
		{
			increaseByValue(Options::changeValueByMouseWheel);
		}
	}
	else if (action->getDetails()->wheel.y < 0)
	{
		_timerInc->stop();
		_timerDec->stop();
		if (action->getAbsoluteXMouse() >= _lstItems->getArrowsLeftEdge() &&
			action->getAbsoluteXMouse() <= _lstItems->getArrowsRightEdge())
		{
			decreaseByValue(Options::changeValueByMouseWheel);
		}
	}
}

/**
 * Increases the quantity of the selected item to buy by one.
 */
void PurchaseState::increase()
{
	_timerDec->setInterval(50);
	_timerInc->setInterval(50);
	increaseByValue(1);
}

/**
 * Increases the quantity of the selected item to buy by "change".
 * @param change How much we want to add.
 */
void PurchaseState::increaseByValue(int change)
{
	if (0 >= change) return;
	std::string errorMessage;

	if (_total + getRow().cost > _game->getSavedGame()->getFunds())
	{
		errorMessage = tr("STR_NOT_ENOUGH_MONEY");
	}
	else
	{
		RuleItem *rule = 0;
		switch (getRow().type)
		{
		case TRANSFER_SOLDIER:
		case TRANSFER_SCIENTIST:
		case TRANSFER_ENGINEER:
			if (_pQty + 1 > _base->getAvailableQuarters() - _base->getUsedQuarters())
			{
				errorMessage = tr("STR_NOT_ENOUGH_LIVING_SPACE");
			}
			break;
		case TRANSFER_CRAFT:
			if (_cQty + 1 > _base->getAvailableHangars() - _base->getUsedHangars())
			{
				errorMessage = tr("STR_NO_FREE_HANGARS_FOR_PURCHASE");
			}
			break;
		case TRANSFER_ITEM:
			rule = (RuleItem*)getRow().rule;
			if (_base->storesOverfull(_iQty + rule->getSize()))
			{
				errorMessage = tr("STR_NOT_ENOUGH_STORE_SPACE");
			}
			break;
		}
	}

	if (errorMessage.empty())
	{
		int maxByMoney = (_game->getSavedGame()->getFunds() - _total) / getRow().cost;
		if (maxByMoney >= 0)
			change = std::min(maxByMoney, change);
		switch (getRow().type)
		{
		case TRANSFER_SOLDIER:
		case TRANSFER_SCIENTIST:
		case TRANSFER_ENGINEER:
			{
				int maxByQuarters = _base->getAvailableQuarters() - _base->getUsedQuarters() - _pQty;
				change = std::min(maxByQuarters, change);
				_pQty += change;
			}
			break;
		case TRANSFER_CRAFT:
			{
				int maxByHangars = _base->getAvailableHangars() - _base->getUsedHangars() - _cQty;
				change = std::min(maxByHangars, change);
				_cQty += change;
			}
			break;
		case TRANSFER_ITEM:
		{
			RuleItem *rule = (RuleItem*)getRow().rule;
			double storesNeededPerItem = rule->getSize();
			double freeStores = _base->getAvailableStores() - _base->getUsedStores() - _iQty;
			double maxByStores = (double)(INT_MAX);
			if (!AreSame(storesNeededPerItem, 0.0))
			{
				maxByStores = (freeStores + 0.05) / storesNeededPerItem;
			}
			change = std::min((int)maxByStores, change);
			_iQty += change * storesNeededPerItem;
		}
			break;
		}
		getRow().amount += change;
		_total += getRow().cost * change;
		updateItemStrings();
	}
	else
	{
		_timerInc->stop();
		RuleInterface *menuInterface = _game->getMod()->getInterface("buyMenu");
		_game->pushState(new ErrorMessageState(errorMessage, _palette, menuInterface->getElement("errorMessage")->color, "BACK13.SCR", menuInterface->getElement("errorPalette")->color));
	}
}

/**
 * Decreases the quantity of the selected item to buy by one.
 */
void PurchaseState::decrease()
{
	_timerInc->setInterval(50);
	_timerDec->setInterval(50);
	decreaseByValue(1);
}

/**
 * Decreases the quantity of the selected item to buy by "change".
 * @param change how much we want to add.
 */
void PurchaseState::decreaseByValue(int change)
{
	if (0 >= change || 0 >= getRow().amount) return;
	change = std::min(getRow().amount, change);

	RuleItem *rule = 0;
	switch (getRow().type)
	{
	case TRANSFER_SOLDIER:
	case TRANSFER_SCIENTIST:
	case TRANSFER_ENGINEER:
		_pQty -= change;
		break;
	case TRANSFER_CRAFT:
		_cQty -= change;
		break;
	case TRANSFER_ITEM:
		rule = (RuleItem*)getRow().rule;
		_iQty -= rule->getSize() * change;
		break;
	}
	getRow().amount -= change;
	_total -= getRow().cost * change;
	updateItemStrings();
}

/**
 * Updates the quantity-strings of the selected item.
 */
void PurchaseState::updateItemStrings()
{
	_txtPurchases->setText(tr("STR_COST_OF_PURCHASES").arg(Unicode::formatFunding(_total)));
	std::ostringstream ss, ss5;
	ss << getRow().amount;
	_lstItems->setCellText(_sel, 3, ss.str());
	if (getRow().amount > 0)
	{
		_lstItems->setRowColor(_sel, _lstItems->getSecondaryColor());
	}
	else
	{
		_lstItems->setRowColor(_sel, _lstItems->getColor());
		if (getRow().type == TRANSFER_ITEM)
		{
			RuleItem *rule = (RuleItem*)getRow().rule;
			if (rule->getBattleType() == BT_AMMO || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0))
			{
				_lstItems->setRowColor(_sel, _ammoColor);
			}
		}
	}
	ss5 << _base->getUsedStores();
	if (std::abs(_iQty) > 0.05)
	{
		ss5 << "(";
		if (_iQty > 0.05)
			ss5 << "+";
		ss5 << std::fixed << std::setprecision(1) << _iQty << ")";
	}
	ss5 << ":" << _base->getAvailableStores();
	_txtSpaceUsed->setText(tr("STR_SPACE_USED").arg(ss5.str()));
}

/**
 * Updates the production list to match the category filter.
 */
void PurchaseState::cbxCategoryChange(Action *)
{
	updateList();
}

}
