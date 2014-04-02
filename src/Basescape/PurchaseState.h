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
#ifndef OPENXCOM_PURCHASESTATE_H
#define OPENXCOM_PURCHASESTATE_H

#include "../Engine/State.h"
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

enum eSelectedTab {TAB_PERSONNEL, TAB_CRAFT, TAB_ITEMS, TAB_TRANSFERS};

/**
 * Purchase/Hire screen that lets the player buy
 * new items for a base.
 */
class PurchaseState : public State
{
private:
	Base *_base;
	TextButton *_btnOk, *_btnCancel, *_btnTab, *_btnPrev, *_btnNext;
	Window *_window;
	Text *_txtTitle, *_txtFunds, *_txtPurchases, *_txtItem, *_txtCost, *_txtQuantity, *_txtInStorage;;
	TextList *_lstPersonnel, *_lstCraft, *_lstItems, *_selList;
	std::vector<std::string> _crafts, _items, _craftItems;
	std::vector<std::string> _tabs;
	std::vector<TextList*> _lists;
	std::vector<int> _qtys, _qtysPersonnel, _qtysCraft;
	unsigned int _sel;
	size_t _selTab;
	int _total, _pQty, _cQty;
	int _iQty;
	Timer *_timerInc, *_timerDec;
	/// Gets selected price.
	int getPrice();
	/// Is it excluded in the options file?
	bool isExcluded(std::string item);
	/// Updates the tab index.
	void updateIndex(size_t &index, std::vector<std::string> &list, int change);
	/// Updates the currently displayed tab.
	void updateTab(int change);
public:
	/// Creates the Purchase state.
	PurchaseState(Game *game, Base *base);
	/// Cleans up the Purchase state.
	~PurchaseState();
	/// Runs the timers.
	void think();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Tab button.
	void btnTabClick(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
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
};

}

#endif
