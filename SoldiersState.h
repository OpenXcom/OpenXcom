/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__SOLDIERSSTATE_H
#define OPENXCOM__SOLDIERSSTATE_H

#include "State.h"

class TextButton;
class Window;
class Text;
class TextList;
class Base;

/**
 * Soldiers screen that lets the player
 * manage all the soldiers in a base.
 */
class SoldiersState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtName, *_txtRank, *_txtCraft;
	TextList *_lstSoldiers;
	Base *_base;
public:
	/// Creates the Soldiers state.
	SoldiersState(Game *game, Base *base);
	/// Cleans up the Soldiers state.
	~SoldiersState();
	/// Updates the soldier names.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Soldiers list.
	void lstSoldiersClick(SDL_Event *ev, int scale);
};

#endif
