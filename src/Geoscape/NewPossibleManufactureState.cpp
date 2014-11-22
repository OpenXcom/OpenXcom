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
#include "NewPossibleManufactureState.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/RuleManufacture.h"
#include "../Basescape/ManufactureState.h"
#include "../Engine/Options.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the EndManufacture screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param possibilities List of newly possible ManufactureProject
 */
NewPossibleManufactureState::NewPossibleManufactureState(Base * base, const std::vector<RuleManufacture *> & possibilities) : _base(base)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 288, 180, 16, 10);
	_btnOk = new TextButton(160, 14, 80, 149);
	_btnManufacture = new TextButton(160, 14, 80, 165);
	_txtTitle = new Text(288, 40, 16, 20);
	_lstPossibilities = new TextList(260, 80, 21, 56);

	// Set palette
	setPalette("PAL_GEOSCAPE", 6);

	add(_window);
	add(_btnOk);
	add(_btnManufacture);
	add(_txtTitle);
	add(_lstPossibilities);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewPossibleManufactureState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NewPossibleManufactureState::btnOkClick, Options::keyCancel);
	_btnManufacture->setColor(Palette::blockOffset(8)+5);
	_btnManufacture->setText(tr("STR_ALLOCATE_MANUFACTURE"));
	_btnManufacture->onMouseClick((ActionHandler)&NewPossibleManufactureState::btnManufactureClick);
	_btnManufacture->onKeyboardPress((ActionHandler)&NewPossibleManufactureState::btnManufactureClick, Options::keyOk);
	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_WE_CAN_NOW_PRODUCE"));

	_lstPossibilities->setColor(Palette::blockOffset(8)+10);
	_lstPossibilities->setColumns(1, 288);
	_lstPossibilities->setBig();
	_lstPossibilities->setAlign(ALIGN_CENTER);
	for (std::vector<RuleManufacture *>::const_iterator iter = possibilities.begin(); iter != possibilities.end(); ++iter)
	{
		_lstPossibilities->addRow (1, tr((*iter)->getName()).c_str());
	}
}

/**
 * return to the previous screen
 * @param action Pointer to an action.
 */
void NewPossibleManufactureState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Open the ManufactureState so the player can dispatch available scientist.
 * @param action Pointer to an action.
 */
void NewPossibleManufactureState::btnManufactureClick(Action *)
{
	_game->popState();
	_game->pushState (new ManufactureState(_base));
}

}
