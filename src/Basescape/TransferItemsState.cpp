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
#include "TransferItemsState.h"
#include <sstream>
#include <climits>
#include <cmath>
#include "../aresame.h"
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
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Engine/Timer.h"
#include "../Savegame/Transfer.h"
#include "../Menu/ErrorMessageState.h"
#include "TransferConfirmState.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Transfer screen.
 * @param game Pointer to the core game.
 * @param baseFrom Pointer to the source base.
 * @param baseTo Pointer to the destination base.
 */
TransferItemsState::TransferItemsState(Game *game, Base *baseFrom, Base *baseTo) : State(game), _baseFrom(baseFrom), _baseTo(baseTo), _qtys(), _soldiers(), _crafts(), _items(), _sel(0), _total(0), _pQty(0), _cQty(0), _aQty(0), _iQty(0.0f), _hasSci(0), _hasEng(0), _distance(0.0)
{
	_changeValueByMouseWheel = Options::getInt("changeValueByMouseWheel");
	_allowChangeListValuesByMouseWheel = (Options::getBool("allowChangeListValuesByMouseWheel") && _changeValueByMouseWheel);
	_containmentLimit = Options::getBool("alienContainmentLimitEnforced");
	_canTransferCraftsWhileAirborne = Options::getBool("canTransferCraftsWhileAirborne");

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtItem = new Text(130, 9, 10, 22);
	_txtQuantity = new Text(50, 9, 150, 22);
	_txtAmountTransfer = new Text(60, 16, 200, 22);
	_txtAmountDestination = new Text(60, 16, 260, 22);
	_lstItems = new TextList(287, 128, 8, 40);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtAmountTransfer);
	add(_txtAmountDestination);
	add(_lstItems);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+6);
	_btnOk->setText(_game->getLanguage()->getString("STR_TRANSFER"));
	_btnOk->onMouseClick((ActionHandler)&TransferItemsState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&TransferItemsState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(15)+6);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&TransferItemsState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&TransferItemsState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_TRANSFER"));

	_txtItem->setColor(Palette::blockOffset(13)+10);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtQuantity->setColor(Palette::blockOffset(13)+10);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_txtAmountTransfer->setColor(Palette::blockOffset(13)+10);
	_txtAmountTransfer->setText(_game->getLanguage()->getString("STR_AMOUNT_TO_TRANSFER"));

	_txtAmountDestination->setColor(Palette::blockOffset(13)+10);
	_txtAmountDestination->setText(_game->getLanguage()->getString("STR_AMOUNT_AT_DESTINATION"));

	_lstItems->setColor(Palette::blockOffset(15)+1);
	_lstItems->setArrowColor(Palette::blockOffset(13)+10);
	_lstItems->setArrowColumn(193, ARROW_VERTICAL);
	_lstItems->setColumns(4, 162, 58, 40, 20);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);
	_lstItems->setAllowScrollOnArrowButtons(!_allowChangeListValuesByMouseWheel);
	_lstItems->onLeftArrowPress((ActionHandler)&TransferItemsState::lstItemsLeftArrowPress);
	_lstItems->onLeftArrowRelease((ActionHandler)&TransferItemsState::lstItemsLeftArrowRelease);
	_lstItems->onLeftArrowClick((ActionHandler)&TransferItemsState::lstItemsLeftArrowClick);
	_lstItems->onRightArrowPress((ActionHandler)&TransferItemsState::lstItemsRightArrowPress);
	_lstItems->onRightArrowRelease((ActionHandler)&TransferItemsState::lstItemsRightArrowRelease);
	_lstItems->onRightArrowClick((ActionHandler)&TransferItemsState::lstItemsRightArrowClick);
	_lstItems->onMousePress((ActionHandler)&TransferItemsState::lstItemsMousePress);

	for (std::vector<Soldier*>::iterator i = _baseFrom->getSoldiers()->begin(); i != _baseFrom->getSoldiers()->end(); ++i)
	{
		if ((*i)->getCraft() == 0)
		{
			_qtys.push_back(0);
			_soldiers.push_back(*i);
			_lstItems->addRow(4, (*i)->getName().c_str(), L"1", L"0", L"0");
		}
	}
	for (std::vector<Craft*>::iterator i = _baseFrom->getCrafts()->begin(); i != _baseFrom->getCrafts()->end(); ++i)
	{
		if ((*i)->getStatus() != "STR_OUT" || (_canTransferCraftsWhileAirborne && (*i)->getFuel() >= (*i)->getFuelLimit(_baseTo)))
		{
			_qtys.push_back(0);
			_crafts.push_back(*i);
			_lstItems->addRow(4, (*i)->getName(_game->getLanguage()).c_str(), L"1", L"0", L"0");
		}
	}
	if (_baseFrom->getAvailableScientists() > 0)
	{
		_qtys.push_back(0);
		_hasSci = 1;
		std::wstringstream ss, ss2;
		ss << _baseFrom->getAvailableScientists();
		ss2 << _baseTo->getAvailableScientists();
		_lstItems->addRow(4, _game->getLanguage()->getString("STR_SCIENTIST").c_str(), ss.str().c_str(), L"0", ss2.str().c_str());
	}
	if (_baseFrom->getAvailableEngineers() > 0)
	{
		_qtys.push_back(0);
		_hasEng = 1;
		std::wstringstream ss, ss2;
		ss << _baseFrom->getAvailableEngineers();
		ss2 << _baseTo->getAvailableEngineers();
		_lstItems->addRow(4, _game->getLanguage()->getString("STR_ENGINEER").c_str(), ss.str().c_str(), L"0", ss2.str().c_str());
	}
	const std::vector<std::string> &items = _game->getRuleset()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		int qty = _baseFrom->getItems()->getItem(*i);
		if (qty > 0)
		{
			_qtys.push_back(0);
			_items.push_back(*i);
			std::wstringstream ss, ss2;
			ss << qty;
			ss2 << _baseTo->getItems()->getItem(*i);
			_lstItems->addRow(4, _game->getLanguage()->getString(*i).c_str(), ss.str().c_str(), L"0", ss2.str().c_str());
		}
	}
	_distance = getDistance();

	_timerInc = new Timer(250);
	_timerInc->onTimer((StateHandler)&TransferItemsState::increase);
	_timerDec = new Timer(250);
	_timerDec->onTimer((StateHandler)&TransferItemsState::decrease);
}

