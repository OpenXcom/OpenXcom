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
#include "SellState.h"
#include <sstream>
#include <climits>
#include <cmath>
#include <iomanip>
#include "../Engine/Action.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/BaseFacility.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Savegame/ItemContainer.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Savegame/Transfer.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Sell/Sack screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param origin Game section that originated this state.
 */
SellState::SellState(Base *base, OptionsOrigin origin) : _base(base), _sel(0), _itemOffset(0), _total(0), _hasSci(0), _hasEng(0), _spaceChange(0), _origin(origin)
{
	bool overfull = Options::storageLimitsEnforced && _base->storesOverfull();

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(overfull? 288:148, 16, overfull? 16:8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtSales = new Text(150, 9, 10, 24);
	_txtFunds = new Text(150, 9, 160, 24);
	_txtSpaceUsed = new Text(150, 9, 160, 34);
	_txtItem = new Text(130, 9, 10, Options::storageLimitsEnforced? 44:33);
	_txtQuantity = new Text(54, 9, 126, Options::storageLimitsEnforced? 44:33);
	_txtSell = new Text(96, 9, 180, Options::storageLimitsEnforced? 44:33);
	_txtValue = new Text(40, 9, 260, Options::storageLimitsEnforced? 44:33);
	_lstItems = new TextList(287, Options::storageLimitsEnforced? 112:120, 8, Options::storageLimitsEnforced? 55:44);

	// Set palette
	setInterface("sellMenu");

	_ammoColor = _game->getRuleset()->getInterface("sellMenu")->getElement("ammoColor")->color;

	add(_window, "window", "sellMenu");
	add(_btnOk, "button", "sellMenu");
	add(_btnCancel, "button", "sellMenu");
	add(_txtTitle, "text", "sellMenu");
	add(_txtSales, "text", "sellMenu");
	add(_txtFunds, "text", "sellMenu");
	add(_txtSpaceUsed, "text", "sellMenu");
	add(_txtItem, "text", "sellMenu");
	add(_txtQuantity, "text", "sellMenu");
	add(_txtSell, "text", "sellMenu");
	add(_txtValue, "text", "sellMenu");
	add(_lstItems, "list", "sellMenu");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setText(tr("STR_SELL_SACK"));
	_btnOk->onMouseClick((ActionHandler)&SellState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SellState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&SellState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&SellState::btnCancelClick, Options::keyCancel);

	if (overfull)
	{
		_btnCancel->setVisible(false);
		_btnOk->setVisible(false);
	}

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_SELL_ITEMS_SACK_PERSONNEL"));

	_txtSales->setText(tr("STR_VALUE_OF_SALES").arg(Text::formatFunding(_total)));

	_txtFunds->setText(tr("STR_FUNDS").arg(Text::formatFunding(_game->getSavedGame()->getFunds())));

	_txtSpaceUsed->setVisible(Options::storageLimitsEnforced);

	std::wostringstream ss5;
	ss5 << _base->getUsedStores() << ":" << _base->getAvailableStores();
	_txtSpaceUsed->setText(ss5.str());
	_txtSpaceUsed->setText(tr("STR_SPACE_USED").arg(ss5.str()));

	_txtItem->setText(tr("STR_ITEM"));

	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_txtSell->setText(tr("STR_SELL_SACK"));

	_txtValue->setText(tr("STR_VALUE"));

	_lstItems->setArrowColumn(182, ARROW_VERTICAL);
	_lstItems->setColumns(4, 156, 54, 24, 53);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);
	_lstItems->onLeftArrowPress((ActionHandler)&SellState::lstItemsLeftArrowPress);
	_lstItems->onLeftArrowRelease((ActionHandler)&SellState::lstItemsLeftArrowRelease);
	_lstItems->onLeftArrowClick((ActionHandler)&SellState::lstItemsLeftArrowClick);
	_lstItems->onRightArrowPress((ActionHandler)&SellState::lstItemsRightArrowPress);
	_lstItems->onRightArrowRelease((ActionHandler)&SellState::lstItemsRightArrowRelease);
	_lstItems->onRightArrowClick((ActionHandler)&SellState::lstItemsRightArrowClick);
	_lstItems->onMousePress((ActionHandler)&SellState::lstItemsMousePress);

	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
	{
		if ((*i)->getCraft() == 0)
		{
			_qtys.push_back(0);
			_soldiers.push_back(*i);
			_lstItems->addRow(4, (*i)->getName(true).c_str(), L"1", L"0", Text::formatFunding(0).c_str());
			++_itemOffset;
		}
	}
	for (std::vector<Craft*>::iterator i = _base->getCrafts()->begin(); i != _base->getCrafts()->end(); ++i)
	{
		if ((*i)->getStatus() != "STR_OUT")
		{
			_qtys.push_back(0);
			_crafts.push_back(*i);
			_lstItems->addRow(4, (*i)->getName(_game->getLanguage()).c_str(), L"1", L"0", Text::formatFunding((*i)->getRules()->getSellCost()).c_str());
			++_itemOffset;
		}
	}
	if (_base->getAvailableScientists() > 0)
	{
		_qtys.push_back(0);
		_hasSci = 1;
		std::wostringstream ss;
		ss << _base->getAvailableScientists();
		_lstItems->addRow(4, tr("STR_SCIENTIST").c_str(), ss.str().c_str(), L"0", Text::formatFunding(0).c_str());
		++_itemOffset;
	}
	if (_base->getAvailableEngineers() > 0)
	{
		_qtys.push_back(0);
		_hasEng = 1;
		std::wostringstream ss;
		ss << _base->getAvailableEngineers();
		_lstItems->addRow(4, tr("STR_ENGINEER").c_str(), ss.str().c_str(), L"0", Text::formatFunding(0).c_str());
		++_itemOffset;
	}
	const std::vector<std::string> &items = _game->getRuleset()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		int qty = _base->getItems()->getItem(*i);
		if (Options::storageLimitsEnforced && origin == OPT_BATTLESCAPE)
		{
			for (std::vector<Transfer*>::iterator j = _base->getTransfers()->begin(); j != _base->getTransfers()->end(); ++j)
			{
				if ((*j)->getItems() == *i)
				{
					qty += (*j)->getQuantity();
				}
			}
			for (std::vector<Craft*>::iterator j = _base->getCrafts()->begin(); j != _base->getCrafts()->end(); ++j)
			{
				qty += (*j)->getItems()->getItem(*i);
			}
		}
		if (qty > 0 && (Options::canSellLiveAliens || !_game->getRuleset()->getItem(*i)->isAlien()))
		{
			_qtys.push_back(0);
			_items.push_back(*i);
			RuleItem *rule = _game->getRuleset()->getItem(*i);
			std::wostringstream ss;
			ss << qty;
			std::wstring item = tr(*i);
			if (rule->getBattleType() == BT_AMMO || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0))
			{
				item.insert(0, L"  ");
				_lstItems->addRow(4, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str());
				_lstItems->setRowColor(_qtys.size() - 1, _ammoColor);
			}
			else
			{
				_lstItems->addRow(4, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str());
			}
		}
	}

	_timerInc = new Timer(250);
	_timerInc->onTimer((StateHandler)&SellState::increase);
	_timerDec = new Timer(250);
	_timerDec->onTimer((StateHandler)&SellState::decrease);
}

