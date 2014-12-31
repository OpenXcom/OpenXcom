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
#ifndef OPENXCOM_MANAGEALIENCONTAINMENTSTATE_H
#define OPENXCOM_MANAGEALIENCONTAINMENTSTATE_H

#include "../Engine/State.h"
#include "../Menu/OptionsBaseState.h"
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
 * ManageAlienContainment screen that lets the player manage
 * alien numbers in a particular base.
 */
class ManageAlienContainmentState : public State
{
private:
	Base *_base;
	OptionsOrigin _origin;
	TextButton *_btnOk, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtUsed, *_txtAvailable, *_txtItem, *_txtLiveAliens, *_txtDeadAliens;
	TextList *_lstAliens;
	Timer *_timerInc, *_timerDec;
	std::vector<int> _qtys;
	std::vector<std::string> _aliens;
	size_t _sel;
	int _aliensSold, _researchedAliens;
	bool _overCrowded;
	/// Gets selected quantity.
	int getQuantity();
public:
	/// Creates the ManageAlienContainment state.
	ManageAlienContainmentState(Base *base, OptionsOrigin origin);
	/// Cleans up the ManageAlienContainment state.
	~ManageAlienContainmentState();
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
	/// Increases the quantity of an alien by one.
	void increase();
	/// Increases the quantity of an alien by the given value.
	void increaseByValue(int change);
	/// Decreases the quantity of an alien by one.
	void decrease();
	/// Decreases the quantity of an alien by the given value.
	void decreaseByValue(int change);
	/// Updates the quantity-strings of the selected alien.
	void updateStrings();
};

}

#endif
