#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class GeoscapeState;
class Base;

/**
 * Items Arriving window that displays all
 * the items that have arrived at bases.
 */
class ItemsArrivingState : public State
{
private:
	GeoscapeState *_state;
	Base *_base;
	TextButton *_btnOk, *_btnGotoBase;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtQuantity, *_txtDestination;
	TextList *_lstTransfers;
public:
	/// Creates the ItemsArriving state.
	ItemsArrivingState(GeoscapeState *state);
	/// Cleans up the ItemsArriving state.
	~ItemsArrivingState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Go To Base button.
	void btnGotoBaseClick(Action *action);
};

}