/**
 *
 */
SellState::~SellState()
{
	delete _timerInc;
	delete _timerDec;
}

/**
 * Runs the arrow timers.
 */
void SellState::think()
{
	State::think();

	_timerInc->think(this, 0);
	_timerDec->think(this, 0);
}

/**
 * Gets the index of selected craft.
 * @param selected Selected craft.
 * @return Index of the selected craft.
 */
size_t SellState::getCraftIndex(size_t selected) const
{
	return selected - _soldiers.size();
}

/**
 * Sells the selected items.
 * @param action Pointer to an action.
 */
void SellState::btnOkClick(Action *)
{
	_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() + _total);
	for (size_t i = 0; i < _qtys.size(); ++i)
	{
		if (_qtys[i] > 0)
		{
			switch (getType(i))
			{
			case SELL_SOLDIER:
				for (std::vector<Soldier*>::iterator s = _base->getSoldiers()->begin(); s != _base->getSoldiers()->end(); ++s)
				{
					if (*s == _soldiers[i])
					{
						if ((*s)->getArmor()->getStoreItem() != "STR_NONE")
						{
							_base->getItems()->addItem((*s)->getArmor()->getStoreItem());
						}
						_base->getSoldiers()->erase(s);
						break;
					}
				}
				delete _soldiers[i];
				break;

			case SELL_CRAFT:
				{
				Craft *craft = _crafts[getCraftIndex(i)];

				// Remove weapons from craft
				for (std::vector<CraftWeapon*>::iterator w = craft->getWeapons()->begin(); w != craft->getWeapons()->end(); ++w)
				{
					if ((*w) != 0)
					{
						_base->getItems()->addItem((*w)->getRules()->getLauncherItem());
						_base->getItems()->addItem((*w)->getRules()->getClipItem(), (*w)->getClipsLoaded(_game->getRuleset()));
					}
				}

				// Remove items from craft
				for (std::map<std::string, int>::iterator it = craft->getItems()->getContents()->begin(); it != craft->getItems()->getContents()->end(); ++it)
				{
					_base->getItems()->addItem(it->first, it->second);
				}

				// Remove soldiers from craft
				for (std::vector<Soldier*>::iterator s = _base->getSoldiers()->begin(); s != _base->getSoldiers()->end(); ++s)
				{
					if ((*s)->getCraft() == craft)
					{
						(*s)->setCraft(0);
					}
				}

				// Clear Hangar
				for (std::vector<BaseFacility*>::iterator f = _base->getFacilities()->begin(); f != _base->getFacilities()->end(); ++f)
				{
					if ((*f)->getCraft() == craft)
					{
						(*f)->setCraft(0);
						break;
					}
				}

				// Remove craft
				for (std::vector<Craft*>::iterator c = _base->getCrafts()->begin(); c != _base->getCrafts()->end(); ++c)
				{
					if (*c == craft)
					{
						_base->getCrafts()->erase(c);
						break;
					}
				}
				delete craft;
				}
				break;

			case SELL_SCIENTIST:
				_base->setScientists(_base->getScientists() - _qtys[i]);
				break;

			case SELL_ENGINEER:
				_base->setEngineers(_base->getEngineers() - _qtys[i]);
				break;

			case SELL_ITEM:
				if (_base->getItems()->getItem(_items[getItemIndex(i)]) < _qtys[i])
				{
					const std::string itemName = _items[getItemIndex(i)];
					int toRemove = _qtys[i] - _base->getItems()->getItem(itemName);

					// remove all of said items from base
					_base->getItems()->removeItem(itemName, INT_MAX);

					// if we still need to remove any, remove them from the crafts first, and keep a running tally
					for (std::vector<Craft*>::iterator j = _base->getCrafts()->begin(); j != _base->getCrafts()->end() && toRemove; ++j)
					{
						if ((*j)->getItems()->getItem(itemName) < toRemove)
						{
							toRemove -= (*j)->getItems()->getItem(itemName);
							(*j)->getItems()->removeItem(itemName, INT_MAX);
						}
						else
						{
							(*j)->getItems()->removeItem(itemName, toRemove);
							toRemove = 0;
						}
					}

					// if there are STILL any left to remove, take them from the transfers, and if necessary, delete it.
					for (std::vector<Transfer*>::iterator j = _base->getTransfers()->begin(); j != _base->getTransfers()->end() && toRemove;)
					{
						if ((*j)->getItems() == itemName)
						{
							if ((*j)->getQuantity() <= toRemove)
							{
								toRemove -= (*j)->getQuantity();
								delete *j;
								j = _base->getTransfers()->erase(j);
							}
							else
							{
								(*j)->setItems((*j)->getItems(), (*j)->getQuantity() - toRemove);
								toRemove = 0;
							}
						}
						else
						{
							++j;
						}
					}
				}
				else
				{
					_base->getItems()->removeItem(_items[getItemIndex(i)], _qtys[i]);
				}
			}
		}
	}
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SellState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Starts increasing the item.
 * @param action Pointer to an action.
 */
