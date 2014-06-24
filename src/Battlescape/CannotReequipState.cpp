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
#include "CannotReequipState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Cannot Reequip screen.
 * @param game Pointer to the core game.
 * @param missingItems List of items still needed for reequip.
 */
CannotReequipState::CannotReequipState(std::vector<ReequipStat> missingItems)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(120, 18, 100, 174);
	_txtTitle = new Text(220, 32, 50, 8);
	_txtItem = new Text(142, 9, 10, 50);
	_txtQuantity = new Text(88, 9, 152, 50);
	_txtCraft = new Text(74, 9, 218, 50);
	_lstItems = new TextList(288, 112, 8, 58);

	// Set palette
	setPalette("PAL_GEOSCAPE", 0);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtCraft);
	add(_lstItems);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CannotReequipState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CannotReequipState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&CannotReequipState::btnOkClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setText(tr("STR_NOT_ENOUGH_EQUIPMENT_TO_FULLY_RE_EQUIP_SQUAD"));
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setWordWrap(true);

	_txtItem->setColor(Palette::blockOffset(15)-1);
	_txtItem->setText(tr("STR_ITEM"));

	_txtQuantity->setColor(Palette::blockOffset(15)-1);
	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_txtCraft->setColor(Palette::blockOffset(15)-1);
	_txtCraft->setText(tr("STR_CRAFT"));

	_lstItems->setColor(Palette::blockOffset(8)+10);
	_lstItems->setColumns(3, 162, 46, 80);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);

	for (std::vector<ReequipStat>::iterator i = missingItems.begin(); i != missingItems.end(); ++i)
	{
		std::wostringstream ss;
		ss << i->qty;
		_lstItems->addRow(3, tr(i->item).c_str(), ss.str().c_str(), i->craft.c_str());
	}
}

/**
 *
 */
CannotReequipState::~CannotReequipState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CannotReequipState::btnOkClick(Action *)
{
	_game->popState();
}

}
