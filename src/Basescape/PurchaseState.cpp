/*
 * Copyright 2010 OpenXcom Developers.
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
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
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
#include "PurchaseErrorState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Purchase/Hire screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
PurchaseState::PurchaseState(Game *game, Base *base) : State(game), _base(base), _crafts(), _items(), _qtys(), _sel(0), _total(0), _pQty(0), _cQty(0), _iQty(0.0f)
{
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
	_lstItems = new TextList(288, 128, 8, 40);

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
	_lstItems->setColumns(4, 152, 58, 44, 32);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);
	_lstItems->onLeftArrowPress((ActionHandler)&PurchaseState::lstItemsLeftArrowPress);
	_lstItems->onLeftArrowRelease((ActionHandler)&PurchaseState::lstItemsLeftArrowRelease);
	_lstItems->onRightArrowPress((ActionHandler)&PurchaseState::lstItemsRightArrowPress);
	_lstItems->onRightArrowRelease((ActionHandler)&PurchaseState::lstItemsRightArrowRelease);

	_crafts.push_back("STR_SKYRANGER");
	_crafts.push_back("STR_INTERCEPTOR");
	_items.push_back("STR_STINGRAY_LAUNCHER");
	_items.push_back("STR_AVALANCHE_LAUNCHER");
	_items.push_back("STR_CANNON");
	_items.push_back("STR_STINGRAY_MISSILES");
	_items.push_back("STR_AVALANCHE_MISSILES");
	_items.push_back("STR_CANNON_ROUNDS_X50");
	_items.push_back("STR_PISTOL");
	_items.push_back("STR_PISTOL_CLIP");
	_items.push_back("STR_RIFLE");
	_items.push_back("STR_RIFLE_CLIP");
	_items.push_back("STR_HEAVY_CANNON");
	_items.push_back("STR_HC_AP_AMMO");
	_items.push_back("STR_HC_HE_AMMO");
	_items.push_back("STR_HC_I_AMMO");
	_items.push_back("STR_AUTO_CANNON");
	_items.push_back("STR_AC_AP_AMMO");
	_items.push_back("STR_AC_HE_AMMO");
	_items.push_back("STR_AC_I_AMMO");
	_items.push_back("STR_ROCKET_LAUNCHER");
	_items.push_back("STR_SMALL_ROCKET");
	_items.push_back("STR_LARGE_ROCKET");
	_items.push_back("STR_INCENDIARY_ROCKET");
	_items.push_back("STR_GRENADE");
	_items.push_back("STR_SMOKE_GRENADE");

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
	for (std::vector<std::string>::iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		_qtys.push_back(0);
		int crafts = 0;
		for (std::vector<Craft*>::iterator c = _base->getCrafts()->begin(); c != _base->getCrafts()->end(); ++c)
		{
			if ((*c)->getRules()->getType() == *i)
				crafts++;
		}
		std::wstringstream ss4;
		ss4 << crafts;
		_lstItems->addRow(4, _game->getLanguage()->getString(*i).c_str(), Text::formatFunding(_game->getRuleset()->getCraft(*i)->getCost()).c_str(), ss4.str().c_str(), L"0");
	}
	for (std::vector<std::string>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		_qtys.push_back(0);
		std::wstringstream ss5;
		ss5 << _base->getItems()->getItem(*i);
		_lstItems->addRow(4, _game->getLanguage()->getString(*i).c_str(), Text::formatFunding(_game->getRuleset()->getItem(*i)->getCost()).c_str(), ss5.str().c_str(), L"0");
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
void PurchaseState::btnOkClick(Action *action)
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
					t->setSoldier(new Soldier(_game->getRuleset()->getSoldier("XCOM"), _game->getRuleset()->getArmor("STR_NONE_UC"), _game->getRuleset()->getPools(), _game->getSavedGame()->getSoldierId()));
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
					t->setCraft(new Craft(rc, _base, _game->getSavedGame()->getCraftIds()));
					_base->getTransfers()->push_back(t);
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
void PurchaseState::btnCancelClick(Action *action)
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
	_timerInc->start();
}

/**
 * Stops increasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsLeftArrowRelease(Action *action)
{
	_timerInc->stop();
}

/**
 * Starts decreasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	_timerDec->start();
}

/**
 * Stops decreasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowRelease(Action *action)
{
	_timerDec->stop();
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
		return _game->getRuleset()->getCraft(_crafts[_sel - 3])->getCost();
	}
	// Item cost
	else
	{
		return _game->getRuleset()->getItem(_items[_sel - 3 - _crafts.size()])->getCost();
	}
}

/**
 * Increases the quantity of the selected item to buy.
 */
void PurchaseState::increase()
{
	if (_total + getPrice() > _game->getSavedGame()->getFunds())
	{
		_timerInc->stop();
		_game->pushState(new PurchaseErrorState(_game, "STR_NOT_ENOUGH_MONEY"));
	}
	else if (_sel <= 2 && _pQty + 1 > _base->getAvailableQuarters() - _base->getUsedQuarters())
	{
		_timerInc->stop();
		_game->pushState(new PurchaseErrorState(_game, "STR_NOT_ENOUGH_LIVING_SPACE"));
	}
	else if (_sel >= 3 && _sel < 3 + _crafts.size() && _cQty + 1 > _base->getAvailableHangars() - _base->getUsedHangars())
	{
		_timerInc->stop();
		_game->pushState(new PurchaseErrorState(_game, "STR_NO_FREE_HANGARS_FOR_PURCHASE"));
	}
	else if (_sel >= 3 + _crafts.size() && _iQty + _game->getRuleset()->getItem(_items[_sel - 3 - _crafts.size()])->getSize() > _base->getAvailableStores() - _base->getUsedStores())
	{
		_timerInc->stop();
		_game->pushState(new PurchaseErrorState(_game, "STR_NOT_ENOUGH_STORE_SPACE"));
	}
	else
	{
		// Personnel count
		if (_sel <= 2)
		{
			_pQty++;
		}
		// Craft count
		else if (_sel >= 3 && _sel < 3 + _crafts.size())
		{
			_cQty++;
		}
		// Item count
		else
		{
			_iQty += _game->getRuleset()->getItem(_items[_sel - 3 - _crafts.size()])->getSize();
		}
		_qtys[_sel]++;
		std::wstringstream ss;
		ss << _qtys[_sel];
		_lstItems->setCellText(_sel, 3, ss.str());
		_total += getPrice();
		std::wstring s = _game->getLanguage()->getString("STR_COST_OF_PURCHASES");
		s += L'\x01' + Text::formatFunding(_total);
		_txtPurchases->setText(s);
	}
}

/**
 * Decreases the quantity of the selected item to buy.
 */
void PurchaseState::decrease()
{
	if (_qtys[_sel] > 0)
	{
		// Personnel count
		if (_sel <= 2)
		{
			_pQty--;
		}
		// Craft count
		else if (_sel >= 3 && _sel < 3 + _crafts.size())
		{
			_cQty--;
		}
		// Item count
		else
		{
			_iQty -= _game->getRuleset()->getItem(_items[_sel - 3 - _crafts.size()])->getSize();
		}
		_qtys[_sel]--;
		std::wstringstream ss;
		ss << _qtys[_sel];
		_lstItems->setCellText(_sel, 3, ss.str());
		_total -= getPrice();
		std::wstring s = _game->getLanguage()->getString("STR_COST_OF_PURCHASES");
		s += L'\x01' + Text::formatFunding(_total);
		_txtPurchases->setText(s);
	}
}

}
