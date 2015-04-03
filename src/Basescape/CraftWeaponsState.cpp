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
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "CraftWeaponsState.h"
#include <sstream>
#include <cmath>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Craft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/Base.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Craft Weapons window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param craft ID of the selected craft.
 * @param weapon ID of the selected weapon.
 */
CraftWeaponsState::CraftWeaponsState(Base *base, size_t craft, size_t weapon) : _base(base), _craft(craft), _weapon(weapon)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 220, 160, 50, 20, POPUP_BOTH);
	_btnCancel = new TextButton(140, 16, 90, 156);
	_txtTitle = new Text(208, 17, 56, 28);
	_txtArmament = new Text(76, 9, 66, 52);
	_txtQuantity = new Text(50, 9, 140, 52);
	_txtAmmunition = new Text(68, 17, 200, 44);
	_lstWeapons = new TextList(188, 80, 58, 68);

	// Set palette
	setPalette("PAL_BASESCAPE", _game->getRuleset()->getInterface("craftWeapons")->getElement("palette")->color);

	add(_window, "window", "craftWeapons");
	add(_btnCancel, "button", "craftWeapons");
	add(_txtTitle, "text", "craftWeapons");
	add(_txtArmament, "text", "craftWeapons");
	add(_txtQuantity, "text", "craftWeapons");
	add(_txtAmmunition, "text", "craftWeapons");
	add(_lstWeapons, "list", "craftWeapons");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&CraftWeaponsState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&CraftWeaponsState::btnCancelClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_SELECT_ARMAMENT"));

	_txtArmament->setText(tr("STR_ARMAMENT"));

	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_txtAmmunition->setText(tr("STR_AMMUNITION_AVAILABLE"));
	_txtAmmunition->setWordWrap(true);

	_lstWeapons->setColumns(3, 94, 50, 36);
	_lstWeapons->setSelectable(true);
	_lstWeapons->setBackground(_window);
	_lstWeapons->setMargin(8);

	_lstWeapons->addRow(1, tr("STR_NONE_UC").c_str());
	_weapons.push_back(0);

	const std::vector<std::string> &weapons = _game->getRuleset()->getCraftWeaponsList();
	for (std::vector<std::string>::const_iterator i = weapons.begin(); i != weapons.end(); ++i)
	{
		RuleCraftWeapon *w = _game->getRuleset()->getCraftWeapon(*i);
		if (_base->getItems()->getItem(w->getLauncherItem()) > 0)
		{
			_weapons.push_back(w);
			std::wostringstream ss, ss2;
			ss << _base->getItems()->getItem(w->getLauncherItem());
			if (!w->getClipItem().empty())
			{
				ss2 << _base->getItems()->getItem(w->getClipItem());
			}
			else
			{
				ss2 << tr("STR_NOT_AVAILABLE");
			}
			_lstWeapons->addRow(3, tr(w->getType()).c_str(), ss.str().c_str(), ss2.str().c_str());
		}
	}
	_lstWeapons->onMouseClick((ActionHandler)&CraftWeaponsState::lstWeaponsClick);
}

/**
 *
 */
CraftWeaponsState::~CraftWeaponsState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CraftWeaponsState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Equips the weapon on the craft and returns to the previous screen.
 * @param action Pointer to an action.
 */
void CraftWeaponsState::lstWeaponsClick(Action *)
{
	CraftWeapon *current = _base->getCrafts()->at(_craft)->getWeapons()->at(_weapon);
	// Remove current weapon
	if (current != 0)
	{
		_base->getItems()->addItem(current->getRules()->getLauncherItem());
		_base->getItems()->addItem(current->getRules()->getClipItem(), current->getClipsLoaded(_game->getRuleset()));
		delete current;
		_base->getCrafts()->at(_craft)->getWeapons()->at(_weapon) = 0;
	}

	// Equip new weapon
	if (_weapons[_lstWeapons->getSelectedRow()] != 0)
	{
		CraftWeapon *sel = new CraftWeapon(_weapons[_lstWeapons->getSelectedRow()], 0);
		sel->setRearming(true);
		_base->getItems()->removeItem(sel->getRules()->getLauncherItem());
		_base->getCrafts()->at(_craft)->getWeapons()->at(_weapon) = sel;
		if (_base->getCrafts()->at(_craft)->getStatus() == "STR_READY")
		{
			_base->getCrafts()->at(_craft)->setStatus("STR_REARMING");
		}
	}

	_game->popState();
}

}