void SellState::lstItemsLeftArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && !_timerInc->isRunning()) _timerInc->start();
}

/**
 * Stops increasing the item.
 * @param action Pointer to an action.
 */
void SellState::lstItemsLeftArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerInc->stop();
	}
}

/**
 * Increases the selected item;
 * by one on left-click, to max on right-click.
 * @param action Pointer to an action.
 */
void SellState::lstItemsLeftArrowClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) changeByValue(INT_MAX, 1);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		changeByValue(1,1);
		_timerInc->setInterval(250);
		_timerDec->setInterval(250);
	}
}

/**
 * Starts decreasing the item.
 * @param action Pointer to an action.
 */
void SellState::lstItemsRightArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && !_timerDec->isRunning()) _timerDec->start();
}

/**
 * Stops decreasing the item.
 * @param action Pointer to an action.
 */
void SellState::lstItemsRightArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerDec->stop();
	}
}

/**
 * Decreases the selected item;
 * by one on left-click, to 0 on right-click.
 * @param action Pointer to an action.
 */
void SellState::lstItemsRightArrowClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) changeByValue(INT_MAX, -1);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		changeByValue(1,-1);
		_timerInc->setInterval(250);
		_timerDec->setInterval(250);
	}
}

/**
 * Handles the mouse-wheels on the arrow-buttons.
 * @param action Pointer to an action.
 */
