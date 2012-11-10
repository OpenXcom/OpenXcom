/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_NOCONTAINMENTSTATE_H
#define OPENXCOM_NOCONTAINMENTSTATE_H

#include "../Engine/State.h"
#include "DebriefingState.h"
#include <vector>
#include <string>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;

/**
 * Screen shown when there's not enough equipment
 * to re-equip a craft after a mission.
 */
class NoContainmentState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtQuantity, *_txtCraft;
	TextList *_lstItems;
public:
	/// Creates the Cannot Reequip state.
	NoContainmentState(Game *game);
	/// Cleans up the Cannot Reequip state.
	~NoContainmentState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
};

}

#endif
