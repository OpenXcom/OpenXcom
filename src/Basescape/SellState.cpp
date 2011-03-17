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
#include "SellState.h"
#include <sstream>
#include <cmath>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Savegame/ItemContainer.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Engine/Timer.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Sell/Sack screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
SellState::SellState(Game *game, Base *base) : State(game), _base(base), _qtys(), _soldiers(), _crafts(), _items(), _sel(0), _total(0), _sOffset(0), _eOffset(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtSales = new Text(190, 9, 10, 24);
	_txtFunds = new Text(114, 9, 200, 24);
	_txtItem = new Text(130, 9, 10, 32);
	_txtQuantity = new Text(44, 9, 140, 32);
	_txtSell = new Text(96, 9, 184, 32);
	_txtValue = new Text(34, 9, 280, 32);
	_lstItems = new TextList(288, 120, 8, 44);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtSales);
	add(_txtFunds);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtSell);
	add(_txtValue);
	add(_lstItems);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+13);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getLanguage()->getString("STR_SELL_SACK"));
	_btnOk->onMouseClick((ActionHandler)&SellState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(13)+13);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&SellState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELL_ITEMS_SACK_PERSONNEL"));
	
	std::wstring s1 = _game->getLanguage()->getString("STR_VALUE_OF_SALES");
	s1 += Text::formatFunding(_total);
	_txtSales->setColor(Palette::blockOffset(13)+10);
	_txtSales->setText(s1);

	std::wstring s2 = _game->getLanguage()->getString("STR_FUNDS");
	s2 += Text::formatFunding(_game->getSavedGame()->getFunds());
	_txtFunds->setColor(Palette::blockOffset(13)+10);
	_txtFunds->setText(s2);

	_txtItem->setColor(Palette::blockOffset(13)+10);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtQuantity->setColor(Palette::blockOffset(13)+10);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_txtSell->setColor(Palette::blockOffset(13)+10);
	_txtSell->setText(_game->getLanguage()->getString("STR_SELL_SACK"));

	_txtValue->setColor(Palette::blockOffset(13)+10);
	_txtValue->setText(_game->getLanguage()->getString("STR_VALUE"));

	_lstItems->setColor(Palette::blockOffset(13)+10);
	_lstItems->setArrowColor(Palette::blockOffset(13)+13);
	_lstItems->setArrowColumn(189);
	_lstItems->setColumns(4, 156, 62, 28, 40);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);
	_lstItems->onLeftArrowPress((ActionHandler)&SellState::lstItemsLeftArrowPress);
	_lstItems->onLeftArrowRelease((ActionHandler)&SellState::lstItemsLeftArrowRelease);
	_lstItems->onRightArrowPress((ActionHandler)&SellState::lstItemsRightArrowPress);
	_lstItems->onRightArrowRelease((ActionHandler)&SellState::lstItemsRightArrowRelease);

	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); i++)
	{
		if ((*i)->getCraft() == 0)
		{
			_qtys.push_back(0);
			_soldiers.push_back(*i);
			_lstItems->addRow(4, (*i)->getName().c_str(), L"1", L"0", Text::formatFunding(0).c_str());
		}
	}
	for (std::vector<Craft*>::iterator i = _base->getCrafts()->begin(); i != _base->getCrafts()->end(); i++)
	{
		if ((*i)->getStatus() != "STR_OUT")
		{
			_qtys.push_back(0);
			_crafts.push_back(*i);
			_lstItems->addRow(4, (*i)->getName(_game->getLanguage()).c_str(), L"1", L"0", Text::formatFunding(0).c_str());
		}
	}
	if (_base->getAvailableScientists() > 0)
	{
		_qtys.push_back(0);
		_sOffset++;
		std::wstringstream ss;
		ss << _base->getAvailableScientists();
		_lstItems->addRow(4, _game->getLanguage()->getString("STR_SCIENTIST").c_str(), ss.str().c_str(), L"0", Text::formatFunding(0).c_str());
	}
	if (_base->getAvailableEngineers() > 0)
	{
		_qtys.push_back(0);
		_eOffset++;
		std::wstringstream ss;
		ss << _base->getAvailableEngineers();
		_lstItems->addRow(4, _game->getLanguage()->getString("STR_ENGINEER").c_str(), ss.str().c_str(), L"0", Text::formatFunding(0).c_str());
	}
	for (std::map<std::string, int>::iterator i = _base->getItems()->getContents()->begin(); i != _base->getItems()->getContents()->end(); i++)
	{
		_qtys.push_back(0);
		_items.push_back(i->first);
		RuleItem *rule = _game->getRuleset()->getItem(i->first);
		std::wstringstream ss;
		ss << i->second;
		_lstItems->addRow(4, _game->getLanguage()->getString(i->first).c_str(), ss.str().c_str(), L"0", Text::formatFunding(rule->getCost() / 2).c_str());
	}

	_timerInc = new Timer(50);
	_timerInc->onTimer((StateHandler)&SellState::increase);
	_timerDec = new Timer(50);
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
 * Runs the game timer and handles popups.
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
void SellState::btnOkClick(Action *action)
{
	_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() + _total);
	for (unsigned int i = 0; i < _qtys.size(); i++)
	{
		if (_qtys[i] > 0)
		{
			// Sell soldiers
			if (i < _soldiers.size())
			{
				delete _soldiers[i];
				for (std::vector<Soldier*>::iterator s = _base->getSoldiers()->begin(); s != _base->getSoldiers()->end(); s++)
				{
					if (*s == _soldiers[i])
					{
						_base->getSoldiers()->erase(s);
						break;
					}
				}
			}
			// Sell crafts
			else if (i >= _soldiers.size() && i < _soldiers.size() + _crafts.size())
			{
				Craft *craft =  _crafts[i - _soldiers.size()];

				// Remove weapons from craft
				for (std::vector<CraftWeapon*>::iterator w = craft->getWeapons()->begin(); w != craft->getWeapons()->end(); w++)
				{
					if ((*w) != 0)
					{
						_base->getItems()->addItem((*w)->getRules()->getLauncherItem());
						_base->getItems()->addItem((*w)->getRules()->getClipItem(), (int)floor((double)(*w)->getAmmo() / (*w)->getRules()->getRearmRate()));
					}
				}

				// Remove items from craft
				for (std::map<std::string, int>::iterator it = craft->getItems()->getContents()->begin(); it != craft->getItems()->getContents()->end(); it++)
				{
					_base->getItems()->addItem(it->first, it->second);
				}

				// Remove soldiers from craft
				for (std::vector<Soldier*>::iterator s = _base->getSoldiers()->begin(); s != _base->getSoldiers()->end(); s++)
				{
					if ((*s)->getCraft() == craft)
					{
						(*s)->setCraft(0);
					}
				}

				// Remove craft
				delete craft;
				for (std::vector<Craft*>::iterator c = _base->getCrafts()->begin(); c != _base->getCrafts()->end(); c++)
				{
					if (*c == craft)
					{
						_base->getCrafts()->erase(c);
						break;
					}
				}
			}
			// Sell scientists
			else if (_base->getAvailableScientists() > 0 && i == _soldiers.size() + _crafts.size())
			{
				_base->setScientists(_base->getScientists() - _qtys[i]);
			}
			// Sell engineers
			else if (_base->getAvailableEngineers() > 0 && i == _soldiers.size() + _crafts.size() + _sOffset)
			{
				_base->setEngineers(_base->getEngineers() - _qtys[i]);
			}
			// Sell items
			else
			{
				_base->getItems()->removeItem(_items[i - _soldiers.size() - _crafts.size() - _sOffset - _eOffset], _qtys[i]);
			}
		}
	}
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SellState::btnCancelClick(Action *action)
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
	_timerInc->start();
}