/**
 *
 */
TransferItemsState::~TransferItemsState()
{
	delete _timerInc;
	delete _timerDec;
}

/**
 * Resets the palette since it's bound to change on other screens.
 */
void TransferItemsState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
}

/**
 * Runs the arrow timers.
 */
void TransferItemsState::think()
{
	State::think();

	_timerInc->think(this, 0);
	_timerDec->think(this, 0);
}

/**
 * Transfers the selected items.
 * @param action Pointer to an action.
 */
void TransferItemsState::btnOkClick(Action *)
{
	_game->pushState(new TransferConfirmState(_game, _baseTo, this));
}

/**
 * Completes the transfer between bases.
 */
void TransferItemsState::completeTransfer()
{
	int time = (int)floor(6 + _distance / 10.0);
	_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() - _total);
	for (unsigned int i = 0; i < _qtys.size(); ++i)
	{
		if (_qtys[i] > 0)
		{
			// Transfer soldiers
			if (i < _soldiers.size())
			{
				for (std::vector<Soldier*>::iterator s = _baseFrom->getSoldiers()->begin(); s != _baseFrom->getSoldiers()->end(); ++s)
				{
					if (*s == _soldiers[i])
					{
						 if((*s)->isInPsiTraining())
						 {
							 (*s)->setPsiTraining();
						 }
						Transfer *t = new Transfer(time);
						t->setSoldier(*s);
						_baseTo->getTransfers()->push_back(t);
						_baseFrom->getSoldiers()->erase(s);
						break;
					}
				}
			}
			// Transfer crafts
			else if (i >= _soldiers.size() && i < _soldiers.size() + _crafts.size())
			{
				Craft *craft =  _crafts[i - _soldiers.size()];

				// Transfer soldiers inside craft
				for (std::vector<Soldier*>::iterator s = _baseFrom->getSoldiers()->begin(); s != _baseFrom->getSoldiers()->end();)
				{
					if ((*s)->getCraft() == craft)
					{
						if ((*s)->isInPsiTraining()) (*s)->setPsiTraining();
						if (craft->getStatus() == "STR_OUT") _baseTo->getSoldiers()->push_back(*s);
						else
						{
							Transfer *t = new Transfer(time);
							t->setSoldier(*s);
							_baseTo->getTransfers()->push_back(t);
						}
						s = _baseFrom->getSoldiers()->erase(s);
					}
					else
					{
						++s;
					}
				}

				// Transfer craft
				for (std::vector<Craft*>::iterator c = _baseFrom->getCrafts()->begin(); c != _baseFrom->getCrafts()->end(); ++c)
				{
					if (*c == craft)
					{
						if (craft->getStatus() == "STR_OUT")
						{
							bool returning = (craft->getDestination() == (Target*)craft->getBase());
							_baseTo->getCrafts()->push_back(craft);
							craft->setBaseOnly(_baseTo);
							if (craft->getFuel() <= craft->getFuelLimit(_baseTo))
							{
								craft->setLowFuel(true);
								craft->returnToBase();
							}
							else if (returning)
							{
								craft->setLowFuel(false);
								craft->returnToBase();
							}
						}
						else
						{
							Transfer *t = new Transfer(time);
							t->setCraft(*c);
							_baseTo->getTransfers()->push_back(t);
						}
						// Clear Hangar
						for (std::vector<BaseFacility*>::iterator f = _baseFrom->getFacilities()->begin(); f != _baseFrom->getFacilities()->end(); ++f)
						{
							if ((*f)->getCraft() == *c)
							{
								(*f)->setCraft(0);
								break;
							}
						}

						_baseFrom->getCrafts()->erase(c);
						break;
					}
				}
			}
			// Transfer scientists
			else if (_baseFrom->getAvailableScientists() > 0 && i == _soldiers.size() + _crafts.size())
			{
				_baseFrom->setScientists(_baseFrom->getScientists() - _qtys[i]);
				Transfer *t = new Transfer(time);
				t->setScientists(_qtys[i]);
				_baseTo->getTransfers()->push_back(t);
			}
			// Transfer engineers
			else if (_baseFrom->getAvailableEngineers() > 0 && i == _soldiers.size() + _crafts.size() + _hasSci)
			{
				_baseFrom->setEngineers(_baseFrom->getEngineers() - _qtys[i]);
				Transfer *t = new Transfer(time);
				t->setEngineers(_qtys[i]);
				_baseTo->getTransfers()->push_back(t);
			}
			// Transfer items
			else
			{
				_baseFrom->getItems()->removeItem(_items[ getItemIndex(i) ], _qtys[i]);
				Transfer *t = new Transfer(time);
				t->setItems(_items[getItemIndex(i)], _qtys[i]);
				_baseTo->getTransfers()->push_back(t);
			}
		}
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void TransferItemsState::btnCancelClick(Action *)
{
	_game->popState();
	_game->popState();
}

/**
 * Starts increasing the item.
 * @param action Pointer to an action.
 */
void TransferItemsState::lstItemsLeftArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && !_timerInc->isRunning()) _timerInc->start();
}

