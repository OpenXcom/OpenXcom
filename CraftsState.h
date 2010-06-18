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
#ifndef OPENXCOM__CRAFTSSTATE_H
#define OPENXCOM__CRAFTSSTATE_H

#include <sstream>
#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "Button.h"
#include "Window.h"
#include "Text.h"
#include "TextList.h"
#include "Craft.h"
#include "Base.h"

using namespace std;

/**
 * Equip Craft screen that lets the player
 * manage all the crafts in a base.
 */
class CraftsState : public State
{
private:
	Button *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtBase, *_txtName, *_txtStatus, *_txtWeapon, *_txtCrew, *_txtHwp;
	TextList *_lstCrafts;
	Base *_base;
public:
	/// Creates the Crafts state.
	CraftsState(Game *game, Base *base);
	/// Cleans up the Crafts state.
	~CraftsState();
	/// Handler for clicking the OK button.
	void btnOkClick(SDL_Event *ev, int scale);
};

#endif
