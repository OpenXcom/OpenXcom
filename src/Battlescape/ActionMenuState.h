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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_ACTIONMENUSTATE_H
#define OPENXCOM_ACTIONMENUSTATE_H

#include "../Engine/State.h"
#include "BattlescapeGame.h"

namespace OpenXcom
{

class Game;
class ActionMenuItem;

/**
 * Window that allows the player
 * to select a battlescape action.
 */
class ActionMenuState : public State
{
private:
	BattleAction *_action;
	ActionMenuItem *_actionMenu[6];
	/// Adds a new menu item for an action.
	void addItem(BattleActionType ba, const std::string &name, int *id);
public:
	/// Creates the Action Menu state.
	ActionMenuState(BattleAction *action, int x, int y);
	/// Cleans up the Action Menu state.
	~ActionMenuState();
	/// Handler for right-clicking anything.
	void handle(Action *action);
	/// Handler for clicking a action menu item.
	void btnActionMenuItemClick(Action *action);
	/// Update the resolution settings, we just resized the window.
	void resize(int &dX, int &dY);
};

}

#endif
