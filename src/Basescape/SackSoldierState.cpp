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
#include <sstream>
#include "SackSoldierState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/Armor.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in a Sack Soldier window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param soldierId ID of the soldier to sack.
 */
SackSoldierState::SackSoldierState(Base *base, size_t soldierId) : _base(base), _soldierId(soldierId)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 152, 80, 84, 60);
	_btnOk = new TextButton(44, 16, 100, 115);
	_btnCancel = new TextButton(44, 16, 176, 115);
	_txtTitle = new Text(142, 9, 89, 75);
	_txtSoldier = new Text(142, 9, 89, 85);

	// Set palette
	setInterface("sackSoldier");

	add(_window, "window", "sackSoldier");
	add(_btnOk, "button", "sackSoldier");
	add(_btnCancel, "button", "sackSoldier");
	add(_txtTitle, "text", "sackSoldier");
	add(_txtSoldier, "text", "sackSoldier");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SackSoldierState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SackSoldierState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&SackSoldierState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&SackSoldierState::btnCancelClick, Options::keyCancel);

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_SACK"));

	std::wostringstream ss;
	ss << _base->getSoldiers()->at(_soldierId)->getName(true) << "?";

	_txtSoldier->setAlign(ALIGN_CENTER);
	_txtSoldier->setText(ss.str().c_str());
}

/**
 *
 */
SackSoldierState::~SackSoldierState()
{

}

/**
 * Sacks the soldier and returns
 * to the previous screen.
 * @param action Pointer to an action.
 */
void SackSoldierState::btnOkClick(Action *)
{
	Soldier *soldier = _base->getSoldiers()->at(_soldierId);
	if (soldier->getArmor()->getStoreItem() != "STR_NONE")
	{
		_base->getItems()->addItem(soldier->getArmor()->getStoreItem());
	}
	_base->getSoldiers()->erase(_base->getSoldiers()->begin() + _soldierId);
	delete soldier;
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SackSoldierState::btnCancelClick(Action *)
{
	_game->popState();
}

}
