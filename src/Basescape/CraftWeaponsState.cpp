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
#include "CraftWeaponsState.h"
#include <sstream>
#include <cmath>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/Ruleset.h"
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
CraftWeaponsState::CraftWeaponsState(Game *game, Base *base, size_t craft, size_t weapon) : State(game), _base(base), _craft(craft), _weapon(weapon), _weapons()
{
	_screen = false;

	// Create objects
	_window = new Window(this, 220, 160, 50, 20, POPUP_BOTH);
	_btnCancel = new TextButton(140, 16, 90, 156);
	_txtTitle = new Text(208, 16, 56, 28);
	_txtArmament = new Text(76, 9, 66, 52);
	_txtQuantity = new Text(50, 9, 145, 52);
	_txtAmmunition = new Text(68, 9, 195, 44);
	_txtAvailable = new Text(68, 9, 195, 52);
	_lstWeapons = new TextList(188, 80, 58, 68);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtArmament);
	add(_txtQuantity);
	add(_txtAmmunition);
	add(_txtAvailable);
	add(_lstWeapons);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+6);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnCancel->setColor(Palette::blockOffset(15)+6);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&CraftWeaponsState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)+6);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_ARMAMENT"));

	_txtArmament->setColor(Palette::blockOffset(15)+6);
	_txtArmament->setText(_game->getLanguage()->getString("STR_ARMAMENT"));

	_txtQuantity->setColor(Palette::blockOffset(15)+6);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_txtAmmunition->setColor(Palette::blockOffset(15)+6);
	_txtAmmunition->setText(_game->getLanguage()->getString("STR_AMMUNITION_UC"));

	_txtAvailable->setColor(Palette::blockOffset(15)+6);
	_txtAvailable->setText(_game->getLanguage()->getString("STR_AVAILABLE"));

	_lstWeapons->setColor(Palette::blockOffset(13)+10);
	_lstWeapons->setArrowColor(Palette::blockOffset(15)+6);
	_lstWeapons->setColumns(3, 94, 50, 36);
	_lstWeapons->setSelectable(true);
	_lstWeapons->setBackground(_window);
	_lstWeapons->setMargin(8);

	_lstWeapons->addRow(1, _game->getLanguage()->getString("STR_NONE_UC").c_str());
	_weapons.push_back(0);

	std::vector<std::string> weapons = _game->getRuleset()->getCraftWeaponsList();
	for (std::vector<std::string>::iterator i = weapons.begin(); i != weapons.end(); ++i)
	{
		RuleCraftWeapon *w = _game->getRuleset()->getCraftWeapon(*i);
		if (_base->getItems()->getItem(w->getLauncherItem()) > 0)
		{
			_weapons.push_back(w);
			std::wstringstream ss, ss2;
			ss << _base->getItems()->getItem(w->getLauncherItem());
			ss2 << _base->getItems()->getItem(w->getClipItem());
			_lstWeapons->addRow(3, _game->getLanguage()->getString(w->getType()).c_str(), ss.str().c_str(), ss2.str().c_str());
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
void CraftWeaponsState::btnCancelClick(Action *action)
{
	_game->popState();
}

/**
 * Equips the weapon on the craft and returns to the previous screen.
 * @param action Pointer to an action.
 */
void CraftWeaponsState::lstWeaponsClick(Action *action)
{
	CraftWeapon *current = _base->getCrafts()->at(_craft)->getWeapons()->at(_weapon);
	// Remove current weapon
	if (current != 0)
	{
		_base->getItems()->addItem(current->getRules()->getLauncherItem());
		_base->getItems()->addItem(current->getRules()->getClipItem(), (int)floor((double)current->getAmmo() / current->getRules()->getRearmRate()));
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
