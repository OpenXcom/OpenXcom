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
#include "PurchaseState.h"
#include <sstream>
#include <climits>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include "../fmath.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
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
#include "../Savegame/Soldier.h"

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
	_txtItem = new Text(140, 9, 10, Options::storageLimitsEnforced? 44:33);
	_txtCost = new Text(102, 9, 152, Options::storageLimitsEnforced? 44:33);
	_txtQuantity = new Text(60, 9, 256, Options::storageLimitsEnforced? 44:33);
	_lstItems = new TextList(287, Options::storageLimitsEnforced? 112:120, 8, Options::storageLimitsEnforced? 55:44);

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
	add(_txtItem, "text", "buyMenu");
	add(_txtCost, "text", "buyMenu");
	add(_txtQuantity, "text", "buyMenu");
	add(_lstItems, "list", "buyMenu");

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

	_txtFunds->setText(tr("STR_CURRENT_FUNDS").arg(Text::formatFunding(_game->getSavedGame()->getFunds())));

	_txtPurchases->setText(tr("STR_COST_OF_PURCHASES").arg(Text::formatFunding(_total)));

	_txtSpaceUsed->setVisible(Options::storageLimitsEnforced);
	std::wostringstream ss1;
	ss1 << _base->getUsedStores() << ":" << _base->getAvailableStores();
	_txtSpaceUsed->setText(tr("STR_SPACE_USED").arg(ss1.str()));

	_txtItem->setText(tr("STR_ITEM"));

	_txtCost->setText(tr("STR_COST_PER_UNIT_UC"));

	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_lstItems->setArrowColumn(227, ARROW_VERTICAL);
	_lstItems->setColumns(4, 150, 55, 50, 28);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);
	_lstItems->onLeftArrowPress((ActionHandler)&PurchaseState::lstItemsLeftArrowPress);
	_lstItems->onLeftArrowRelease((ActionHandler)&PurchaseState::lstItemsLeftArrowRelease);
	_lstItems->onLeftArrowClick((ActionHandler)&PurchaseState::lstItemsLeftArrowClick);
	_lstItems->onRightArrowPress((ActionHandler)&PurchaseState::lstItemsRightArrowPress);
	_lstItems->onRightArrowRelease((ActionHandler)&PurchaseState::lstItemsRightArrowRelease);
	_lstItems->onRightArrowClick((ActionHandler)&PurchaseState::lstItemsRightArrowClick);
	_lstItems->onMousePress((ActionHandler)&PurchaseState::lstItemsMousePress);

	const std::vector<std::string> &soldiers = _game->getMod()->getSoldiersList();
	for (std::vector<std::string>::const_iterator i = soldiers.begin(); i != soldiers.end(); ++i)
	{
		RuleSoldier *rule = _game->getMod()->getSoldier(*i);
		{
			_soldiers.push_back(*i);
			_qtys.push_back(0);
			std::wostringstream ss;
			ss << _base->getSoldierCount(*i);
			_lstItems->addRow(4, tr(*i).c_str(), Text::formatFunding(rule->getBuyCost()).c_str(), ss.str().c_str(), L"0");
		}
	}

	_qtys.push_back(0);
	std::wostringstream ss2;
	ss2 << _base->getTotalScientists();
	_lstItems->addRow(4, tr("STR_SCIENTIST").c_str(), Text::formatFunding(_game->getMod()->getScientistCost() * 2).c_str(), ss2.str().c_str(), L"0");
	_qtys.push_back(0);
	std::wostringstream ss3;
	ss3 << _base->getTotalEngineers();
	_lstItems->addRow(4, tr("STR_ENGINEER").c_str(), Text::formatFunding(_game->getMod()->getEngineerCost() * 2).c_str(), ss3.str().c_str(), L"0");

	const std::vector<std::string> &crafts = _game->getMod()->getCraftsList();
	for (std::vector<std::string>::const_iterator i = crafts.begin(); i != crafts.end(); ++i)
	{
		RuleCraft *rule = _game->getMod()->getCraft(*i);
		if (rule->getBuyCost() != 0 && _game->getSavedGame()->isResearched(rule->getRequirements()))
		{
			_crafts.push_back(*i);
			_qtys.push_back(0);
			std::wostringstream ss;
			ss << _base->getCraftCount(*i);
			_lstItems->addRow(4, tr(*i).c_str(), Text::formatFunding(rule->getBuyCost()).c_str(), ss.str().c_str(), L"0");
		}
	}
	const std::vector<std::string> &items = _game->getMod()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		RuleItem *rule = _game->getMod()->getItem(*i);
		if (rule->getBuyCost() != 0 && _game->getSavedGame()->isResearched(rule->getRequirements()) && !isExcluded(*i))
		{
			_items.push_back(*i);
			_qtys.push_back(0);
			std::wostringstream ss;
			ss << _base->getItems()->getItem(*i);
			std::wstring item = tr(*i);
			if (rule->getBattleType() == BT_AMMO || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0))
			{
				item.insert(0, L"  ");
				_lstItems->addRow(4, item.c_str(), Text::formatFunding(rule->getBuyCost()).c_str(), ss.str().c_str(), L"0");
				_lstItems->setRowColor(_qtys.size() - 1, _ammoColor);
			}
			else
			{
				_lstItems->addRow(4, item.c_str(), Text::formatFunding(rule->getBuyCost()).c_str(), ss.str().c_str(), L"0");
			}
		}
	}

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
 * Returns whether the item is excluded in the options file.
 * @param item Item to look up.
 * @return True if the item is excluded in the options file.
 */
