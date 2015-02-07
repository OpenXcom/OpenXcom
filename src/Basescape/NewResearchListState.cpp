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
#include "NewResearchListState.h"
#include <algorithm>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Ruleset/RuleResearch.h"
#include "ResearchInfoState.h"
#include "../Savegame/ItemContainer.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the Research list screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
NewResearchListState::NewResearchListState(Base *base) : _base(base)
{
	_screen = false;

	_window = new Window(this, 230, 140, 45, 30, POPUP_BOTH);
	_btnOK = new TextButton(214, 16, 53, 146);
	_txtTitle = new Text(214, 16, 53, 38);
	_lstResearch = new TextList(198, 88, 53, 54);

	// Set palette
	setPalette("PAL_BASESCAPE", _game->getRuleset()->getInterface("researchMenu")->getElement("palette")->color);

	add(_window, "window", "selectNewResearch");
	add(_btnOK, "button", "selectNewResearch");
	add(_txtTitle, "text", "selectNewResearch");
	add(_lstResearch, "list", "selectNewResearch");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOK->setText(tr("STR_OK"));
	_btnOK->onMouseClick((ActionHandler)&NewResearchListState::btnOKClick);
	_btnOK->onKeyboardPress((ActionHandler)&NewResearchListState::btnOKClick, Options::keyCancel);

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_NEW_RESEARCH_PROJECTS"));

	_lstResearch->setColumns(1, 190);
	_lstResearch->setSelectable(true);
	_lstResearch->setBackground(_window);
	_lstResearch->setMargin(8);
	_lstResearch->setAlign(ALIGN_CENTER);
	_lstResearch->onMouseClick((ActionHandler)&NewResearchListState::onSelectProject);
}

/**
 * Initializes the screen (fills the list).
 */
void NewResearchListState::init()
{
	State::init();
	fillProjectList();
}

/**
 * Selects the RuleResearch to work on.
 * @param action Pointer to an action.
 */
void NewResearchListState::onSelectProject(Action *)
{
	_game->pushState(new ResearchInfoState(_base, _projects[_lstResearch->getSelectedRow()]));
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void NewResearchListState::btnOKClick(Action *)
{
	_game->popState();
}

/**
 * Fills the list with possible ResearchProjects.
 */
void NewResearchListState::fillProjectList()
{
	_projects.clear();
	_lstResearch->clearList();
	_game->getSavedGame()->getAvailableResearchProjects(_projects, _game->getRuleset() , _base);
	std::vector<RuleResearch*>::iterator it = _projects.begin();
	while (it != _projects.end())
	{
		if ((*it)->getRequirements().empty())
		{
			_lstResearch->addRow(1, tr((*it)->getName()).c_str());
			++it;
		}
		else
		{
			it = _projects.erase(it);
		}
	}
}

}
