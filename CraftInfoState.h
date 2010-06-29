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
#ifndef OPENXCOM__CRAFTINFOSTATE_H
#define OPENXCOM__CRAFTINFOSTATE_H

#include <sstream>
#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "TextButton.h"
#include "Window.h"
#include "Text.h"
#include "Craft.h"
#include "Base.h"

using namespace std;

/**
 * Craft Info screen that shows all the 
 * info of a specific craft.
 */
class CraftInfoState : public State
{
private:
	Base *_base;
	unsigned int _craft;

	TextButton *_btnOk, *_btnW1, *_btnW2, *_btnCrew, *_btnEquip, *_btnArmour;
	Window *_window;
	Text *_txtCraft, *_txtDamage, *_txtFuel;
	Text *_txtW1Name, *_txtW1Ammo, *_txtW1Max, *_txtW2Name, *_txtW2Ammo, *_txtW2Max;
	Surface *_sprite, *_w1, *_w2, *_crew, *_equip;
public:
	/// Creates the Craft Info state.
	CraftInfoState(Game *game, Base *base, unsigned int craft);
	/// Cleans up the Craft Info state.
	~CraftInfoState();
	/// Updates the craft info.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(SDL_Event *ev, int scale);
};

#endif
