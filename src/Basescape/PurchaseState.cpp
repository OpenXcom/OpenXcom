/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "../aresame.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
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
#include "../Ruleset/RuleCraftWeapon.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Purchase/Hire screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
PurchaseState::PurchaseState(Game *game, Base *base) : State(game), _base(base), _crafts(), _items(), _qtys(), _qtysPersonnel(), _qtysCraft(), _sel(0), _total(0), _pQty(0), _cQty(0), _iQty(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtFunds = new Text(150, 9, 10, 24);
	_txtPurchases = new Text(150, 9, 160, 24);
	_btnPrev = new TextButton(28, 14, 8, 34);
	_btnTab = new TextButton(56, 14, 38, 34);
	_btnNext = new TextButton(28, 14, 96, 34);
	_txtItem = new Text(130, 9, 30, 50);
	_txtCost = new Text(65, 18, 139, 42);
	_txtInStorage = new Text(45, 18, 205, 42);
	_txtQuantity = new Text(60, 9, 255, 52);
	_lstPersonnel = new TextList(288, 104, 8, 62);
	_lstCraft = new TextList(288, 104, 8, 62);
	_lstItems = new TextList(288, 104, 8, 62);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtFunds);
	add(_txtPurchases);
	add(_btnPrev);
	add(_btnTab);
	add(_btnNext);;
	add(_txtItem);
	add(_txtCost);
	add(_txtInStorage);
	add(_txtQuantity);
	add(_lstPersonnel);
	add(_lstCraft);
	add(_lstItems);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&PurchaseState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&PurchaseState::btnOkClick, Options::keyOk);

	_btnCancel->setColor(Palette::blockOffset(13)+10);
	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&PurchaseState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&PurchaseState::btnCancelClick, Options::keyCancel);

	_btnPrev->setColor(Palette::blockOffset(13)+10);
	_btnPrev->setText(L"<<");
	_btnPrev->onMouseClick((ActionHandler)&PurchaseState::btnPrevClick);

	_btnTab->setColor(Palette::blockOffset(13)+10);
	_btnTab->onMouseClick((ActionHandler)&PurchaseState::btnTabClick, 0);

	_btnNext->setColor(Palette::blockOffset(13)+10);
	_btnNext->setText(L">>");
	_btnNext->onMouseClick((ActionHandler)&PurchaseState::btnNextClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_PURCHASE_HIRE_PERSONNEL"));

	_txtFunds->setColor(Palette::blockOffset(13)+10);
	_txtFunds->setSecondaryColor(Palette::blockOffset(13));
	_txtFunds->setText(tr("STR_CURRENT_FUNDS").arg(Text::formatFunding(_game->getSavedGame()->getFunds())));

	_txtPurchases->setColor(Palette::blockOffset(13)+10);
	_txtPurchases->setSecondaryColor(Palette::blockOffset(13));
	_txtPurchases->setText(tr("STR_COST_OF_PURCHASES").arg(Text::formatFunding(_total)));

	_txtItem->setColor(Palette::blockOffset(13)+5);
	_txtItem->setText(tr("STR_ITEM_LC"));

	_txtCost->setColor(Palette::blockOffset(13)+5);
	_txtCost->setText(tr("STR_COST_PER_UNIT"));
	_txtCost->setWordWrap(true);
	_txtCost->setVerticalAlign(ALIGN_BOTTOM);

	_txtQuantity->setColor(Palette::blockOffset(13)+5);
	_txtQuantity->setText(tr("STR_QUANTITY"));

	_txtInStorage->setColor(Palette::blockOffset(13)+5);
	_txtInStorage->setText(tr("STR_ON_BASE"));
	_txtInStorage->setWordWrap(true);
	_txtInStorage->setVerticalAlign(ALIGN_BOTTOM);

	_lstPersonnel->setColor(Palette::blockOffset(13)+10);
	_lstPersonnel->setArrowColumn(230, ARROW_VERTICAL);
	_lstPersonnel->setColumns(4, 150, 60, 50, 32);
	_lstPersonnel->setSelectable(true);
	_lstPersonnel->setBackground(_window);
	_lstPersonnel->setMargin(2);
	_lstPersonnel->setVisible(false);

	_lstCraft->setColor(Palette::blockOffset(13)+10);
	_lstCraft->setArrowColumn(230, ARROW_VERTICAL);
	_lstCraft->setColumns(4, 150, 60, 50, 32);
	_lstCraft->setSelectable(true);
	_lstCraft->setBackground(_window);
	_lstCraft->setMargin(2);
	_lstCraft->setVisible(false);
	_lstCraft->onLeftArrowPress((ActionHandler)&PurchaseState::lstItemsLeftArrowPress);
	_lstCraft->onLeftArrowRelease((ActionHandler)&PurchaseState::lstItemsLeftArrowRelease);
	_lstCraft->onRightArrowPress((ActionHandler)&PurchaseState::lstItemsRightArrowPress);
	_lstCraft->onRightArrowRelease((ActionHandler)&PurchaseState::lstItemsRightArrowRelease);

	_lstItems->setColor(Palette::blockOffset(13)+10);
	_lstItems->setArrowColumn(230, ARROW_VERTICAL);
	_lstItems->setColumns(4, 150, 60, 50, 32);
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

	_lists.push_back(_lstPersonnel);
	_lists.push_back(_lstCraft);
	_lists.push_back(_lstItems);

	_tabs.push_back("STR_PERSONNEL");
	_tabs.push_back("STR_CRAFT");
	_tabs.push_back("STR_ITEMS");

	// start on items tab
	_selTab = TAB_ITEMS;
	_btnTab->setText(tr("STR_ITEMS"));
	_selList = _lstItems;

	_qtysPersonnel.push_back(0);
	std::wostringstream ss;
	ss << _base->getTotalSoldiers();
	_lstPersonnel->addRow(4, tr("STR_SOLDIER").c_str(), Text::formatFunding(_game->getRuleset()->getSoldierCost() * 2).c_str(), ss.str().c_str(), L"0");

	_qtysPersonnel.push_back(0);
	std::wostringstream ss2;
	ss2 << _base->getTotalScientists();
	_lstPersonnel->addRow(4, tr("STR_SCIENTIST").c_str(), Text::formatFunding(_game->getRuleset()->getScientistCost() * 2).c_str(), ss2.str().c_str(), L"0");

	_qtysPersonnel.push_back(0);
	std::wostringstream ss3;
	ss3 << _base->getTotalEngineers();
	_lstPersonnel->addRow(4, tr("STR_ENGINEER").c_str(), Text::formatFunding(_game->getRuleset()->getEngineerCost() * 2).c_str(), ss3.str().c_str(), L"0");

	const std::vector<std::string> &crafts = _game->getRuleset()->getCraftsList();
	for (std::vector<std::string>::const_iterator i = crafts.begin(); i != crafts.end(); ++i)
	{
		RuleCraft *rule = _game->getRuleset()->getCraft(*i);
		if (rule->getBuyCost() > 0 && _game->getSavedGame()->isResearched(rule->getRequirements()))
		{
			_crafts.push_back(*i);
			_qtysCraft.push_back(0);
			int crafts = 0;
			for (std::vector<Craft*>::iterator c = _base->getCrafts()->begin(); c != _base->getCrafts()->end(); ++c)
			{
				if ((*c)->getRules()->getType() == *i)
					crafts++;
			}
			std::wostringstream ss4;
			ss4 << crafts;
			_lstCraft->addRow(4, tr(*i).c_str(), Text::formatFunding(rule->getBuyCost()).c_str(), ss4.str().c_str(), L"0");
		}
	}
	std::vector<std::string> items = _game->getRuleset()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		RuleItem *rule = _game->getRuleset()->getItem(*i);
		if (rule->getBuyCost() > 0 && !isExcluded(*i))
		{
			std::wostringstream ss5;
			ss5 << _base->getItems()->getItem(*i);
			std::wstring item = tr(*i);
			if (rule->getSubCategory() == "CRAFT_WEAPON")
			{
				_craftItems.push_back(*i);
				_qtysCraft.push_back(0);
				if (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0)
				{
					item.insert(0, L"  ");
					_lstCraft->addRow(4, item.c_str(), Text::formatFunding(rule->getBuyCost()).c_str(), ss5.str().c_str(), L"0");
					_lstCraft->setRowColor(_qtysCraft.size() - 1, Palette::blockOffset(15) + 6);
				}
				else
				{
					_lstCraft->addRow(4, item.c_str(), Text::formatFunding(rule->getBuyCost()).c_str(), ss5.str().c_str(), L"0");
				}
			}
			else
			{
				_items.push_back(*i);
				_qtys.push_back(0);
				if (rule->getBattleType() == BT_AMMO)
				{
					item.insert(0, L"  ");
					_lstItems->addRow(4, item.c_str(), Text::formatFunding(rule->getBuyCost()).c_str(), ss5.str().c_str(), L"0");
					_lstItems->setRowColor(_qtys.size() - 1, Palette::blockOffset(15) + 6);
				}
				else
				{
					_lstItems->addRow(4, item.c_str(), Text::formatFunding(rule->getBuyCost()).c_str(), ss5.str().c_str(), L"0");
				}
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
bool PurchaseState::isExcluded(std::string item)
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
	// Hire the personnel
	for (unsigned int i = 0; i < _qtysPersonnel.size(); ++i)
	{
		if (_qtysPersonnel[i] > 0)
		{
			// Buy soldiers
			if (i == 0)
			{
				for (int s = 0; s < _qtysPersonnel[i]; s++)
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
				t->setScientists(_qtysPersonnel[i]);
				_base->getTransfers()->push_back(t);
			}
			// Buy engineers
			else
			{
				Transfer *t = new Transfer(_game->getRuleset()->getPersonnelTime());
				t->setEngineers(_qtysPersonnel[i]);
				_base->getTransfers()->push_back(t);
			}
		}
	}
	// Buy crafts and craft items
	for (unsigned int i = 0; i < _qtysCraft.size(); ++i)
	{
		if (_qtysCraft[i] > 0)
		{
			// Buy crafts
			if (i < _crafts.size())
			{
				for (int c = 0; c < _qtysCraft[i]; c++)
				{
					RuleCraft *rc = _game->getRuleset()->getCraft(_crafts[i]);
					Transfer *t = new Transfer(rc->getTransferTime());
					Craft *craft = new Craft(rc, _base, _game->getSavedGame()->getId(_crafts[i]));
					craft->setStatus("STR_REFUELLING");
					t->setCraft(craft);
					_base->getTransfers()->push_back(t);
				}
			}
			// Buy craft items
			else
			{
				RuleItem *ri = _game->getRuleset()->getItem(_craftItems[i - _crafts.size()]);
				Transfer *t = new Transfer(ri->getTransferTime());
				t->setItems(_craftItems[i - _crafts.size()], _qtysCraft[i]);
				_base->getTransfers()->push_back(t);
			}
		}
	}

	// Buy items
	for (unsigned int i = 0; i < _qtys.size(); ++i)
	{
		if (_qtys[i] > 0)
		{
			RuleItem *ri = _game->getRuleset()->getItem(_items[i]);
			Transfer *t = new Transfer(ri->getTransferTime());
			t->setItems(_items[i], _qtys[i]);
			_base->getTransfers()->push_back(t);
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
 * Updates the list index.
 * @param index Index value.
 * @param list List the index belongs to.
 * @param change Amount to change the index.
 */
void PurchaseState::updateIndex(size_t &index, std::vector<std::string> &list, int change)
{
	int i = index;
	if (i + change >= (int)list.size())
	{
		index = 0;
	}
	else if (i + change < 0)
	{
		index = list.size() - 1;
	}
	else
	{
		index += change;
	}
}

/**
 * Makes the the next list visible.
 * @param action Pointer to an action.
 */
void PurchaseState::btnTabClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		updateIndex(_selTab, _tabs, 1);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		updateIndex(_selTab, _tabs, -1);
	}
	_btnTab->setText(tr(_tabs[_selTab]));
	_selList = _lists[_selTab];

	for(std::vector<TextList*>::iterator it = _lists.begin(); it != _lists.end(); ++it)
	{
		(*it)->setVisible(false);
	}
	_selList->setVisible(true);
}

/**
 * Goes to the previous list.
 * @param action Pointer to an action.
 */
void PurchaseState::btnPrevClick(Action *)
{
	updateIndex(_selTab, _tabs, -1);
	_btnTab->setText(tr(_tabs[_selTab]));
	_selList = _lists[_selTab];
	for(std::vector<TextList*>::iterator it = _lists.begin(); it != _lists.end(); ++it)
	{
		(*it)->setVisible(false);
	}
	_selList->setVisible(true);
}

/**
 * Goes to the next list.
 * @param action Pointer to an action.
 */
void PurchaseState::btnNextClick(Action *)
{
	updateIndex(_selTab, _tabs, 1);
	_btnTab->setText(tr(_tabs[_selTab]));
	_selList = _lists[_selTab];
	for(std::vector<TextList*>::iterator it = _lists.begin(); it != _lists.end(); ++it)
	{
		(*it)->setVisible(false);
	}
	_selList->setVisible(true);
}

/**
 * Starts increasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsLeftArrowPress(Action *action)
{
	if (_selTab == TAB_ITEMS)
	{
		_sel = _lstItems->getSelectedRow();
	}
	else if (_selTab == TAB_CRAFT)
	{
		_sel = _lstCraft->getSelectedRow();
	}
	else
	{
		_sel = _lstPersonnel->getSelectedRow();
	}
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
	if (_selTab == TAB_ITEMS)
	{
		_sel = _lstItems->getSelectedRow();
	}
	else if (_selTab == TAB_CRAFT)
	{
		_sel = _lstCraft->getSelectedRow();
	}
	else
	{
		_sel = _lstPersonnel->getSelectedRow();
	}
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
	if (_selTab == TAB_PERSONNEL)
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
		else
		{
			return _game->getRuleset()->getEngineerCost() * 2;
		}
	}
	// Craft cost
	else if (_selTab == TAB_CRAFT)
	{
		// Is it a craft?
		if (_sel < 2)
		{
			return _game->getRuleset()->getCraft(_crafts[_sel])->getBuyCost();
		}
		// Or a craft related item
		else
		{
			return _game->getRuleset()->getItem(_craftItems[_sel - _crafts.size()])->getBuyCost();
		}
	}
	// Item cost
	else
	{
		return _game->getRuleset()->getItem(_items[_sel])->getBuyCost();
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
	if (_total + getPrice() > _game->getSavedGame()->getFunds())
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NOT_ENOUGH_MONEY", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
	}
	else if ((_selTab == TAB_PERSONNEL) && (_pQty + 1 > _base->getAvailableQuarters() - _base->getUsedQuarters()))
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NOT_ENOUGH_LIVING_SPACE", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
	}
	// Is it a craft
	else if ((_selTab == TAB_CRAFT) && (_sel < _crafts.size()) && (_cQty + 1 > _base->getAvailableHangars() - _base->getUsedHangars()))
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NO_FREE_HANGARS_FOR_PURCHASE", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
	}
	// Or a craft item
	else if ((_selTab == TAB_CRAFT) && (_sel >= _crafts.size())
		&& _iQty + (int)(10 * _game->getRuleset()->getItem(_craftItems[_sel - _crafts.size()])->getSize() > (10 * _base->getAvailableStores() - (int)(10 *_base->getExactUsedStores() + 0.5))))
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NOT_ENOUGH_STORE_SPACE", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
	}
	else if ((_selTab == TAB_ITEMS)
		&& _iQty + (int)(10 * _game->getRuleset()->getItem(_items[_sel])->getSize()) > (10 * _base->getAvailableStores() - (int)(10 *_base->getExactUsedStores() + 0.5)))
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NOT_ENOUGH_STORE_SPACE", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
	}
	else
	{
		int maxByMoney = (_game->getSavedGame()->getFunds() - _total) / getPrice();
		change = std::min(maxByMoney, change);
		if (_selTab == TAB_PERSONNEL)
		{
			// Personnel count
			int maxByQuarters = _base->getAvailableQuarters() - _base->getUsedQuarters() - _pQty;
			change = std::min(maxByQuarters, change);
			_pQty += change;
			_qtysPersonnel[_sel] += change;
		}
		else if (_selTab == TAB_CRAFT && _sel < _crafts.size())
		{
			// Craft count
			int maxByHangars = _base->getAvailableHangars() - _base->getUsedHangars() - _cQty;
			change = std::min(maxByHangars, change);
			_cQty += change;
			_qtysCraft[_sel] += change;
		}
		else
		{
			// Item count
			int storesNeededPerItem;
			if (_selTab == TAB_CRAFT)
				storesNeededPerItem = (int)(10 *_game->getRuleset()->getItem(_craftItems[_sel - _crafts.size()])->getSize());
			else
				storesNeededPerItem = (int)(10 *_game->getRuleset()->getItem(_items[_sel])->getSize());
			int freeStores = 10 * _base->getAvailableStores() - (int)(10 * _base->getUsedStores()) - _iQty;
			int maxByStores;

			if (storesNeededPerItem == 0)
			{
				maxByStores = INT_MAX;
			}
			else
			{
				maxByStores = floor(freeStores / storesNeededPerItem);
			}
			change = std::min(maxByStores, change);
			_iQty += change * storesNeededPerItem;
			if (_selTab == TAB_CRAFT)
				_qtysCraft[_sel] += change;
			else
				_qtys[_sel] += change;
		}

		_total += getPrice() * change;
		updateItemStrings();
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
	if (0 >= change) return;

	if (_selTab == TAB_PERSONNEL)
 	{
		// Personnel count
		if (0 >= _qtysPersonnel[_sel]) return;
		change = std::min(_qtysPersonnel[_sel], change);
		_pQty -= change;
		_qtysPersonnel[_sel] -= change;
	}
	else if (_selTab == TAB_CRAFT && _sel < _crafts.size())
	{
		// Craft count
		if (0 >= _qtysCraft[_sel]) return;
		change = std::min(_qtysCraft[_sel], change);
		_cQty -= change;
		_qtysCraft[_sel] -= change;
	}
	else
	{
		// Item count
		if (0 >= _qtys[_sel]) return;

		if (_selTab == TAB_CRAFT)
		{
			change = std::min(_qtysCraft[_sel - _crafts.size()], change);
			_iQty -= (int)(10 *_game->getRuleset()->getItem(_craftItems[_sel - _crafts.size()])->getSize()) * change;
			_qtysCraft[_sel] -= change;
		}
		else
		{
			change = std::min(_qtys[_sel], change);
			_iQty -= (int)(10 *_game->getRuleset()->getItem(_items[_sel])->getSize()) * change;
			_qtys[_sel] -= change;
		}
	}

	_total -= getPrice() * change;
	updateItemStrings();
}

/**
 * Updates the quantity-strings of the selected item.
 */
void PurchaseState::updateItemStrings()
{
	_txtPurchases->setText(tr("STR_COST_OF_PURCHASES").arg(Text::formatFunding(_total)));
	std::wostringstream ss;
	if (_selTab == TAB_PERSONNEL)
	{
		ss << _qtysPersonnel[_sel];
		_lstPersonnel->setCellText(_sel, 3, ss.str());
		if (_qtysPersonnel[_sel] > 0)
		{
			_lstPersonnel->setRowColor(_sel, Palette::blockOffset(13));
		}
		else
		{
			_lstPersonnel->setRowColor(_sel, Palette::blockOffset(13) + 10);
		}
	}
	else if (_selTab == TAB_CRAFT)
	{
		ss << _qtysCraft[_sel];
		_lstCraft->setCellText(_sel, 3, ss.str());

		if (_qtysCraft[_sel] == 0)
		{
			if (_sel < _crafts.size())
			{
				_lstCraft->setRowColor(_sel, Palette::blockOffset(13) + 10);
			}
			else
			{
				RuleItem *rule = _game->getRuleset()->getItem(_craftItems[_sel - _crafts.size()]);
				if (rule->getClipSize() > 0)
				{
					_lstCraft->setRowColor(_sel, Palette::blockOffset(15) + 6);
				}
				else
				{
					_lstCraft->setRowColor(_sel, Palette::blockOffset(13) + 10);
				}
			}
		}
		else
		{
			_lstCraft->setRowColor(_sel, Palette::blockOffset(13));
		}
	}
	else
	{
		ss << _qtys[_sel];
		_lstItems->setCellText(_sel, 3, ss.str());

		RuleItem *rule = _game->getRuleset()->getItem(_items[_sel]);

		if (rule->getBattleType() == BT_AMMO && _qtys[_sel] == 0)
		{
			_lstItems->setRowColor(_sel, Palette::blockOffset(15) + 6);
		}
		else if (_qtys[_sel] > 0)
		{
			_lstItems->setRowColor(_sel, Palette::blockOffset(13));
		}
		else
		{
			_lstItems->setRowColor(_sel, Palette::blockOffset(13) + 10);
		}
	}
}

}