bool PurchaseState::isExcluded(const std::string &item)
{
	for (std::vector<std::string>::const_iterator s = Options::purchaseExclusions.begin(); s != Options::purchaseExclusions.end(); ++s)
	{
		if (item == *s)
		{
			return true;
		}
	}
	return false;
}

/**
 * Purchases the selected items.
 * @param action Pointer to an action.
 */
void PurchaseState::btnOkClick(Action *)
{
	_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() - _total);
	for (size_t i = 0; i < _qtys.size(); ++i)
	{
		if (_qtys[i] > 0)
		{
			Transfer *t = 0;
			switch (getType(i))
			{
			case TRANSFER_SOLDIER:
				for (int s = 0; s < _qtys[i]; s++)
				{
					t = new Transfer(_game->getMod()->getPersonnelTime());
					t->setSoldier(_game->getMod()->genSoldier(_game->getSavedGame(), _soldiers[i]));
					_base->getTransfers()->push_back(t);
				}
				break;
			case TRANSFER_SCIENTIST:
				t = new Transfer(_game->getMod()->getPersonnelTime());
				t->setScientists(_qtys[i]);
				_base->getTransfers()->push_back(t);
				break;
			case TRANSFER_ENGINEER:
				t = new Transfer(_game->getMod()->getPersonnelTime());
				t->setEngineers(_qtys[i]);
				_base->getTransfers()->push_back(t);
				break;
			case TRANSFER_CRAFT:
				for (int c = 0; c < _qtys[i]; c++)
				{
					RuleCraft *rc = _game->getMod()->getCraft(_crafts[getCraftIndex(i)]);
					t = new Transfer(rc->getTransferTime());
					Craft *craft = new Craft(rc, _base, _game->getSavedGame()->getId(_crafts[getCraftIndex(i)]));
					craft->setStatus("STR_REFUELLING");
					t->setCraft(craft);
					_base->getTransfers()->push_back(t);
				}
				break;
			case TRANSFER_ITEM:
				RuleItem *ri = _game->getMod()->getItem(_items[getItemIndex(i)]);
				t = new Transfer(ri->getTransferTime());
				t->setItems(_items[getItemIndex(i)], _qtys[i]);
				_base->getTransfers()->push_back(t);
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
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP)
	{
		_timerInc->stop();
		_timerDec->stop();
		if (action->getAbsoluteXMouse() >= _lstItems->getArrowsLeftEdge() &&
			action->getAbsoluteXMouse() <= _lstItems->getArrowsRightEdge())
		{
			increaseByValue(Options::changeValueByMouseWheel);
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN)
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
 * Gets the price of the currently selected item.
 * @return Price of the currently selected item.
 */
int PurchaseState::getPrice()
{
	switch (getType(_sel))
	{
	case TRANSFER_SOLDIER:
		return _game->getMod()->getSoldier(_soldiers[_sel])->getBuyCost();
	case TRANSFER_ENGINEER:
		return _game->getMod()->getScientistCost() * 2;
	case TRANSFER_SCIENTIST:
		return _game->getMod()->getEngineerCost() * 2;
	case TRANSFER_ITEM:
		return _game->getMod()->getItem(_items[getItemIndex(_sel)])->getBuyCost();
	case TRANSFER_CRAFT:
		return _game->getMod()->getCraft(_crafts[getCraftIndex(_sel)])->getBuyCost();
	}
	return 0;
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
	std::wstring errorMessage;

	if (_total + getPrice() > _game->getSavedGame()->getFunds())
	{
		errorMessage = tr("STR_NOT_ENOUGH_MONEY");
	}
	else
	{
		switch (getType(_sel))
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
			if (_iQty + _game->getMod()->getItem(_items[getItemIndex(_sel)])->getSize() > _base->getAvailableStores() - _base->getUsedStores())
			{
				errorMessage = tr("STR_NOT_ENOUGH_STORE_SPACE");
			}
			break;
		}
	}

	if (errorMessage.empty())
	{
		int maxByMoney = (_game->getSavedGame()->getFunds() - _total) / getPrice();
		change = std::min(maxByMoney, change);
		switch (getType(_sel))
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
				double storesNeededPerItem = _game->getMod()->getItem(_items[getItemIndex(_sel)])->getSize();
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
		_qtys[_sel] += change;
		_total += getPrice() * change;
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
	if (0 >= change || 0 >= _qtys[_sel]) return;
	change = std::min(_qtys[_sel], change);

	switch (getType(_sel))
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
		_iQty -= _game->getMod()->getItem(_items[getItemIndex(_sel)])->getSize() * change;
		break;
	}
	_qtys[_sel] -= change;
	_total -= getPrice() * change;
	updateItemStrings();
}

/**
 * Updates the quantity-strings of the selected item.
 */
void PurchaseState::updateItemStrings()
{
	_txtPurchases->setText(tr("STR_COST_OF_PURCHASES").arg(Text::formatFunding(_total)));
	std::wostringstream ss, ss5;
	ss << _qtys[_sel];
	_lstItems->setCellText(_sel, 3, ss.str());
	if (_qtys[_sel] > 0)
	{
		_lstItems->setRowColor(_sel, _lstItems->getSecondaryColor());
	}
	else
	{
		_lstItems->setRowColor(_sel, _lstItems->getColor());
		if(getType(_sel) == TRANSFER_ITEM)
		{
			RuleItem *rule = _game->getMod()->getItem(_items[getItemIndex(_sel)]);
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
 * Gets the Type of the selected item.
 * @param selected Currently selected item.
 * @return The type of the selected item.
 */
TransferType PurchaseState::getType(size_t selected) const
{
	size_t max = _soldiers.size();

	if (selected < max)
		return TRANSFER_SOLDIER;
	if (selected < (max += 1))
		return TRANSFER_SCIENTIST;
	if (selected < (max += 1))
		return TRANSFER_ENGINEER;
	if (selected < (max += _crafts.size()))
		return TRANSFER_CRAFT;

	return TRANSFER_ITEM;
}

/**
 * Gets the index of the selected item.
 * @param selected Currently selected item.
 * @return Index of the selected item.
 */
size_t PurchaseState::getItemIndex(size_t selected) const
{
	return selected - _soldiers.size() - _crafts.size() - 2;
}

/**
 * Gets the index of selected craft.
 * @param selected Selected craft.
 * @return Index of the selected craft.
 */
size_t PurchaseState::getCraftIndex(size_t selected) const
{
	return selected - _soldiers.size() - 2;
}

}