void SellState::lstItemsMousePress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP)
	{
		_timerInc->stop();
		_timerDec->stop();
		if (action->getAbsoluteXMouse() >= _lstItems->getArrowsLeftEdge() &&
			action->getAbsoluteXMouse() <= _lstItems->getArrowsRightEdge())
		{
			changeByValue(Options::changeValueByMouseWheel, 1);
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN)
	{
		_timerInc->stop();
		_timerDec->stop();
		if (action->getAbsoluteXMouse() >= _lstItems->getArrowsLeftEdge() &&
			action->getAbsoluteXMouse() <= _lstItems->getArrowsRightEdge())
		{
			changeByValue(Options::changeValueByMouseWheel, -1);
		}
	}
}

/**
 * Gets the price of the currently selected item.
 * @return Price of the selected item.
 */
int SellState::getPrice()
{
	// Personnel/craft aren't worth anything
	switch(getType(_sel))
	{
	case SELL_SOLDIER:
	case SELL_ENGINEER:
	case SELL_SCIENTIST:
		return 0;
	case SELL_ITEM:
		return _game->getRuleset()->getItem(_items[getItemIndex(_sel)])->getSellCost();
	case SELL_CRAFT:
		Craft *craft =  _crafts[getCraftIndex(_sel)];
		return craft->getRules()->getSellCost();
	}
	return 0;
}

/**
 * Gets the quantity of the currently selected item
 * on the base.
 * @return Quantity of selected item on the base.
 */
int SellState::getQuantity()
{
	int qty = 0;
	// Soldiers/crafts are individual
	switch(getType(_sel))
	{
	case SELL_SOLDIER:
	case SELL_CRAFT:
		return 1;
	case SELL_SCIENTIST:
		return _base->getAvailableScientists();
	case SELL_ENGINEER:
		return _base->getAvailableEngineers();
	case SELL_ITEM:
		qty = _base->getItems()->getItem(_items[getItemIndex(_sel)]);
		if (Options::storageLimitsEnforced && _origin == OPT_BATTLESCAPE)
		{
			for (std::vector<Transfer*>::iterator j = _base->getTransfers()->begin(); j != _base->getTransfers()->end(); ++j)
			{
				if ((*j)->getItems() == _items[getItemIndex(_sel)])
				{
					qty += (*j)->getQuantity();
				}
			}
			for (std::vector<Craft*>::iterator j = _base->getCrafts()->begin(); j != _base->getCrafts()->end(); ++j)
			{
				qty += (*j)->getItems()->getItem(_items[getItemIndex(_sel)]);
			}
		}
		return qty;
	}

	return 0;
}

/**
 * Increases the quantity of the selected item to sell by one.
 */
void SellState::increase()
{
	_timerDec->setInterval(50);
	_timerInc->setInterval(50);
	changeByValue(1,1);
}

/**
 * Increases or decreases the quantity of the selected item to sell.
 * @param change How much we want to add or remove.
 * @param dir Direction to change, +1 to increase or -1 to decrease.
 */