/**
 * Stops increasing the item.
 * @param action Pointer to an action.
 */
void TransferItemsState::lstItemsLeftArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerInc->stop();
	}
}

/**
 * Increases the selected item;
 * by one on left-click; to max on right-click.
 * @param action Pointer to an action.
 */
void TransferItemsState::lstItemsLeftArrowClick(Action *action)
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
void TransferItemsState::lstItemsRightArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && !_timerDec->isRunning()) _timerDec->start();
}

/**
 * Stops decreasing the item.
 * @param action Pointer to an action.
 */
void TransferItemsState::lstItemsRightArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerDec->stop();
	}
}

/**
 * Decreases the selected item;
 * by one on left-click; to 0 on right-click.
 * @param action Pointer to an action.
 */
void TransferItemsState::lstItemsRightArrowClick(Action *action)
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
void TransferItemsState::lstItemsMousePress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP)
	{
		_timerInc->stop();
		_timerDec->stop();
		if (_allowChangeListValuesByMouseWheel
			&& action->getAbsoluteXMouse() >= _lstItems->getArrowsLeftEdge()
			&& action->getAbsoluteXMouse() <= _lstItems->getArrowsRightEdge())
		{
			increaseByValue(_changeValueByMouseWheel);
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN)
	{
		_timerInc->stop();
		_timerDec->stop();
		if (_allowChangeListValuesByMouseWheel
			&& action->getAbsoluteXMouse() >= _lstItems->getArrowsLeftEdge()
			&& action->getAbsoluteXMouse() <= _lstItems->getArrowsRightEdge())
		{
			decreaseByValue(_changeValueByMouseWheel);
		}
	}
}

/**
 * Gets the transfer cost of the currently selected item.
 * @return Transfer cost.
 */
int TransferItemsState::getCost() const
{
	int cost = 0;
	// Item cost
	if (TRANSFER_ITEM == getType(_sel))
	{
		cost = 1;
	}
	// Craft cost
	else if (TRANSFER_CRAFT == getType(_sel))
	{
		cost = 25;
	}
	// Personnel cost
	else
	{
		cost = 5;
	}
	return (int)floor(_distance * cost);
}

