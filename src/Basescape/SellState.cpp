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
#include "../Savegame/Transfer.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
#include "PurchaseState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Sell/Sack screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
SellState::SellState(Game *game, Base *base, OptionsOrigin origin) : State(game), _base(base), _soldiers(), _crafts(), _sel(0), _total(0), _spaceChange(0)
{
	_overfull = Options::storageLimitsEnforced && _base->storesOverfull();
	_haveTransfers = false;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);

	_btnOk = new TextButton(_overfull? 288:148, 16, _overfull? 16:8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtSales = new Text(150, 9, 10, 24);
	_txtFunds = new Text(150, 9, 160, 24);
	_btnPrev = new TextButton(28, 14, 8, 34);
	_btnTab = new TextButton(56, 14, 38, 34);
	_btnNext = new TextButton(28, 14, 96, 34);
	_txtSpaceUsed = new Text(150, 8, 160, 34);
	_txtItem = new Text(130, 9, 30, 50);
	_txtQuantity = new Text(54, 9, 112, 50);
	_txtSell = new Text(96, 9, 172, 50);
	_txtValue = new Text(40, 9, 225, 50);
	_txtSpace = new Text(40, 9, 268, 50);
	_lstPersonnel = new TextList(288, 104, 8, 62);
	_lstCraft = new TextList(288, 104, 8, 62);
	_lstItems = new TextList(288, 104, 8, 62);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
	if (origin == OPT_BATTLESCAPE)
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
		_color  = Palette::blockOffset(15)-1;
		_color2 = Palette::blockOffset(8)+10;
		_color3 = Palette::blockOffset(8)+5;
		_colorAmmo = Palette::blockOffset(15)+6;
	}
	else
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);
		_color  = Palette::blockOffset(13)+10;
		_color2 = Palette::blockOffset(13);
		_color3 = Palette::blockOffset(13)+5;
		_colorAmmo = Palette::blockOffset(15)+6;
	}

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_btnPrev);
	add(_btnTab);
	add(_btnNext);
	add(_txtTitle);
	add(_txtSales);
	add(_txtFunds);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtSell);
	add(_txtValue);
	add(_txtSpace);
	add(_txtSpaceUsed);
	add(_lstPersonnel);
	add(_lstCraft);
	add(_lstItems);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(_color);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(_color);
	_btnOk->setText(tr("STR_SELL_SACK"));
	_btnOk->onMouseClick((ActionHandler)&SellState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SellState::btnOkClick, Options::keyOk);

	_btnCancel->setColor(_color);
	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&SellState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&SellState::btnCancelClick, Options::keyCancel);

	if (_overfull)
	{
		_btnCancel->setVisible(false);
		_btnOk->setVisible(false);
	}

	_btnPrev->setColor(_color);
	_btnPrev->setText(L"<<");
	_btnPrev->onMouseClick((ActionHandler)&SellState::btnPrevClick);

	_btnTab->setColor(_color);
	_btnTab->onMouseClick((ActionHandler)&SellState::btnTabClick, 0);

	_btnNext->setColor(_color);
	_btnNext->setText(L">>");
	_btnNext->onMouseClick((ActionHandler)&SellState::btnNextClick);

	_txtTitle->setColor(_color);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_SELL_ITEMS_SACK_PERSONNEL"));

	_txtSales->setColor(_color);
	_txtSales->setSecondaryColor(_color2);
	_txtSales->setText(tr("STR_VALUE_OF_SALES").arg(Text::formatFunding(_total)));

	_txtFunds->setColor(_color);
	_txtFunds->setSecondaryColor(_color2);
	_txtFunds->setText(tr("STR_FUNDS").arg(Text::formatFunding(_game->getSavedGame()->getFunds())));

	_txtItem->setColor(_color3);
	_txtItem->setText(tr("STR_ITEM_LC"));

	_txtQuantity->setColor(_color3);
	_txtQuantity->setText(tr("STR_QUANTITY"));

	_txtSell->setColor(_color3);
	_txtSell->setText(tr("STR_SELL_SACK"));

	_txtValue->setColor(_color3);
	_txtValue->setText(tr("STR_VALUE"));

	_txtSpace->setColor(_color3);
	_txtSpace->setText(tr("STR_SPACE"));

	_txtSpaceUsed->setColor(_color);
	_txtSpaceUsed->setSecondaryColor(_color2);

	std::wostringstream ss5;
	ss5 << _base->getExactUsedStores() << ":" << _base->getAvailableStores();
	_txtSpaceUsed->setText(ss5.str());
	_txtSpaceUsed->setText(tr("STR_SPACE_USED").arg(ss5.str()));

	_lstItems->onLeftArrowPress((ActionHandler)&SellState::lstItemsLeftArrowPress);
	_lstItems->onLeftArrowRelease((ActionHandler)&SellState::lstItemsLeftArrowRelease);
	_lstItems->onLeftArrowClick((ActionHandler)&SellState::lstItemsLeftArrowClick);
	_lstItems->onRightArrowPress((ActionHandler)&SellState::lstItemsRightArrowPress);
	_lstItems->onRightArrowRelease((ActionHandler)&SellState::lstItemsRightArrowRelease);
	_lstItems->onRightArrowClick((ActionHandler)&SellState::lstItemsRightArrowClick);
	_lstItems->onMousePress((ActionHandler)&SellState::lstItemsMousePress);

	_lists.push_back(_lstPersonnel);
	_tabs.push_back("STR_PERSONNEL");
	_containers.push_back(0);

	_lists.push_back(_lstCraft);
	_tabs.push_back("STR_CRAFT");
	_containers.push_back(_base->getItems());

	_lists.push_back(_lstItems);
	_tabs.push_back("STR_ITEMS");
	_containers.push_back(_base->getItems());

	if (_overfull)
	{
		if (!_base->getTransfers()->empty())
		{
			ItemContainer *tItems = new ItemContainer();
			for (std::vector<Transfer*>::iterator t = _base->getTransfers()->begin(); t != _base->getTransfers()->end(); ++t)
			{
				if ((*t)->getType() == TRANSFER_ITEM)
				{
					tItems->addItem((*t)->getItems(), (*t)->getQuantity());
				}
			}
			if (tItems->getTotalQuantity() > 0)
			{
				_haveTransfers = true;
				TextList *lstTransfers = new TextList(288, 104, 8, 62);
				add(lstTransfers);
				_lists.push_back(lstTransfers);
				_tabs.push_back("STR_TRANSFERS_UC");
				_containers.push_back(tItems);
			}
		}

		if (!_base->getCrafts()->empty())
		{
			for (std::vector<Craft*>::iterator c = _base->getCrafts()->begin(); c != _base->getCrafts()->end(); ++c)
			{
				if ((*c)->getItems()->getTotalQuantity() > 0)
				{
					TextList *lstCraftInventory = new TextList(288, 104, 8, 62);
					add(lstCraftInventory);
					_lists.push_back(lstCraftInventory);
					_tabs.push_back(Language::wstrToUtf8((*c)->getName(_game->getLanguage())));
				}
				_containers.push_back((*c)->getItems());
			}
		}
	}

	for (size_t i = 0; i < _lists.size(); i++)
	{
		_quantities.push_back(std::vector<int>());
		_items.push_back(std::vector<std::string>());
	}

	for (std::vector<TextList*>::iterator i = _lists.begin(); i != _lists.end(); ++i)
	{
		(*i)->setColor(_color);
		(*i)->setArrowColumn(145, ARROW_VERTICAL);
		(*i)->setColumns(5, 120, 60, 28, 56, 22);
		(*i)->setSelectable(true);
		(*i)->setBackground(_window);
		(*i)->setMargin(2);
		(*i)->setVisible(false);
	}

	_lstPersonnel->setColumns(4, 120, 60, 28, 56);

	// start on items tab
	_selTab = TAB_ITEMS;
	_btnTab->setText(tr("STR_ITEMS"));
	_selList = _lstItems;
	_selList->setVisible(true);

	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
	{
		if ((*i)->getCraft() == 0)
		{
			_quantities[TAB_PERSONNEL].push_back(0);
			_soldiers.push_back(*i);
			_lstPersonnel->addRow(4, (*i)->getName().c_str(), L"1", L"0", Text::formatFunding(0).c_str());
		}
	}
	for (std::vector<Craft*>::iterator i = _base->getCrafts()->begin(); i != _base->getCrafts()->end(); ++i)
	{
		if (!_overfull && (*i)->getStatus() != "STR_OUT")
		{
			_quantities[TAB_CRAFT].push_back(0);
			_crafts.push_back(*i);
			_lstCraft->addRow(5, (*i)->getName(_game->getLanguage()).c_str(), L"1", L"0", Text::formatFunding((*i)->getRules()->getSellCost()).c_str(), L"-");
		}
	}
	if (_base->getAvailableScientists() > 0)
	{
		_quantities[TAB_PERSONNEL].push_back(0);
		std::wostringstream ss;
		ss << _base->getAvailableScientists();
		_lstPersonnel->addRow(4, tr("STR_SCIENTIST").c_str(), ss.str().c_str(), L"0", Text::formatFunding(0).c_str());
	}
	if (_base->getAvailableEngineers() > 0)
	{
		_quantities[TAB_PERSONNEL].push_back(0);
		std::wostringstream ss;
		ss << _base->getAvailableEngineers();
		_lstPersonnel->addRow(4, tr("STR_ENGINEER").c_str(), ss.str().c_str(), L"0", Text::formatFunding(0).c_str());
	}
	const std::vector<std::string> &items = _game->getRuleset()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		int qty = _base->getItems()->getItem(*i);
		if (qty > 0 && (Options::canSellLiveAliens || !_game->getRuleset()->getItem(*i)->getAlien()))
		{
			RuleItem *rule = _game->getRuleset()->getItem(*i);
			std::wostringstream ss, ss2;
			ss << qty;
			ss2 << qty * rule->getSize();

			std::wstring item = tr(*i);

			if (rule->getSubCategory() == "CRAFT_WEAPON")
			{
				_quantities[TAB_CRAFT].push_back(0);
				_items[TAB_CRAFT].push_back(*i);

				if (rule->getClipSize() > 0)
				{
					item.insert(0, L"  ");
					_lstCraft->addRow(5, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str(), ss2.str().c_str());
					_lstCraft->setRowColor(_quantities[TAB_CRAFT].size() - 1, _colorAmmo);
				}
				else
				{
					_lstCraft->addRow(5, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str(), ss2.str().c_str());
				}
			}
			else
			{
				_quantities[TAB_ITEMS].push_back(0);
				_items[TAB_ITEMS].push_back(*i);

				if (rule->getBattleType() == BT_AMMO || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0))
				{
					item.insert(0, L"  ");
					_lstItems->addRow(5, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str(), ss2.str().c_str());
					_lstItems->setRowColor(_quantities[TAB_ITEMS].size() - 1, _colorAmmo);
				}
				else
				{
					_lstItems->addRow(5, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str(), ss2.str().c_str());
				}
			}
		}
	}

	if (_overfull)
	{
		if (_haveTransfers)
		{
			for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
			{
				int qty = _containers[TAB_TRANSFERS]->getItem(*i);

				if (qty > 0 && (Options::canSellLiveAliens || !_game->getRuleset()->getItem(*i)->getAlien()))
				{
					RuleItem *rule = _game->getRuleset()->getItem(*i);
					std::wstringstream ss, ss2;
					ss << qty;
					ss2 << qty * rule->getSize();

					std::wstring item = tr(*i);

					_quantities[TAB_TRANSFERS].push_back(0);
					_items[TAB_TRANSFERS].push_back(*i);

					if (rule->getBattleType() == BT_AMMO || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0))
					{
						item.insert(0, L"  ");
						_lists[TAB_TRANSFERS]->addRow(5, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str(), ss2.str().c_str());
						_lists[TAB_TRANSFERS]->setRowColor(_quantities[TAB_TRANSFERS].size() - 1, _colorAmmo);
					}
					else
					{
						_lists[TAB_TRANSFERS]->addRow(5, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str(), ss2.str().c_str());
					}
				}
			}
		}

		int craftIndex = _haveTransfers ? 4 : 3;

		for (size_t tab = craftIndex; tab < _lists.size(); tab++)
		{
			for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
			{
				int qty = _containers[tab]->getItem(*i);

				if (qty > 0 && (Options::canSellLiveAliens || !_game->getRuleset()->getItem(*i)->getAlien()))
				{
					RuleItem *rule = _game->getRuleset()->getItem(*i);
					std::wstringstream ss, ss2;
					ss << qty;
					ss2 << qty * rule->getSize();

					std::wstring item = tr(*i);

					_quantities[tab].push_back(0);
					_items[tab].push_back(*i);

					if (rule->getBattleType() == BT_AMMO || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0))
					{
						item.insert(0, L"  ");
						_lists[tab]->addRow(5, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str(), ss2.str().c_str());
						_lists[tab]->setRowColor(_quantities[tab].size() - 1, _colorAmmo);
					}
					else
					{
						_lists[tab]->addRow(5, item.c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getSellCost()).c_str(), ss2.str().c_str());
					}
				}
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
 * Sells the selected items.
 * @param action Pointer to an action.
 */
