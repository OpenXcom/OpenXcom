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

/**
 * Purchase/Hire screen that lets the player buy
 * new items for a base.
 */
class PurchaseState : public State
{
private:
	TextButton *_btnOk, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtFunds, *_txtPurchases, *_txtItem, *_txtCost, *_txtQuantity;
	TextList *_lstItems;
	std::vector<std::string> _crafts, _items;
	std::vector<int> _qtys;
	int _total, _sel;
	Timer *_timerInc, *_timerDec;
public:
	/// Creates the Purchase state.
	PurchaseState(Game *game);
	/// Cleans up the Purchase state.
	~PurchaseState();
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
	/// Handler for pressing a Decrease arrow in the list.
	void lstItemsRightArrowPress(Action *action);
	/// Handler for releasing a Decrease arrow in the list.
	void lstItemsRightArrowRelease(Action *action);
	/// Increases the quantity of an item.
	void increase();
	/// Decreases the quantity of an item.
	void decrease();
};

}

#endif
