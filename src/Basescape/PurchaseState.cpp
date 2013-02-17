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
#include "PurchaseState.h"
#include <sstream>
#include <climits>
#include <cmath>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Timer.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleItem.h"
#include "../Savegame/Base.h"
#include "../Engine/Action.h"
#include "../Savegame/Transfer.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/ItemContainer.h"
#include "../Menu/ErrorMessageState.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Purchase/Hire screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
PurchaseState::PurchaseState(Game *game, Base *base) : State(game), _base(base), _crafts(), _items(), _qtys(), _sel(0), _total(0), _pQty(0), _cQty(0), _iQty(0.0f)
{
	bool allowChangeListValuesByMouseWheel=Options::getBool("allowChangeListValuesByMouseWheel");
	_changeValueByMouseWheel = Options::getInt("changeValueByMouseWheel");

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtFunds = new Text(150, 9, 10, 24);
	_txtPurchases = new Text(150, 9, 160, 24);
	_txtItem = new Text(140, 9, 10, 32);
	_txtCost = new Text(102, 9, 152, 32);
	_txtQuantity = new Text(60, 9, 256, 32);
	_lstItems = new TextList(287, 128, 8, 40);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtFunds);
	add(_txtPurchases);
	add(_txtItem);
	add(_txtCost);
	add(_txtQuantity);
	add(_lstItems);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&PurchaseState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(13)+10);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&PurchaseState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_PURCHASE_HIRE_PERSONNEL"));

	_txtFunds->setColor(Palette::blockOffset(13)+10);
	_txtFunds->setSecondaryColor(Palette::blockOffset(13));
	std::wstring s1 = _game->getLanguage()->getString("STR_CURRENT_FUNDS");
	s1 += L'\x01' + Text::formatFunding(_game->getSavedGame()->getFunds());
	_txtFunds->setText(s1);

	_txtPurchases->setColor(Palette::blockOffset(13)+10);
	_txtPurchases->setSecondaryColor(Palette::blockOffset(13));
	std::wstring s2 = _game->getLanguage()->getString("STR_COST_OF_PURCHASES");
	s2 += L'\x01' + Text::formatFunding(_total);
	_txtPurchases->setText(s2);

	_txtItem->setColor(Palette::blockOffset(13)+10);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtCost->setColor(Palette::blockOffset(13)+10);
	_txtCost->setText(_game->getLanguage()->getString("STR_COST_PER_UNIT_UC"));

	_txtQuantity->setColor(Palette::blockOffset(13)+10);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_lstItems->setColor(Palette::blockOffset(13)+10);
	_lstItems->setArrowColumn(227, ARROW_VERTICAL);
	_lstItems->setColumns(4, 150, 55, 46, 32);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);
	if (allowChangeListValuesByMouseWheel) _lstItems->setAllowScrollOnArrowButtons(false);
	_lstItems->onLeftArrowPress((ActionHandler)&PurchaseState::lstItemsLeftArrowPress);
	_lstItems->onLeftArrowRelease((ActionHandler)&PurchaseState::lstItemsLeftArrowRelease);
	_lstItems->onLeftArrowClick((ActionHandler)&PurchaseState::lstItemsLeftArrowClick);
	_lstItems->onRightArrowPress((ActionHandler)&PurchaseState::lstItemsRightArrowPress);
	_lstItems->onRightArrowRelease((ActionHandler)&PurchaseState::lstItemsRightArrowRelease);
	_lstItems->onRightArrowClick((ActionHandler)&PurchaseState::lstItemsRightArrowClick);
	if (allowChangeListValuesByMouseWheel) _lstItems->onMousePress((ActionHandler)&PurchaseState::lstItemsMousePress);

	_qtys.push_back(0);
	std::wstringstream ss;
	ss << _base->getTotalSoldiers();
	_lstItems->addRow(4, _game->getLanguage()->getString("STR_SOLDIER").c_str(), Text::formatFunding(_game->getRuleset()->getSoldierCost() * 2).c_str(), ss.str().c_str(), L"0");
	_qtys.push_back(0);
	std::wstringstream ss2;
	ss2 << _base->getTotalScientists();
	_lstItems->addRow(4, _game->getLanguage()->getString("STR_SCIENTIST").c_str(), Text::formatFunding(_game->getRuleset()->getScientistCost() * 2).c_str(), ss2.str().c_str(), L"0");
	_qtys.push_back(0);
	std::wstringstream ss3;
	ss3 << _base->getTotalEngineers();
	_lstItems->addRow(4, _game->getLanguage()->getString("STR_ENGINEER").c_str(), Text::formatFunding(_game->getRuleset()->getEngineerCost() * 2).c_str(), ss3.str().c_str(), L"0");

	const std::vector<std::string> &crafts = _game->getRuleset()->getCraftsList();
	for (std::vector<std::string>::const_iterator i = crafts.begin(); i != crafts.end(); ++i)
	{
		if (_game->getRuleset()->getCraft(*i)->getBuyCost() > 0)
		{
			_crafts.push_back(*i);
			_qtys.push_back(0);
			int crafts = 0;
			for (std::vector<Craft*>::iterator c = _base->getCrafts()->begin(); c != _base->getCrafts()->end(); ++c)
			{
				if ((*c)->getRules()->getType() == *i)
					crafts++;
			}
			std::wstringstream ss4;
			ss4 << crafts;
			_lstItems->addRow(4, _game->getLanguage()->getString(*i).c_str(), Text::formatFunding(_game->getRuleset()->getCraft(*i)->getBuyCost()).c_str(), ss4.str().c_str(), L"0");
		}
	}
	const std::vector<std::string> &items = _game->getRuleset()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		if (_game->getRuleset()->getItem(*i)->getBuyCost() > 0)
		{
			_items.push_back(*i);
			_qtys.push_back(0);
			std::wstringstream ss5;
			ss5 << _base->getItems()->getItem(*i);
			_lstItems->addRow(4, _game->getLanguage()->getString(*i).c_str(), Text::formatFunding(_game->getRuleset()->getItem(*i)->getBuyCost()).c_str(), ss5.str().c_str(), L"0");
		}
	}

	_timerInc = new Timer(50);
	_timerInc->onTimer((StateHandler)&PurchaseState::increase);
	_timerDec = new Timer(50);
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
 * Purchases the selected items.
 * @param action Pointer to an action.
 */
