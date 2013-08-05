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
 * Select Squad screen that lets the player
 * pick the soldiers to assign to a craft.
 */
class CraftSoldiersState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtName, *_txtRank, *_txtCraft, *_txtAvailable, *_txtUsed;
	TextList *_lstSoldiers;

	Base *_base;
	size_t _craft;
public:
	/// Creates the Craft Soldiers state.
	CraftSoldiersState(Game *game, Base *base, size_t craft);
	/// Cleans up the Craft Soldiers state.
	~CraftSoldiersState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Shows Soldiers in a list.
	void populateList();
	/// Handler for clicking the Soldiers reordering button.
	void lstItemsLeftArrowClick(Action *action);
	/// Handler for clicking the Soldiers reordering button.
	void lstItemsRightArrowClick(Action *action);
	/// Handler for clicking the Soldiers list.
	void lstSoldiersClick(Action *action);
};

}

#endif
