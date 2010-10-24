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
#include "CraftWeaponsState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Resource/LangString.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/Craft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Savegame/Item.h"
#include "../Savegame/Base.h"

/**
 * Initializes all the elements in the Craft Weapons window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param craft ID of the selected craft.
 * @param weapon ID of the selected weapon.
 */
CraftWeaponsState::CraftWeaponsState(Game *game, Base *base, unsigned int craft, unsigned int weapon) : State(game), _base(base), _craft(craft), _weapon(weapon)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 220, 160, 50, 20, POPUP_BOTH);
	_btnCancel = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 140, 16, 90, 156);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 208, 16, 56, 28);
	_txtArmament = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 76, 8, 66, 52);
	_txtQuantity = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 50, 8, 145, 52);
	_txtAmmunition = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 68, 8, 195, 44);
	_txtAvailable = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 68, 8, 195, 52);
	_lstWeapons = new TextList(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 180, 80, 66, 68);

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
	_window->setColor(Palette::blockOffset(15)+9);
	_window->setBackground(game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnCancel->setColor(Palette::blockOffset(15)+9);
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString(STR_CANCEL_UC));
	_btnCancel->onMouseClick((ActionHandler)&CraftWeaponsState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)+6);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_SELECT_ARMAMENT));

	_txtArmament->setColor(Palette::blockOffset(15)+6);
	_txtArmament->setText(_game->getResourcePack()->getLanguage()->getString(STR_ARMAMENT));

	_txtQuantity->setColor(Palette::blockOffset(15)+6);
	_txtQuantity->setText(_game->getResourcePack()->getLanguage()->getString(STR_QUANTITY_UC));

	_txtAmmunition->setColor(Palette::blockOffset(15)+6);
	_txtAmmunition->setText(_game->getResourcePack()->getLanguage()->getString(STR_AMMUNITION));

	_txtAvailable->setColor(Palette::blockOffset(15)+6);
	_txtAvailable->setText(_game->getResourcePack()->getLanguage()->getString(STR_AVAILABLE));

	_lstWeapons->setColor(Palette::blockOffset(13)+10);
	_lstWeapons->setColumns(3, 94, 50, 36);
	_lstWeapons->setSelectable(true);
	_lstWeapons->setBackground(_window);
	_lstWeapons->addRow(STR_NONE, 3, _game->getResourcePack()->getLanguage()->getString(STR_NONE).c_str(), "", "");
	for (int i = STR_STINGRAY_UC; i <= STR_CANNON_UC; i++)
	{
		RuleCraftWeapon *w = _game->getRuleset()->getCraftWeapon((LangString)i);
		if ((*_base->getItems())[w->getLauncherItem()]->getQuantity() > 0)
		{
			std::stringstream ss, ss2;
			ss << (*_base->getItems())[w->getLauncherItem()]->getQuantity();
			ss2 << (*_base->getItems())[w->getClipItem()]->getQuantity();
			_lstWeapons->addRow(i, 3, _game->getResourcePack()->getLanguage()->getString((LangString)i).c_str(), ss.str().c_str(), ss2.str().c_str());
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
 * @note Ammo consumption currently disabled for testing purposes.
 * @param action Pointer to an action.
 */
void CraftWeaponsState::lstWeaponsClick(Action *action)
{
	CraftWeapon *current = _base->getCrafts()->at(_craft)->getWeapons()->at(_weapon);
	// Remove current weapon
	if (current != 0)
	{
		(*_base->getItems())[current->getRules()->getLauncherItem()]->setQuantity((*_base->getItems())[current->getRules()->getLauncherItem()]->getQuantity() + 1);
		//(*_base->getItems())[current->getRules()->getClipItem()]->setQuantity((*_base->getItems())[current->getRules()->getClipItem()]->getQuantity() + current->getAmmo());
		delete current;
		_base->getCrafts()->at(_craft)->getWeapons()->at(_weapon) = 0;
	}

	// Equip new weapon
	if (_lstWeapons->getSelectedValue() != STR_NONE)
	{
		CraftWeapon *sel = new CraftWeapon(_game->getRuleset()->getCraftWeapon((LangString)_lstWeapons->getSelectedValue()), 0);
		(*_base->getItems())[sel->getRules()->getLauncherItem()]->setQuantity((*_base->getItems())[sel->getRules()->getLauncherItem()]->getQuantity() - 1);
		_base->getCrafts()->at(_craft)->getWeapons()->at(_weapon) = sel;
		if (_base->getCrafts()->at(_craft)->getStatus() == STR_READY)
			_base->getCrafts()->at(_craft)->setStatus(STR_REARMING);
	}

	_game->popState();
}
