/*
 * Copyright 2010 OpenXcom Developers.
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
#include "SoldierArmorState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Base.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Armor window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param soldier ID of the selected soldier.
 */
SoldierArmorState::SoldierArmorState(Game *game, Base *base, unsigned int soldier) : State(game), _base(base), _soldier(soldier)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 192, 120, 64, 40, POPUP_BOTH);
	_btnCancel = new TextButton(140, 16, 90, 136);
	_txtTitle = new Text(182, 9, 69, 48);
	_txtSoldier = new Text(182, 9, 69, 56);
	_txtType = new Text(90, 9, 80, 72);
	_txtQuantity = new Text(70, 9, 177, 72);
	_lstArmor = new TextList(160, 48, 73, 88);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtSoldier);
	add(_txtType);
	add(_txtQuantity);
	add(_lstArmor);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+13);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnCancel->setColor(Palette::blockOffset(13)+8);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&SoldierArmorState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(13)+5);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_ARMOR_FOR"));

	_txtSoldier->setColor(Palette::blockOffset(13)+5);
	_txtSoldier->setAlign(ALIGN_CENTER);
	Soldier *s = _base->getSoldiers()->at(_soldier);
	_txtSoldier->setText(s->getName());

	_txtType->setColor(Palette::blockOffset(13)+5);
	_txtType->setText(_game->getLanguage()->getString("STR_TYPE"));

	_txtQuantity->setColor(Palette::blockOffset(13)+5);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_lstArmor->setColor(Palette::blockOffset(13));
	_lstArmor->setArrowColor(Palette::blockOffset(13)+8);
	_lstArmor->setColumns(2, 112, 41);
	_lstArmor->setSelectable(true);
	_lstArmor->setBackground(_window);
	_lstArmor->setMargin(8);
	_lstArmor->onMouseClick((ActionHandler)&SoldierArmorState::lstArmorClick);

	_lstArmor->addRow(1, _game->getLanguage()->getString("STR_NONE_UC").c_str());
}

/**
 *
 */
SoldierArmorState::~SoldierArmorState()
{
	
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierArmorState::btnCancelClick(Action *action)
{
	_game->popState();
}

/**
 * Equips the armor on the soldier and returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierArmorState::lstArmorClick(Action *action)
{
	_game->popState();
}

}