void SellState::changeByValue(int change, int dir)
{
	if (dir > 0)
	{
		if (0 >= change || getQuantity() <=_qtys[_sel]) return;
		change = std::min(getQuantity() - _qtys[_sel], change);
	}
	else
	{
		if (0 >= change || 0 >= _qtys[_sel]) return;
		change = std::min(_qtys[_sel], change);
	}
	_qtys[_sel] += dir * change;
	_total += dir * getPrice() * change;

	// Calculate the change in storage space.
	Craft *craft;
	RuleItem *armor, *item, *weapon, *ammo;
	double total = 0.0;
	switch (getType(_sel))
	{
	case SELL_SOLDIER:
		if (_soldiers[_sel]->getArmor()->getStoreItem() != "STR_NONE")
		{
			armor = _game->getRuleset()->getItem(_soldiers[_sel]->getArmor()->getStoreItem());
			_spaceChange += dir * armor->getSize();
		}
		break;
	case SELL_CRAFT:
		craft = _crafts[getCraftIndex(_sel)];
		for (std::vector<CraftWeapon*>::iterator w = craft->getWeapons()->begin(); w != craft->getWeapons()->end(); ++w)
		{
			if (*w)
			{
				weapon = _game->getRuleset()->getItem((*w)->getRules()->getLauncherItem());
				total += weapon->getSize();
				ammo = _game->getRuleset()->getItem((*w)->getRules()->getClipItem());
				if (ammo)
					total += ammo->getSize() * (*w)->getClipsLoaded(_game->getRuleset());
			}
		}
		_spaceChange += dir * total;
		break;
	case SELL_ITEM:
		item = _game->getRuleset()->getItem(_items[getItemIndex(_sel)]);
		_spaceChange -= dir * change * item->getSize();
		break;
	default:
		break;
	}

	updateItemStrings();
}

/**
 * Decreases the quantity of the selected item to sell by one.
 */
void SellState::decrease()
{
	_timerInc->setInterval(50);
	_timerDec->setInterval(50);
	changeByValue(1,-1);
}

/**
 * Updates the quantity-strings of the selected item.
 */
void SellState::updateItemStrings()
{
	std::wostringstream ss, ss2, ss5;
	ss << _qtys[_sel];
	_lstItems->setCellText(_sel, 2, ss.str());
	ss2 << getQuantity() - _qtys[_sel];
	_lstItems->setCellText(_sel, 1, ss2.str());
	_txtSales->setText(tr("STR_VALUE_OF_SALES").arg(Text::formatFunding(_total)));

	if (_qtys[_sel] > 0)
	{
		_lstItems->setRowColor(_sel, _lstItems->getSecondaryColor());
	}
	else
	{
		_lstItems->setRowColor(_sel, _lstItems->getColor());
		if (_sel > _itemOffset)
		{
			RuleItem *rule = _game->getRuleset()->getItem(_items[_sel - _itemOffset]);
			if (rule->getBattleType() == BT_AMMO || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0))
			{
				_lstItems->setRowColor(_sel, _ammoColor);
			}
		}
	}

	ss5 << _base->getUsedStores();
	if (std::abs(_spaceChange) > 0.05)
	{
		ss5 << "(";
		if (_spaceChange > 0.05)
			ss5 << "+";
		ss5 << std::fixed << std::setprecision(1) << _spaceChange << ")";
	}
	ss5 << ":" << _base->getAvailableStores();
	_txtSpaceUsed->setText(tr("STR_SPACE_USED").arg(ss5.str()));
	if (Options::storageLimitsEnforced)
	{
		_btnOk->setVisible(!_base->storesOverfull(_spaceChange));
	}
}

/**
 * Gets the Type of the selected item.
 * @param selected Currently selected item.
 * @return The type of the selected item.
 */
enum SellType SellState::getType(size_t selected) const
{
	size_t max = _soldiers.size();

	if (selected < max)
		return SELL_SOLDIER;
	if (selected < (max += _crafts.size()) )
		return SELL_CRAFT;
	if (selected < (max += _hasSci))
		return SELL_SCIENTIST;
	if (selected < (max += _hasEng))
		return SELL_ENGINEER;

	return SELL_ITEM;
}

/**
 * Gets the index of the selected item.
 * @param selected Currently selected item.
 * @return Index of the selected item.
 */
size_t SellState::getItemIndex(size_t selected) const
{
	return selected - _soldiers.size() - _crafts.size() - _hasSci - _hasEng;
}

}
