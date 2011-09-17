/*
 * Copyright 2011 OpenXcom Developers.
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
#include "NewPossibleResearchState.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/RuleResearchProject.h"
#include "GeoResearchState.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the EndResearch screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param possibilities List of newly possible ResearchProject
 */
NewPossibleResearchState::NewPossibleResearchState(Game * game, Base * base, const std::vector<RuleResearchProject *> & possibilities) : State (game), _base(base)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 280, 180, 20, 10, POPUP_BOTH);
	_btnOk = new TextButton(180, 13, 70, 150);
	_btnSelect = new TextButton(180, 13, 70, 165);
	_txtTitle = new Text(200, 40, 60, 30);
	_possibilities = new TextList(160, 70, 80, 70);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnSelect);
	add(_txtTitle);
	add(_possibilities);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+8);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewPossibleResearchState::btnOkClick);
	_btnSelect->setColor(Palette::blockOffset(8)+8);
	_btnSelect->setText(_game->getLanguage()->getString("STR_ALLOCATE_RESEARCH"));
	_btnSelect->onMouseClick((ActionHandler)&NewPossibleResearchState::btnSelectClick);
	if (!possibilities.empty ())
	{
		_txtTitle->setColor(Palette::blockOffset(8)+5);
		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setText(_game->getLanguage()->getString("STR_WE_CAN_NOW_RESEARCH"));
	}

	_possibilities->setColor(Palette::blockOffset(8)+10);
	_possibilities->setArrowColor(Palette::blockOffset(8)+8);
	_possibilities->setColumns(1, 160);
	_possibilities->setSelectable(false);
	_possibilities->setBackground(_window);
	_possibilities->setMargin(2);
	_possibilities->setBig();
	_possibilities->setAlign(ALIGN_CENTER);
	for(std::vector<RuleResearchProject *>::const_iterator iter = possibilities.begin (); iter != possibilities.end (); ++iter)
	{
		_possibilities->addRow (1, _game->getLanguage()->getString((*iter)->getName ()).c_str());
	}
}

/**
 * return to the previous screen
 * @param action Pointer to an action.
 */
void NewPossibleResearchState::btnOkClick(Action *action)
{
	_game->popState ();
}

/**
 * Open the ResearchState so the player can dispatch available scientist.
 * @param action Pointer to an action.
 */
void NewPossibleResearchState::btnSelectClick(Action *action)
{
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->pushState (new GeoResearchState(_game, _base));
}
}