void SellState::btnOkClick(Action *)
{
	_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() + _total);

	// Sack the personnel
	for (unsigned int i = 0; i < _quantities[TAB_PERSONNEL].size(); ++i)
	{
		if (_quantities[TAB_PERSONNEL][i] > 0)
		{
			if (i < _soldiers.size())
			{
				// Sell soldiers
				for (std::vector<Soldier*>::iterator s = _base->getSoldiers()->begin(); s != _base->getSoldiers()->end(); ++s)
				{
					if (*s == _soldiers[i])
					{
						if((*s)->getArmor()->getStoreItem() != "STR_NONE")
						{
							_base->getItems()->addItem((*s)->getArmor()->getStoreItem());
						}
						_base->getSoldiers()->erase(s);
						break;
					}
				}

				delete _soldiers[i];
			}
			else if (_base->getAvailableScientists() > 0 && i == _soldiers.size())
			{
				// Sell scientists
				_base->setScientists(_base->getScientists() - _quantities[TAB_PERSONNEL][i]);
			}
			else if (_base->getAvailableEngineers() > 0)
			{
				// Sell engineers
				_base->setEngineers(_base->getEngineers() - _quantities[TAB_PERSONNEL][i]);
			}
		}
	}

	// Dump the craft
	for (unsigned int i = 0; i < _quantities[TAB_CRAFT].size(); ++i)
	{
		if (_quantities[TAB_CRAFT][i] <= 0) continue;
		if (i < _crafts.size())
		{
			Craft *craft = _crafts[i];

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
		else
 		{
			// Sell craft related items
			_base->getItems()->removeItem(_items[TAB_CRAFT][i - _crafts.size()], _quantities[TAB_CRAFT][i]);
 		}
	}

	// Sell the items
	for (size_t tab = 0; tab < _lists.size(); tab++)
	{
		if (tab == TAB_CRAFT || tab == TAB_PERSONNEL)
		{
			continue;
		}

		for (unsigned int i = 0; i < _quantities[tab].size(); ++i)
		{
			if (_quantities[tab][i] > 0)
			{
				if (_haveTransfers && tab == TAB_TRANSFERS)
				{
					for (std::vector<Transfer*>::iterator j = _base->getTransfers()->begin(); j != _base->getTransfers()->end() && _quantities[tab][i] != 0 ;)
					{
						if ( (*j)->getItems() == _items[tab][i] )
						{
							if (_quantities[tab][i] == (*j)->getQuantity())
							{
								delete *j;
								j = _base->getTransfers()->erase(j);
								_quantities[tab][i] = 0;
							}
							else if (_quantities[tab][i] >= (*j)->getQuantity())
							{
								_quantities[tab][i] -= (*j)->getQuantity();
								delete *j;
								j = _base->getTransfers()->erase(j);
							}
							else
							{
								Transfer *t = new Transfer((*j)->getHours());
								t->setItems(_items[tab][i], (*j)->getQuantity() - _quantities[tab][i]);
								delete *j;
								j = _base->getTransfers()->erase(j);
								j = _base->getTransfers()->insert(j,t);
								_quantities[tab][i] = 0;
							}
						}
						else
							j++;
					}
				}
				else
				{
					_containers[tab]->removeItem(_items[tab][i], _quantities[tab][i]);
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
 * Updates the list index.
 * @param index Index value.
 * @param list List the index belongs to.
 * @param change Amount to change the index.
 */
void SellState::updateIndex(size_t &index, std::vector<std::string> &list, int change)
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
	if (_overfull)
	{
		// Soldiers might have armor; do not sack right now.
		if (index == TAB_PERSONNEL)
		{
			updateIndex(index, list, change);
		}
	}
}

/**
 * Makes the the next list visible.
 * @param action Pointer to an action.
 */
void SellState::btnTabClick(Action *action)
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
void SellState::btnPrevClick(Action *)
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
void SellState::btnNextClick(Action *)
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
void SellState::lstItemsLeftArrowPress(Action *action)
{
	_sel = _selList->getSelectedRow();
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
void SellState::lstItemsRightArrowPress(Action *action)
{
	_sel = _selList->getSelectedRow();
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
void SellState::lstItemsMousePress(Action *action)
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
 * @return Price of the selected item.
 */
int SellState::getPrice()
{
	if (_selTab == TAB_PERSONNEL)
 	{
		// Personnel aren't worth anything
 		return 0;
 	}
 	else if (_selTab == TAB_CRAFT && _sel < _crafts.size())
	{
		// Craft
		Craft *craft = _crafts[_sel];
		return craft->getRules()->getSellCost();
	}

	// Item cost
	return _game->getRuleset()->getItem(_items[_selTab][getItemIndex(_sel)])->getSellCost();
}

/**
 * Gets the quantity of the currently selected item
 * on the base.
 * @return Quantity of selected item on the base.
 */
int SellState::getQuantity()
{
	// Crafts are individual
	if (_selTab == TAB_CRAFT && _sel < _crafts.size())
	{
		return 1;
 	}
 	// Personnel quantity
 	else if (_selTab == TAB_PERSONNEL)
 	{
		// Soldiers are individual
		if (_sel < _soldiers.size())
		{
			return 1;
		}
		// Scientist quantity
		else if (_base->getAvailableScientists() > 0 && _sel == _soldiers.size())
		{
			return _base->getAvailableScientists();
		}
		// Engineer quantity
		else
		{
			return _base->getAvailableEngineers();
		}
 	}
	// Item quantity
	else
 	{
		return _containers[_selTab]->getItem(_items[_selTab][getItemIndex(_sel)]);
 	}
}

/**
 * Increases the quantity of the selected item to sell by one.
 */
void SellState::increase()
{
	_timerDec->setInterval(50);
	_timerInc->setInterval(50);
	increaseByValue(1);
}

/**
 * Increases the quantity of the selected item to sell by "change".
 * @param change How much we want to add.
 */
void SellState::increaseByValue(int change)
{
	if (0 >= change || _quantities[_selTab][_sel] >= getQuantity()) return;

	change = std::min(getQuantity() - _quantities[_selTab][_sel], change);
	_quantities[_selTab][_sel] += change;

	if (_selTab != TAB_PERSONNEL && !(_selTab == TAB_CRAFT && _sel < _crafts.size()))
	{
		// Calculate the change in storage space
		_spaceChange +=  change * (int)(10 * _game->getRuleset()->getItem(_items[_selTab][getItemIndex(_sel)])->getSize());
	}
	_total += getPrice() * change;
	updateItemStrings();
}

/**
 * Decreases the quantity of the selected item to sell by one.
 */
void SellState::decrease()
{
	_timerInc->setInterval(50);
	_timerDec->setInterval(50);
	decreaseByValue(1);
}

/**
 * Decreases the quantity of the selected item to sell by "change".
 * @param change How much we want to remove.
 */
void SellState::decreaseByValue(int change)
{
	if (0 >= change || 0 >= _quantities[_selTab][_sel]) return;

	change = std::min(_quantities[_selTab][_sel], change);
	_quantities[_selTab][_sel] -= change;

	if (_selTab != TAB_PERSONNEL && !(_selTab == TAB_CRAFT && _sel < _crafts.size()))
	{
		// Calculate the change in storage space
		_spaceChange -=  change * (int)(10 * _game->getRuleset()->getItem(_items[_selTab][getItemIndex(_sel)])->getSize());
	}
	_total -= getPrice() * change;
	updateItemStrings();
}

/**
 * Updates the quantity-strings of the selected item.
 */
void SellState::updateItemStrings()
{
	_txtSales->setText(tr("STR_VALUE_OF_SALES").arg(Text::formatFunding(_total)));

	std::wostringstream ss, ss2, ss5;
	ss << _quantities[_selTab][_sel];
	_selList->setCellText(_sel, 2, ss.str());
	ss2 << getQuantity() - _quantities[_selTab][_sel];
	_selList->setCellText(_sel, 1, ss2.str());
	ss5 << _base->getExactUsedStores();
	if (_spaceChange != 0)
		ss5 << "(-" << std::fixed << std::setprecision(1) << ((float)_spaceChange/10) << ")";
	ss5 << ":" << _base->getAvailableStores();
	_txtSpaceUsed->setText(tr("STR_SPACE_USED").arg(ss5.str()));

	if (_quantities[_selTab][_sel] > 0)
	{
		_selList->setRowColor(_sel, _color2);
	}
	else if (_selTab == TAB_PERSONNEL || (_selTab == TAB_CRAFT && _sel < _crafts.size()))
	{
		_selList->setRowColor(_sel, _color);
	}
	else
	{
		RuleItem *rule = _game->getRuleset()->getItem(_items[_selTab][getItemIndex(_sel)]);
		if (rule->getClipSize() > 0 || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0))
		{
			_selList->setRowColor(_sel, _colorAmmo);
		}
		else
		{
			_selList->setRowColor(_sel, _color);
		}
	}

	if (_overfull)
	{
		_btnOk->setVisible(!_base->storesOverfull(-_spaceChange));
	}
}

/**
 * Gets the index of the selected item.
 * @param selected Currently selected item.
 * @return Index of the selected item.
 */
int SellState::getItemIndex(unsigned selected) const
{
	if (_selTab == TAB_CRAFT)
		return selected - _crafts.size();
	return selected;
}

}