void PurchaseState::btnOkClick(Action *)
{
	_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() - _total);
	for (unsigned int i = 0; i < _qtys.size(); ++i)
	{
		if (_qtys[i] > 0)
		{
			// Buy soldiers
			if (i == 0)
			{
				for (int s = 0; s < _qtys[i]; s++)
				{
					Transfer *t = new Transfer(_game->getRuleset()->getPersonnelTime());
					t->setSoldier(new Soldier(_game->getRuleset()->getSoldier("XCOM"), _game->getRuleset()->getArmor("STR_NONE_UC"), &_game->getRuleset()->getPools(), _game->getSavedGame()->getId("STR_SOLDIER")));
					_base->getTransfers()->push_back(t);
				}
			}
			// Buy scientists
			else if (i == 1)
			{
				Transfer *t = new Transfer(_game->getRuleset()->getPersonnelTime());
				t->setScientists(_qtys[i]);
				_base->getTransfers()->push_back(t);
			}
			// Buy engineers
			else if (i == 2)
			{
				Transfer *t = new Transfer(_game->getRuleset()->getPersonnelTime());
				t->setEngineers(_qtys[i]);
				_base->getTransfers()->push_back(t);
			}
			// Buy crafts
			else if (i >= 3 && i < 3 + _crafts.size())
			{
				for (int c = 0; c < _qtys[i]; c++)
				{
					RuleCraft *rc = _game->getRuleset()->getCraft(_crafts[i - 3]);
					Transfer *t = new Transfer(rc->getTransferTime());
					Craft *craft = new Craft(rc, _base, _game->getSavedGame()->getId(_crafts[i - 3]));
					craft->setStatus("STR_REFUELLING");
					t->setCraft(craft);
					_base->getTransfers()->push_back(t);
					craft->setName(L"", _game->getLanguage());
				}
			}
			// Buy items
			else
			{
				RuleItem *ri = _game->getRuleset()->getItem(_items[i - 3 - _crafts.size()]);
				Transfer *t = new Transfer(ri->getTransferTime());
				t->setItems(_items[i - 3 - _crafts.size()], _qtys[i]);
				_base->getTransfers()->push_back(t);
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
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerInc->start();
}

/**
 * Stops increasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsLeftArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerInc->stop();
}

/**
 * Increases the item to max on right-click.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsLeftArrowClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) increase(INT_MAX);
}

/**
 * Starts decreasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerDec->start();
}

/**
 * Stops decreasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerDec->stop();
}

/**
 * Decreases the item to 0 on right-click.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) decrease(INT_MAX);
}

/**
 * Handles the mouse-wheels on the arrow-buttons.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsMousePress(Action *action)
{
	if (action->getAbsoluteXMouse() >= _lstItems->getArrowsLeftEdge() && action->getAbsoluteXMouse() <= _lstItems->getArrowsRightEdge())
	{
		_sel = _lstItems->getSelectedRow();
		if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP) increase(_changeValueByMouseWheel);
		else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN) decrease(_changeValueByMouseWheel);
	}
}

/**
 * Gets the price of the currently selected item.
 */
int PurchaseState::getPrice()
{
	// Soldier cost
	if (_sel == 0)
	{
		return _game->getRuleset()->getSoldierCost() * 2;
	}
	// Scientist cost
	else if (_sel == 1)
	{
		return _game->getRuleset()->getScientistCost() * 2;
	}
	// Engineer cost
	else if (_sel == 2)
	{
		return _game->getRuleset()->getEngineerCost() * 2;
	}
	// Craft cost
	else if (_sel >= 3 && _sel < 3 + _crafts.size())
	{
		return _game->getRuleset()->getCraft(_crafts[_sel - 3])->getBuyCost();
	}
	// Item cost
	else
	{
		return _game->getRuleset()->getItem(_items[_sel - 3 - _crafts.size()])->getBuyCost();
	}
}

/**
 * Increases the quantity of the selected item to buy by one.
 */
void PurchaseState::increase()
{
	increase(1);
}

/**
 * Increases the quantity of the selected item to buy by "change".
 * @param change how much we want to add
 */
void PurchaseState::increase(int change)
{
	if (0 >= change) return;
	if (_total + getPrice() > _game->getSavedGame()->getFunds())
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NOT_ENOUGH_MONEY", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
	}
	else if (_sel <= 2 && _pQty + 1 > _base->getAvailableQuarters() - _base->getUsedQuarters())
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NOT_ENOUGH_LIVING_SPACE", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
	}
	else if (_sel >= 3 && _sel < 3 + _crafts.size() && _cQty + 1 > _base->getAvailableHangars() - _base->getUsedHangars())
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NO_FREE_HANGARS_FOR_PURCHASE", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
	}
	else if (_sel >= 3 + _crafts.size() && _iQty + _game->getRuleset()->getItem(_items[_sel - 3 - _crafts.size()])->getSize() > _base->getAvailableStores() - _base->getUsedStores())
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NOT_ENOUGH_STORE_SPACE", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
	}
	else
	{
		int maxByMoney = (_game->getSavedGame()->getFunds() - _total) / getPrice();
		change = std::min(maxByMoney, change);
		if (_sel <= 2)
		{
			// Personnel count
			int maxByQuarters = _base->getAvailableQuarters() - _base->getUsedQuarters() - _pQty;
			change = std::min(maxByQuarters, change);
			_pQty += change;
		}
		else if (_sel >= 3 && _sel < 3 + _crafts.size())
		{
			// Craft count
			int maxByHangars = _base->getAvailableHangars() - _base->getUsedHangars() - _cQty;
			change = std::min(maxByHangars, change);
			_cQty += change;
		}
		else if (_sel >= 3 + _crafts.size())
		{
			// Item count
			float storesNeededPerItem = _game->getRuleset()->getItem(_items[_sel - 3 - _crafts.size()])->getSize();
			float freeStores = (float)(_base->getAvailableStores() - _base->getUsedStores()) - _iQty;
			int maxByStores;
			if (0 == storesNeededPerItem) maxByStores = INT_MAX;
			else maxByStores = floor(freeStores / storesNeededPerItem);
			change = std::min(maxByStores, change);
			_iQty += ((float)(change)) * storesNeededPerItem;
		}
		_qtys[_sel] += change;
		_total += getPrice() * change;
		updateItemStrings();
	}
}

/**
 * Decreases the quantity of the selected item to buy by one.
 */
void PurchaseState::decrease()
{
	decrease(1);
}

/**
 * Decreases the quantity of the selected item to buy by "change".
 * @param change how much we want to add
 */
void PurchaseState::decrease(int change)
{
	if (0 >= change || 0 >= _qtys[_sel]) return;
	change = std::min(_qtys[_sel], change);
	// Personnel count
	if (_sel <= 2) _pQty -= change;
	// Craft count
	else if (_sel >= 3 && _sel < 3 + _crafts.size()) _cQty -= change;
	// Item count
	else _iQty -= _game->getRuleset()->getItem(_items[_sel - 3 - _crafts.size()])->getSize() * ((float)(change));
	_qtys[_sel] -= change;
	_total -= getPrice() * change;
	updateItemStrings();
}

/**
 * Updates the quantity-strings of the selected item.
 */
void PurchaseState::updateItemStrings()
{
	std::wstring s = _game->getLanguage()->getString("STR_COST_OF_PURCHASES");
	s += L'\x01' + Text::formatFunding(_total);
	_txtPurchases->setText(s);
	std::wstringstream ss;
	ss << _qtys[_sel];
	_lstItems->setCellText(_sel, 3, ss.str());
}

}
