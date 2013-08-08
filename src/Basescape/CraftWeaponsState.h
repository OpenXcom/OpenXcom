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
#ifndef OPENXCOM_CRAFTWEAPONSSTATE_H
#define OPENXCOM_CRAFTWEAPONSSTATE_H

#include <vector>
#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class TextButton;
class Window;
class Text;
class TextList;
class RuleCraftWeapon;

/**
 * Select Armament window for
 * changing the weapon equipped on a craft.
 */
class CraftWeaponsState : public State
{
private:
	Base *_base;
	size_t _craft, _weapon;

	TextButton *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtArmament, *_txtQuantity, *_txtAmmunition, *_txtAvailable;
	TextList *_lstWeapons;
	std::vector<RuleCraftWeapon*> _weapons;
public:
	/// Creates the Craft Weapons state.
	CraftWeaponsState(Game *game, Base *base, size_t craft, size_t weapon);
	/// Cleans up the Craft Weapons state.
	~CraftWeaponsState();
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Weapons list.
	void lstWeaponsClick(Action *action);
};

}

#endif
