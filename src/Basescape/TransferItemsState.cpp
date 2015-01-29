/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include <cfloat>
#include <cmath>
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
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Engine/Timer.h"
#include "../Menu/ErrorMessageState.h"
#include "TransferConfirmState.h"
#include "../Engine/Options.h"
#include "../fmath.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Transfer screen.
 * @param game Pointer to the core game.
 * @param baseFrom Pointer to the source base.
 * @param baseTo Pointer to the destination base.
 */
TransferItemsState::TransferItemsState(Base *baseFrom, Base *baseTo) : _baseFrom(baseFrom), _baseTo(baseTo), _sel(0), _itemOffset(0), _total(0), _pQty(0), _cQty(0), _aQty(0), _iQty(0.0), _hasSci(0), _hasEng(0), _distance(0.0), _ammoColor(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtItem = new Text(130, 9, 10, 24);
	_txtQuantity = new Text(50, 9, 150, 24);
	_txtAmountTransfer = new Text(60, 17, 200, 24);
	_txtAmountDestination = new Text(60, 17, 260, 24);
	_lstItems = new TextList(287, 120, 8, 44);

	// Set palette
	setPalette("PAL_BASESCAPE", _game->getRuleset()->getInterface("transferMenu")->getElement("palette")->color);
	_ammoColor = _game->getRuleset()->getInterface("transferMenu")->getElement("ammoColor")->color;

	add(_window, "window", "transferMenu");
	add(_btnOk, "button", "transferMenu");
	add(_btnCancel, "button", "transferMenu");
	add(_txtTitle, "text", "transferMenu");
	add(_txtItem, "text", "transferMenu");
	add(_txtQuantity, "text", "transferMenu");
	add(_txtAmountTransfer, "text", "transferMenu");
	add(_txtAmountDestination, "text", "transferMenu");
	add(_lstItems, "list", "transferMenu");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setText(tr("STR_TRANSFER"));
	_btnOk->onMouseClick((ActionHandler)&TransferItemsState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&TransferItemsState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&TransferItemsState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&TransferItemsState::btnCancelClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_TRANSFER"));

	_txtItem->setText(tr("STR_ITEM"));

	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_txtAmountTransfer->setText(tr("STR_AMOUNT_TO_TRANSFER"));
	_txtAmountTransfer->setWordWrap(true);

	_txtAmountDestination->setText(tr("STR_AMOUNT_AT_DESTINATION"));
	_txtAmountDestination->setWordWrap(true);

	_lstItems->setArrowColumn(193, ARROW_VERTICAL);
	_lstItems->setColumns(4, 162, 58, 40, 20);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);
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
			_baseQty.push_back(1);
			_transferQty.push_back(0);
			_soldiers.push_back(*i);
			_lstItems->addRow(4, (*i)->getName().c_str(), L"1", L"0", L"0");
			++_itemOffset;
		}
	}
	for (std::vector<Craft*>::iterator i = _baseFrom->getCrafts()->begin(); i != _baseFrom->getCrafts()->end(); ++i)
	{
		if ((*i)->getStatus() != "STR_OUT" || (Options::canTransferCraftsWhileAirborne && (*i)->getFuel() >= (*i)->getFuelLimit(_baseTo)))
		{
			_baseQty.push_back(1);
			_transferQty.push_back(0);
			_crafts.push_back(*i);
			_lstItems->addRow(4, (*i)->getName(_game->getLanguage()).c_str(), L"1", L"0", L"0");
			++_itemOffset;
		}
	}
	if (_baseFrom->getAvailableScientists() > 0)
	{
		_baseQty.push_back(_baseFrom->getAvailableScientists());
		_transferQty.push_back(0);
		_hasSci = 1;
		std::wostringstream ss, ss2;
		ss << _baseQty.back();
		ss2 << _baseTo->getAvailableScientists();
		_lstItems->addRow(4, tr("STR_SCIENTIST").c_str(), ss.str().c_str(), L"0", ss2.str().c_str());
		++_itemOffset;
	}
	if (_baseFrom->getAvailableEngineers() > 0)
	{
		_baseQty.push_back(_baseFrom->getAvailableEngineers());
		_transferQty.push_back(0);
		_hasEng = 1;
		std::wostringstream ss, ss2;
		ss << _baseQty.back();
		ss2 << _baseTo->getAvailableEngineers();
		_lstItems->addRow(4, tr("STR_ENGINEER").c_str(), ss.str().c_str(), L"0", ss2.str().c_str());
		++_itemOffset;
	}
	const std::vector<std::string> &items = _game->getRuleset()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		int qty = _baseFrom->getItems()->getItem(*i);
		if (qty > 0)
		{
			_baseQty.push_back(qty);
			_transferQty.push_back(0);
			_items.push_back(*i);
			RuleItem *rule = _game->getRuleset()->getItem(*i);
			std::wostringstream ss, ss2;
			ss << qty;
			ss2 << _baseTo->getItems()->getItem(*i);
			std::wstring item = tr(*i);
			if (rule->getBattleType() == BT_AMMO || (rule->getBattleType() == BT_NONE && rule->getClipSize() > 0))
			{
				item.insert(0, L"  ");
				_lstItems->addRow(4, item.c_str(), ss.str().c_str(), L"0", ss2.str().c_str());
				_lstItems->setRowColor(_baseQty.size() - 1, _ammoColor);
			}
			else
			{
				_lstItems->addRow(4, item.c_str(), ss.str().c_str(), L"0", ss2.str().c_str());
			}
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
	_game->pushState(new TransferConfirmState(_baseTo, this));
}

