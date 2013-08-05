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
 * @param soldier Pointer to the soldier to sack.
 */
SackSoldierState::SackSoldierState(Game *game, Base *base, Soldier *soldier) : State(game), _base(base), _soldier(soldier)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 152, 80, 84, 60);
	_btnOk = new TextButton(44, 16, 100, 115);
	_btnCancel = new TextButton(44, 16, 176, 115);
	_txtTitle = new Text(142, 9, 89, 75);
	_txtSoldier = new Text(142, 9, 89, 85);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtSoldier);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+6);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SackSoldierState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SackSoldierState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(15)+6);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&SackSoldierState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&SackSoldierState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SACK"));

	_txtSoldier->setColor(Palette::blockOffset(13)+10);
	_txtSoldier->setAlign(ALIGN_CENTER);
	_txtSoldier->setText(_soldier->getName());
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
	if(_soldier->getArmor()->getStoreItem() != "STR_NONE")
	{
		_base->getItems()->addItem(_soldier->getArmor()->getStoreItem());
	}
	for (std::vector<Soldier*>::iterator s = _base->getSoldiers()->begin(); s != _base->getSoldiers()->end(); ++s)
	{
		if (*s == _soldier)
		{
			_base->getSoldiers()->erase(s);
			break;
		}
	}
	delete(_soldier);
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
