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
#ifndef OPENXCOM_CRAFTSOLDIERSSTATE_H
#define OPENXCOM_CRAFTSOLDIERSSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;

/**
 * Equipment screen that lets the player
 * pick the equipment to carry on a craft.
 */
class CraftEquipmentState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtStores, *_txtAvailable, *_txtUsed;
	TextList *_lstEquipment;

	Base *_base;
	unsigned int _craft;
public:
	/// Creates the Craft Equipment state.
	CraftEquipmentState(Game *game, Base *base, unsigned int craft);
	/// Cleans up the Craft Equipment state.
	~CraftEquipmentState();
	/// Updates the soldier status.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Equipment list.
	void lstEquipmentClick(Action *action);
};

}

#endif