/**
 * Stops increasing the item.
 * @param action Pointer to an action.
 */
void SellState::lstItemsLeftArrowRelease(Action *action)
{
	_timerInc->stop();
}

/**
 * Starts decreasing the item.
 * @param action Pointer to an action.
 */
void SellState::lstItemsRightArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	_timerDec->start();
}

/**
 * Stops decreasing the item.
 * @param action Pointer to an action.
 */
void SellState::lstItemsRightArrowRelease(Action *action)
{
	_timerDec->stop();	
}

/**
 * Gets the price of the currently selected item.
 */
int SellState::getPrice()
{
	// Personnel/craft aren't worth anything
	if (_sel < _soldiers.size() + _crafts.size() + _sOffset + _eOffset)
	{
		return 0;
	}
	// Item cost
	else
	{
		return _game->getRuleset()->getItem(_items[_sel - _soldiers.size() - _crafts.size() - _sOffset - _eOffset])->getCost() / 2;
	}
}

/**
 * Gets the quantity of the currently selected item
 * on the base.
 */
int SellState::getQuantity()
{
	// Soldiers/crafts are individual
	if (_sel < _soldiers.size() + _crafts.size())
	{
		return 1;
	}
	// Scientist quantity
	else if (_base->getAvailableScientists() > 0 && _sel == _soldiers.size() + _crafts.size())
	{
		return _base->getAvailableScientists();
	}
	// Engineer quantity
	else if (_base->getAvailableEngineers() > 0 && _sel == _soldiers.size() + _crafts.size() + _sOffset)
	{
		return _base->getAvailableEngineers();
	}
	// Item quantity
	else
	{
		return _base->getItems()->getItem(_items[_sel - _soldiers.size() - _crafts.size() - _sOffset - _eOffset]);
	}
}

/**
 * Increases the quantity of the selected item to sell.
 */
void SellState::increase()
{
	if (_qtys[_sel] < getQuantity())
	{
		_qtys[_sel]++;
		std::wstringstream ss;
		ss << _qtys[_sel];
		_lstItems->getCell(_sel, 2)->setText(ss.str());
		_lstItems->draw();
		_total += getPrice();
		std::wstring s = _game->getLanguage()->getString("STR_VALUE_OF_SALES");
		s += Text::formatFunding(_total);
		_txtSales->setText(s);
	}
}

/**
 * Decreases the quantity of the selected item to sell.
 */
void SellState::decrease()
{
	if (_qtys[_sel] > 0)
	{
		_qtys[_sel]--;
		std::wstringstream ss;
		ss << _qtys[_sel];
		_lstItems->getCell(_sel, 2)->setText(ss.str());
		_lstItems->draw();
		_total -= getPrice();
		std::wstring s = _game->getLanguage()->getString("STR_VALUE_OF_SALES");
		s += Text::formatFunding(_total);
		_txtSales->setText(s);
	}
}

}
