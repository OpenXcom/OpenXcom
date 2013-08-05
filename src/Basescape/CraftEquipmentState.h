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
#ifndef OPENXCOM_CRAFTEQUIPMENTSTATE_H
#define OPENXCOM_CRAFTEQUIPMENTSTATE_H

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

/**
 * Equipment screen that lets the player
 * pick the equipment to carry on a craft.
 */
class CraftEquipmentState : public State
{
private:
	TextButton *_btnOk, *_btnClear;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtStores, *_txtAvailable, *_txtUsed, *_txtCrew;
	TextList *_lstEquipment;
	Timer *_timerLeft, *_timerRight;
	unsigned int _sel;
	Base *_base;
	size_t _craft;
	std::vector<std::string> _items;
	int _changeValueByMouseWheel;
	bool _allowChangeListValuesByMouseWheel;
	/// Updates quantities of item.
	void updateQuantity();
public:
	/// Creates the Craft Equipment state.
	CraftEquipmentState(Game *game, Base *base, size_t craft);
	/// Cleans up the Craft Equipment state.
	~CraftEquipmentState();
	/// Runs the timers.
	void think();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for pressing a Move Left arrow in the list.
	void lstEquipmentLeftArrowPress(Action *action);
	/// Handler for releasing a Move Left arrow in the list.
	void lstEquipmentLeftArrowRelease(Action *action);
	/// Handler for clicking a Move Left arrow in the list.
	void lstEquipmentLeftArrowClick(Action *action);
	/// Handler for pressing a Move Right arrow in the list.
	void lstEquipmentRightArrowPress(Action *action);
	/// Handler for releasing a Move Right arrow in the list.
	void lstEquipmentRightArrowRelease(Action *action);
	/// Handler for clicking a Move Right arrow in the list.
	void lstEquipmentRightArrowClick(Action *action);
	/// Handler for pressing-down a mouse-button in the list.
	void lstEquipmentMousePress(Action *action);
	/// Moves an item to the base.
	void moveLeft();
	/// Moves the given number of items to the base.
	void moveLeftByValue(int change);
	/// Moves an item to the craft.
	void moveRight();
	/// Moves the given number of items to the craft.
	void moveRightByValue(int change);
	/// Empties the contents of the craft, moving all of the items back to the base.
	void btnClearClick(Action *action);
};

}

#endif
