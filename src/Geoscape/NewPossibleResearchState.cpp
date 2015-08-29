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
#include <algorithm>
#include "NewPossibleResearchState.h"
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Mod/ResourcePack.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Mod/RuleResearch.h"
#include "../Basescape/ResearchState.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Options.h"
#include "../Mod/Ruleset.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the EndResearch screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param possibilities List of newly possible ResearchProject
 */
NewPossibleResearchState::NewPossibleResearchState(Base * base, const std::vector<RuleResearch *> & possibilities) : _base(base)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 288, 180, 16, 10);
	_btnOk = new TextButton(160, 14, 80, 149);
	_btnResearch = new TextButton(160, 14, 80, 165);
	_txtTitle = new Text(288, 40, 16, 20);
	_lstPossibilities = new TextList(288, 80, 16, 56);

	// Set palette
	setInterface("geoResearch");

	add(_window, "window", "geoResearch");
	add(_btnOk, "button", "geoResearch");
	add(_btnResearch, "button", "geoResearch");
	add(_txtTitle, "text1", "geoResearch");
	add(_lstPossibilities, "text2", "geoResearch");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewPossibleResearchState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NewPossibleResearchState::btnOkClick, Options::keyCancel);
	_btnResearch->setText(tr("STR_ALLOCATE_RESEARCH"));
	_btnResearch->onMouseClick((ActionHandler)&NewPossibleResearchState::btnResearchClick);
	_btnResearch->onKeyboardPress((ActionHandler)&NewPossibleResearchState::btnResearchClick, Options::keyOk);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_lstPossibilities->setColumns(1, 288);
	_lstPossibilities->setBig();
	_lstPossibilities->setAlign(ALIGN_CENTER);
	
	size_t tally(0);
	for (std::vector<RuleResearch *>::const_iterator iter = possibilities.begin(); iter != possibilities.end(); ++iter)
	{
		bool liveAlien = _game->getRuleset()->getUnit((*iter)->getName()) != 0;
		if (!_game->getSavedGame()->wasResearchPopped(*iter) && (*iter)->getRequirements().empty() && !liveAlien)
		{
			_game->getSavedGame()->addPoppedResearch((*iter));
			_lstPossibilities->addRow (1, tr((*iter)->getName()).c_str());
		}
		else
		{
			tally++;
		}
	}

	if (!(tally == possibilities.size() || possibilities.empty()))
	{
		_txtTitle->setText(tr("STR_WE_CAN_NOW_RESEARCH"));
	}
}

/**
 * return to the previous screen
 * @param action Pointer to an action.
 */
void NewPossibleResearchState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Open the ResearchState so the player can dispatch available scientist.
 * @param action Pointer to an action.
 */
void NewPossibleResearchState::btnResearchClick(Action *)
{
	_game->popState();
	_game->pushState (new ResearchState(_base));
}

}
