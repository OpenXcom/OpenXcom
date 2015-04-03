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
#ifndef OPENXCOM_SELLSTATE_H
#define OPENXCOM_SELLSTATE_H

#include "../Engine/State.h"
#include "../Menu/OptionsBaseState.h"
#include <vector>
#include <string>

namespace OpenXcom
{
enum SellType { SELL_SOLDIER, SELL_CRAFT, SELL_ITEM, SELL_SCIENTIST, SELL_ENGINEER };

class TextButton;
class Window;
class Text;
class TextList;
class Timer;
class Base;
class Soldier;
class Craft;

/**
 * Sell/Sack screen that lets the player sell
 * any items in a particular base.
 */
class SellState : public State
{
private:
	Base *_base;
	TextButton *_btnOk, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtSales, *_txtFunds, *_txtItem, *_txtQuantity, *_txtSell, *_txtValue, *_txtSpaceUsed;
	TextList *_lstItems;
	std::vector<int> _qtys;
	std::vector<Soldier*> _soldiers;
	std::vector<Craft*> _crafts;
	std::vector<std::string> _items;
	size_t _sel, _itemOffset;
	int _total, _hasSci, _hasEng;
	double _spaceChange;
	Timer *_timerInc, *_timerDec;
	Uint8 _ammoColor;
	OptionsOrigin _origin;
	/// Gets selected price.
	int getPrice();
	/// Gets selected quantity.
	int getQuantity();
	/// Gets the Type of the selected item.
	enum SellType getType(size_t selected) const;
	/// Gets the index of selected item.
	size_t getItemIndex(size_t selected) const;
	/// Gets the index of the selected craft.
	size_t getCraftIndex(size_t selected) const;
public:
	/// Creates the Sell state.
	SellState(Base *base, OptionsOrigin origin = OPT_GEOSCAPE);
	/// Cleans up the Sell state.
	~SellState();
	/// Runs the timers.
	void think();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
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
	/// Decreases the quantity of an item by one.
	void decrease();
	/// Changes the quantity of an item by the given value.
	void changeByValue(int change, int dir);
	/// Updates the quantity-strings of the selected item.
	void updateItemStrings();
};

}

#endif
