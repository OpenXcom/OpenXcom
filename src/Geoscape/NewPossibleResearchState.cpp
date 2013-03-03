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
#include <algorithm>
#include "NewPossibleResearchState.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/RuleResearch.h"
#include "../Basescape/ResearchState.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the EndResearch screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param possibilities List of newly possible ResearchProject
 */
NewPossibleResearchState::NewPossibleResearchState(Game * game, Base * base, const std::vector<RuleResearch *> & possibilities) : State (game), _base(base)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 288, 180, 16, 10);
	_btnOk = new TextButton(160, 14, 80, 149);
	_btnResearch = new TextButton(160, 14, 80, 165);
	_txtTitle = new Text(288, 40, 16, 20);
	_lstPossibilities = new TextList(288, 80, 16, 56);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnResearch);
	add(_txtTitle);
	add(_lstPossibilities);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewPossibleResearchState::btnOkClick);
	_btnResearch->setColor(Palette::blockOffset(8)+5);
	_btnResearch->setText(_game->getLanguage()->getString("STR_ALLOCATE_RESEARCH"));
	_btnResearch->onMouseClick((ActionHandler)&NewPossibleResearchState::btnResearchClick);
	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_lstPossibilities->setColor(Palette::blockOffset(8)+10);
	_lstPossibilities->setColumns(1, 288);
	_lstPossibilities->setBig();
	_lstPossibilities->setAlign(ALIGN_CENTER);
	
	size_t tally(0);
	for(std::vector<RuleResearch *>::const_iterator iter = possibilities.begin (); iter != possibilities.end (); ++iter)
	{
		std::vector<std::string>::const_iterator unlocked = std::find((*iter)->getUnlocked().begin(), (*iter)->getUnlocked().end(), "STR_ALIEN_ORIGINS");
		if((*iter)->getRequirements().size() == 0 && unlocked == (*iter)->getUnlocked().end())
		{
			_lstPossibilities->addRow (1, _game->getLanguage()->getString((*iter)->getName ()).c_str());
		}
		else
		{
			tally++;
		}
	}

	if (!(tally == possibilities.size () || possibilities.empty ()))
	{
		_txtTitle->setText(_game->getLanguage()->getString("STR_WE_CAN_NOW_RESEARCH"));
	}
}

/**
 * Resets the palette.
 */
void NewPossibleResearchState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);
}

/**
 * return to the previous screen
 * @param action Pointer to an action.
 */
void NewPossibleResearchState::btnOkClick(Action *)
{
	_game->popState ();
}

/**
 * Open the ResearchState so the player can dispatch available scientist.
 * @param action Pointer to an action.
 */
void NewPossibleResearchState::btnResearchClick(Action *)
{
	_game->popState();
	_game->pushState (new ResearchState(_game, _base));
}
}
