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
#ifndef OPENXCOM_TRANSFERITEMSSTATE_H
#define OPENXCOM_TRANSFERITEMSSTATE_H

#include "../Engine/State.h"
#include "../Savegame/Transfer.h"
#include <vector>
#include <string>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Timer;
class Base;
class Soldier;
class Craft;

/**
 * Transfer screen that lets the player pick
 * what items to transfer between bases.
 */
class TransferItemsState : public State
{
private:
	Base *_baseFrom, *_baseTo;
	TextButton *_btnOk, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtQuantity, *_txtAmountTransfer, *_txtAmountDestination;
	TextList *_lstItems;
	std::vector<int> _qtys;
	std::vector<Soldier*> _soldiers;
	std::vector<Craft*> _crafts;
	std::vector<std::string> _items;
	unsigned int _sel;
	int _total, _pQty, _cQty, _aQty;
	float _iQty;
    int _hasSci, _hasEng;
	double _distance;
	Timer *_timerInc, *_timerDec;
	int _changeValueByMouseWheel;
	bool _canTransferCraftsWhileAirborne, _containmentLimit, _allowChangeListValuesByMouseWheel;
	/// Gets selected cost.
	int getCost() const;
	/// Gets selected quantity.
	int getQuantity() const;
	/// Gets distance between bases.
	double getDistance() const;
	/// Gets type of selected item.
	enum TransferType getType(unsigned selected) const;
	/// Gets item Index.
	int getItemIndex(unsigned selected) const;
public:
	/// Creates the Transfer Items state.
	TransferItemsState(Game *game, Base *baseFrom, Base *baseTo);
	/// Cleans up the Transfer Items state.
	~TransferItemsState();
	/// Updates the palette.
	void init();
	/// Runs the timers.
	void think();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Completes the transfer between bases.
	void completeTransfer();
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for pressing an Increase arrow in the list.
	void lstItemsLeftArrowPress(Action *action);
	/// Handler for releasing an Increase arrow in the list.
	void lstItemsLeftArrowRelease(Action *action);
	/// Handler for clicking an Increase arrow in the list.
	void lstItemsLeftArrowClick(Action *action);
	/// Handler for pressing a Decrease arrow in the list.
	void lstItemsRightArrowPress(Action *action);
	/// Handler for releasing a Decrease arrow in the list.
	void lstItemsRightArrowRelease(Action *action);
	/// Handler for clicking a Decrease arrow in the list.
	void lstItemsRightArrowClick(Action *action);
	/// Handler for pressing-down a mouse-button in the list.
	void lstItemsMousePress(Action *action);
	/// Increases the quantity of an item by one.
	void increase();
	/// Increases the quantity of an item by the given value.
	void increaseByValue(int change);
	/// Decreases the quantity of an item by one.
	void decrease();
	/// Decreases the quantity of an item by the given value.
	void decreaseByValue(int change);
	/// Updates the quantity-strings of the selected item.
	void updateItemStrings();
	/// Gets the total of the transfer.
	int getTotal() const;
};

}

#endif
