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
#ifndef OPENXCOM_CRAFTINFOSTATE_H
#define OPENXCOM_CRAFTINFOSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class TextButton;
class Window;
class Text;
class TextEdit;
class Surface;
class Craft;

/**
 * Craft Info screen that shows all the
 * info of a specific craft.
 */
class CraftInfoState : public State
{
private:
	static const int WeaponMax = 4;

	Base *_base;
	size_t _craftId;
	Craft *_craft;
	std::wstring _defaultName;
	int _weaponNum;

	TextButton *_btnOk, *_btnW[WeaponMax], *_btnCrew, *_btnEquip, *_btnArmor;
	Window *_window;
	TextEdit *_edtCraft;
	Text *_txtDamage, *_txtFuel;
	Text *_txtWName[WeaponMax], *_txtWAmmo[WeaponMax];
	Surface *_sprite, *_weapon[WeaponMax], *_crew, *_equip;
	/// Formats an amount of time.
	std::wstring formatTime(int time);
public:
	/// Creates the Craft Info state.
	CraftInfoState(Base *base, size_t craftId);
	/// Cleans up the Craft Info state.
	~CraftInfoState();
	/// Updates the craft info.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the weapon button.
	void btnWClick(Action *action);
	/// Handler for clicking the Crew button.
	void btnCrewClick(Action *action);
	/// Handler for clicking the Equipment button.
	void btnEquipClick(Action *action);
	/// Handler for clicking the Armor button.
	void btnArmorClick(Action *action);
	/// Handler for changing the text on the Name edit.
	void edtCraftChange(Action *action);
};

}

#endif