/**
 * Gets the quantity of the currently selected item
 * on the base.
 * @return Item quantity.
 */
int TransferItemsState::getQuantity() const
{
	switch(getType(_sel))
	{
	case TRANSFER_SOLDIER:
	case TRANSFER_CRAFT:
		return 1;
	case TRANSFER_SCIENTIST:
		return _baseFrom->getAvailableScientists();
	case TRANSFER_ENGINEER:
		return _baseFrom->getAvailableEngineers();
	case TRANSFER_ITEM:
		return _baseFrom->getItems()->getItem(_items[getItemIndex(_sel)]);
	}
	return 1;
}

/**
 * Increases the quantity of the selected item to transfer by one.
 */
void TransferItemsState::increase()
{
	_timerDec->setInterval(50);
	_timerInc->setInterval(50);
	increaseByValue(1);
}

/**
 * Increases the quantity of the selected item to transfer by "change".
 * @param change How much we want to add.
 */
void TransferItemsState::increaseByValue(int change)
{
	const enum TransferType selType = getType(_sel);
	RuleItem * selItem = NULL;

	if (TRANSFER_ITEM == selType)
		selItem = _game->getRuleset()->getItem(_items[getItemIndex(_sel)]);

	if (0 >= change || getQuantity() <= _qtys[_sel]) return;
	// Check Living Quarters
	if ((TRANSFER_SOLDIER == selType || TRANSFER_SCIENTIST == selType || TRANSFER_ENGINEER == selType)
		&& _pQty + 1 > _baseTo->getAvailableQuarters() - _baseTo->getUsedQuarters())
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NO_FREE_ACCOMODATION", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
		return;
	}
	if (TRANSFER_CRAFT == selType )
	{
		Craft *craft =  _crafts[_sel - _soldiers.size()];
		if (_cQty + 1 > _baseTo->getAvailableHangars() - _baseTo->getUsedHangars())
		{
			_timerInc->stop();
			_game->pushState(new ErrorMessageState(_game, "STR_NO_FREE_HANGARS_FOR_TRANSFER", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
			return;
		}
		if (_pQty + craft->getNumSoldiers() > _baseTo->getAvailableQuarters() - _baseTo->getUsedQuarters())
		{
			_timerInc->stop();
			_game->pushState(new ErrorMessageState(_game, "STR_NO_FREE_ACCOMODATION_CREW", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
			return;
		}
	}
	if (TRANSFER_ITEM == selType && !selItem->getAlien()
		&& (_iQty + selItem->getSize()) > (_baseTo->getAvailableStores() - _baseTo->getUsedStores()))
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NOT_ENOUGH_STORE_SPACE", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
		return;
	}
	if (TRANSFER_ITEM == selType && selItem->getAlien()
		&& _containmentLimit * _aQty + 1 > _baseTo->getAvailableContainment() - _containmentLimit * _baseTo->getUsedContainment())
	{
		_timerInc->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NO_ALIEN_CONTAINMENT_FOR_TRANSFER", Palette::blockOffset(15)+1, "BACK13.SCR", 0));
		return;
	}

	// Personnel count
	if (TRANSFER_SOLDIER == selType || TRANSFER_SCIENTIST == selType || TRANSFER_ENGINEER == selType)
	{
		int freeQuarters = _baseTo->getAvailableQuarters() - _baseTo->getUsedQuarters() - _pQty;
		change = std::min(std::min(freeQuarters, getQuantity() - _qtys[_sel]), change);
		_pQty += change;
		_qtys[_sel] += change;
		_total += getCost() * change;
	}
	// Craft count
	else if (TRANSFER_CRAFT == selType)
	{
		Craft *craft = _crafts[_sel - _soldiers.size()];
		_cQty++;
		_pQty += craft->getNumSoldiers();
		_qtys[_sel]++;
		if (!_canTransferCraftsWhileAirborne || craft->getStatus() != "STR_OUT") _total += getCost();
	}
	// Item count
	else if (TRANSFER_ITEM == selType && !selItem->getAlien() )
	{
		float storesNeededPerItem = _game->getRuleset()->getItem(_items[getItemIndex(_sel)])->getSize();
		float freeStores = (float)(_baseTo->getAvailableStores() - _baseTo->getUsedStores()) - _iQty;
		int freeStoresForItem;
		if ( AreSame(storesNeededPerItem, 0.f) ) { 
			freeStoresForItem = INT_MAX;
		}
		else
		{
			freeStoresForItem = floor(freeStores / storesNeededPerItem);
		}
		change = std::min(std::min(freeStoresForItem, getQuantity() - _qtys[_sel]), change);
		_iQty += ((float)(change)) * storesNeededPerItem;
		_qtys[_sel] += change;
		_total += getCost() * change;
	}
	// Live Aliens count
	else if (TRANSFER_ITEM == selType && selItem->getAlien() )
	{
		int freeContainment = _containmentLimit ? _baseTo->getAvailableContainment() - _baseTo->getUsedContainment() - _aQty : INT_MAX;
		change = std::min(std::min(freeContainment, getQuantity() - _qtys[_sel]), change);
		_aQty += change;
		_qtys[_sel] += change;
		_total += getCost() * change;
	}
	updateItemStrings();
}

/**
 * Decreases the quantity of the selected item to transfer by one.
 */
void TransferItemsState::decrease()
{
	_timerInc->setInterval(50);
	_timerDec->setInterval(50);
	decreaseByValue(1);
}

/**
 * Decreases the quantity of the selected item to transfer by "change".
 * @param change How much we want to remove.
 */
void TransferItemsState::decreaseByValue(int change)
{
	const enum TransferType selType = getType(_sel);
	if (0 >= change || 0 >= _qtys[_sel]) return;
	Craft *craft = 0;
	change = std::min(_qtys[_sel], change);
	// Personnel count
	if (TRANSFER_SOLDIER == selType || TRANSFER_SCIENTIST == selType || TRANSFER_ENGINEER == selType)
	{
		_pQty -= change;
	}
	// Craft count
	else if (TRANSFER_CRAFT == selType)
	{
		craft = _crafts[_sel - _soldiers.size()];
		_cQty--;
		_pQty -= craft->getNumSoldiers();
	}
	// Item count
	else if (TRANSFER_ITEM == selType)
	{
		const RuleItem * selItem = _game->getRuleset()->getItem(_items[getItemIndex(_sel)]);
		if (!selItem->getAlien() )
		{
			_iQty -= selItem->getSize() * ((float)(change));
		}
		else
		{
			_aQty -= change;
		}
	}
	_qtys[_sel] -= change;
	if (!_canTransferCraftsWhileAirborne || 0 == craft || craft->getStatus() != "STR_OUT")
		_total -= getCost() * change;
	updateItemStrings();
}

/**
 * Updates the quantity-strings of the selected item.
 */
void TransferItemsState::updateItemStrings()
{
	std::wstringstream ss;
	ss << _qtys[_sel];
	_lstItems->setCellText(_sel, 2, ss.str());
}

/**
 * Gets the total cost of the current transfer.
 * @return Total cost.
 */
int TransferItemsState::getTotal() const
{
	return _total;
}

/**
 * Gets the shortest distance between the two bases.
 * @return Distance.
 */
double TransferItemsState::getDistance() const
{
	double x[3], y[3], z[3], r = 51.2;
	Base *base = _baseFrom;
	for (int i = 0; i < 2; ++i) {
		x[i] = r * cos(base->getLatitude()) * cos(base->getLongitude());
		y[i] = r * cos(base->getLatitude()) * sin(base->getLongitude());
		z[i] = r * -sin(base->getLatitude());
		base = _baseTo;
	}
	x[2] = x[1] - x[0];
	y[2] = y[1] - y[0];
	z[2] = z[1] - z[0];
	return sqrt(x[2] * x[2] + y[2] * y[2] + z[2] * z[2]);
}

/**
 * Gets type of selected item.
 * @param selected The selected item.
 * @return The type of the selected item.
 */
enum TransferType TransferItemsState::getType(unsigned selected) const
{
	unsigned max = _soldiers.size();

	if (selected < max)
		return TRANSFER_SOLDIER;
	if (selected < (max += _crafts.size()) )
		return TRANSFER_CRAFT;
	if (selected < (max += _hasSci))
		return TRANSFER_SCIENTIST;
	if (selected < (max += _hasEng))
		return TRANSFER_ENGINEER;

	return TRANSFER_ITEM;
}

/**
 * Gets the index of the selected item.
 * @param selected Currently selected item.
 * @return Index of the selected item.
 */
int TransferItemsState::getItemIndex(unsigned selected) const
{
	return selected - _soldiers.size() - _crafts.size() - _hasSci - _hasEng;
}

}