/**
 * Completes the transfer between bases.
 */
void TransferItemsState::completeTransfer()
{
	int time = (int)floor(6 + _distance / 10.0);
	_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() - _total);
	for (size_t i = 0; i < _transferQty.size(); ++i)
	{
		if (_transferQty[i] > 0)
		{
			// Transfer soldiers
			if (i < _soldiers.size())
			{
				for (std::vector<Soldier*>::iterator s = _baseFrom->getSoldiers()->begin(); s != _baseFrom->getSoldiers()->end(); ++s)
				{
					if (*s == _soldiers[i])
					{
						 if ((*s)->isInPsiTraining())
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
							craft->setBase(_baseTo, false);
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
				_baseFrom->setScientists(_baseFrom->getScientists() - _transferQty[i]);
				Transfer *t = new Transfer(time);
				t->setScientists(_transferQty[i]);
				_baseTo->getTransfers()->push_back(t);
			}
			// Transfer engineers
			else if (_baseFrom->getAvailableEngineers() > 0 && i == _soldiers.size() + _crafts.size() + _hasSci)
			{
				_baseFrom->setEngineers(_baseFrom->getEngineers() - _transferQty[i]);
				Transfer *t = new Transfer(time);
				t->setEngineers(_transferQty[i]);
				_baseTo->getTransfers()->push_back(t);
			}
			// Transfer items
			else
			{
				_baseFrom->getItems()->removeItem(_items[ getItemIndex(i) ], _transferQty[i]);
				Transfer *t = new Transfer(time);
				t->setItems(_items[getItemIndex(i)], _transferQty[i]);
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

	if (0 >= change || getQuantity() <= _transferQty[_sel]) return;
	std::wstring errorMessage = L"";
	// Check Living Quarters
	if ((TRANSFER_SOLDIER == selType || TRANSFER_SCIENTIST == selType || TRANSFER_ENGINEER == selType)
		&& _pQty + 1 > _baseTo->getAvailableQuarters() - _baseTo->getUsedQuarters())
	{
		_timerInc->stop();
		errorMessage = tr("STR_NO_FREE_ACCOMODATION");
	}
	else if (TRANSFER_CRAFT == selType )
	{
		Craft *craft =  _crafts[_sel - _soldiers.size()];
		if (_cQty + 1 > _baseTo->getAvailableHangars() - _baseTo->getUsedHangars())
		{
			_timerInc->stop();
			errorMessage = tr("STR_NO_FREE_HANGARS_FOR_TRANSFER");
		}
		else if (_pQty + craft->getNumSoldiers() > _baseTo->getAvailableQuarters() - _baseTo->getUsedQuarters())
		{
			_timerInc->stop();
			errorMessage = tr("STR_NO_FREE_ACCOMODATION_CREW");
		}
		else if (Options::storageLimitsEnforced && _baseTo->storesOverfull(_iQty + craft->getItems()->getTotalSize(_game->getRuleset())))
		{
			_timerInc->stop();
			errorMessage = tr("STR_NOT_ENOUGH_STORE_SPACE_FOR_CRAFT");
		}
	}
	else if (TRANSFER_ITEM == selType && !selItem->getAlien()
		&& _baseTo->storesOverfull(selItem->getSize() + _iQty))
	{
		_timerInc->stop();
		errorMessage = tr("STR_NOT_ENOUGH_STORE_SPACE");
	}
	else if (TRANSFER_ITEM == selType && selItem->getAlien()
		&& Options::storageLimitsEnforced * _aQty + 1 > _baseTo->getAvailableContainment() - Options::storageLimitsEnforced * _baseTo->getUsedContainment())
	{
		_timerInc->stop();
		errorMessage = tr("STR_NO_ALIEN_CONTAINMENT_FOR_TRANSFER");
	}

	if (errorMessage != L"")
	{
		RuleInterface *menuInterface = _game->getRuleset()->getInterface("transferMenu");
		_game->pushState(new ErrorMessageState(errorMessage, _palette, menuInterface->getElement("errorMessage")->color, "BACK13.SCR", menuInterface->getElement("errorPalette")->color));
		return;
	}

	// Personnel count
	if (TRANSFER_SOLDIER == selType || TRANSFER_SCIENTIST == selType || TRANSFER_ENGINEER == selType)
	{
		int freeQuarters = _baseTo->getAvailableQuarters() - _baseTo->getUsedQuarters() - _pQty;
		change = std::min(std::min(freeQuarters, getQuantity() - _transferQty[_sel]), change);
		_pQty += change;
		_baseQty[_sel] -= change;
		_transferQty[_sel] += change;
		_total += getCost() * change;
	}
	// Craft count
	else if (TRANSFER_CRAFT == selType)
	{
		Craft *craft = _crafts[_sel - _soldiers.size()];
		_cQty++;
		_pQty += craft->getNumSoldiers();
		_iQty += craft->getItems()->getTotalSize(_game->getRuleset());
		_baseQty[_sel]--;
		_transferQty[_sel]++;
		if (!Options::canTransferCraftsWhileAirborne || craft->getStatus() != "STR_OUT") _total += getCost();
	}
	// Item count
	else if (TRANSFER_ITEM == selType && !selItem->getAlien() )
	{
		double storesNeededPerItem = _game->getRuleset()->getItem(_items[getItemIndex(_sel)])->getSize();
		double freeStores = _baseTo->getAvailableStores() - _baseTo->getUsedStores() - _iQty;
		double freeStoresForItem = DBL_MAX;
		if (!AreSame(storesNeededPerItem, 0.0))
		{
			freeStoresForItem = (freeStores + 0.05) / storesNeededPerItem;
		}
		change = std::min(std::min((int)freeStoresForItem, getQuantity() - _transferQty[_sel]), change);
		_iQty += change * storesNeededPerItem;
		_baseQty[_sel] -= change;
		_transferQty[_sel] += change;
		_total += getCost() * change;
	}
	// Live Aliens count
	else if (TRANSFER_ITEM == selType && selItem->getAlien() )
	{
		int freeContainment = Options::storageLimitsEnforced ? _baseTo->getAvailableContainment() - _baseTo->getUsedContainment() - _aQty : INT_MAX;
		change = std::min(std::min(freeContainment, getQuantity() - _transferQty[_sel]), change);
		_aQty += change;
		_baseQty[_sel] -= change;
		_transferQty[_sel] += change;
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
	if (0 >= change || 0 >= _transferQty[_sel]) return;
	Craft *craft = 0;
	change = std::min(_transferQty[_sel], change);
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
		_iQty -= craft->getItems()->getTotalSize(_game->getRuleset());
	}
	// Item count
	else if (TRANSFER_ITEM == selType)
	{
		const RuleItem * selItem = _game->getRuleset()->getItem(_items[getItemIndex(_sel)]);
		if (!selItem->getAlien())
		{
			_iQty -= selItem->getSize() * change;
		}
		else
		{
			_aQty -= change;
		}
	}
	_baseQty[_sel] += change;
	_transferQty[_sel] -= change;
	if (!Options::canTransferCraftsWhileAirborne || 0 == craft || craft->getStatus() != "STR_OUT")
		_total -= getCost() * change;
	updateItemStrings();
}

/**
 * Updates the quantity-strings of the selected item.
 */
void TransferItemsState::updateItemStrings()
{
	std::wostringstream ss1, ss2;
	ss1 << _baseQty[_sel];
	ss2 << _transferQty[_sel];
	_lstItems->setCellText(_sel, 1, ss1.str());
	_lstItems->setCellText(_sel, 2, ss2.str());
	if (_transferQty[_sel] > 0)
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
enum TransferType TransferItemsState::getType(size_t selected) const
{
	size_t max = _soldiers.size();

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
size_t TransferItemsState::getItemIndex(size_t selected) const
{
	return selected - _soldiers.size() - _crafts.size() - _hasSci - _hasEng;
}

}
