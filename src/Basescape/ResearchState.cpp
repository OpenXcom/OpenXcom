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
#include "ResearchState.h"
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
#include "../Savegame/Base.h"
#include "NewResearchListState.h"
#include "../Savegame/ResearchProject.h"
#include "../Ruleset/RuleResearch.h"
#include "ResearchInfoState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Research screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
ResearchState::ResearchState(Game *game, Base *base) : State(game), _base(base)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnNew = new TextButton(148, 16, 8, 176);
	_btnOk = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtAvailable = new Text(150, 9, 10, 24);
	_txtAllocated = new Text(150, 9, 160, 24);
	_txtSpace = new Text(300, 9, 10, 34);
	_txtProject = new Text(106, 9, 10, 44);
	_txtScientists = new Text(135, 9, 115, 44);
	_txtProgress = new Text(64, 9, 238, 44);
	_lstResearch = new TextList(288, 120, 8, 54);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);

	add(_window);
	add(_btnNew);
	add(_btnOk);
	add(_txtTitle);
	add(_txtAvailable);
	add(_txtAllocated);
	add(_txtSpace);
	add(_txtProject);
	add(_txtScientists);
	add(_txtProgress);
	add(_lstResearch);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnNew->setColor(Palette::blockOffset(15)+6);
	_btnNew->setText(_game->getLanguage()->getString("STR_NEW_PROJECT"));
	_btnNew->onMouseClick((ActionHandler)&ResearchState::btnNewClick);

	_btnOk->setColor(Palette::blockOffset(15)+6);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ResearchState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_CURRENT_RESEARCH"));

	_txtAvailable->setColor(Palette::blockOffset(13)+10);
	_txtAvailable->setSecondaryColor(Palette::blockOffset(13));

	_txtAllocated->setColor(Palette::blockOffset(13)+10);
	_txtAllocated->setSecondaryColor(Palette::blockOffset(13));

	_txtSpace->setColor(Palette::blockOffset(13)+10);
	_txtSpace->setSecondaryColor(Palette::blockOffset(13));

	_txtProject->setColor(Palette::blockOffset(13)+10);
	_txtProject->setText(_game->getLanguage()->getString("STR_RESEARCH_PROJECT"));

	_txtScientists->setColor(Palette::blockOffset(13)+10);
	_txtScientists->setText(_game->getLanguage()->getString("STR_SCIENTISTS_ALLOCATED_UC"));

	_txtProgress->setColor(Palette::blockOffset(13)+10);
	_txtProgress->setText(_game->getLanguage()->getString("STR_PROGRESS"));

	_lstResearch->setColor(Palette::blockOffset(15)+6);
	_lstResearch->setArrowColor(Palette::blockOffset(13)+10);
	_lstResearch->setColumns(3, 158, 70, 58);
	_lstResearch->setSelectable(true);
	_lstResearch->setBackground(_window);
	_lstResearch->setMargin(2);
	_lstResearch->onMouseClick((ActionHandler)&ResearchState::onSelectProject);
	fillProjectList();
}

/**
 *
 */
ResearchState::~ResearchState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ResearchState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ResearchState::btnNewClick(Action *action)
{
	_game->pushState(new NewResearchListState(_game, _base));
}

/**
 * Display list of possible ResearchProject
 * @param action Pointer to an action.
*/
void ResearchState::onSelectProject(Action *action)
{
	const std::vector<ResearchProject *> & baseProjects(_base->getResearch());
	_game->pushState(new ResearchInfoState(_game, _base, baseProjects[_lstResearch->getSelectedRow()]));
}

/**
 * Init State
*/
void ResearchState::init()
{
	fillProjectList();
}

/**
 * Fill list with Base ResearchProject. Also update count of available lab space and available/allocated scientist.
*/
void ResearchState::fillProjectList()
{
	const std::vector<ResearchProject *> & baseProjects(_base->getResearch());
	_lstResearch->clearList();
	for(std::vector<ResearchProject *>::const_iterator iter = baseProjects.begin (); iter != baseProjects.end (); ++iter)
	{
		std::wstringstream sstr;
		sstr << (*iter)->getAssigned ();
		const RuleResearch *r = (*iter)->getRules();
		std::wstring wstr = _game->getLanguage()->getString(r->getName ());
		_lstResearch->addRow(3, wstr.c_str(), sstr.str().c_str(), _game->getLanguage()->getString((*iter)->getResearchProgress()).c_str());
	}
	std::wstringstream ss;
	ss << _game->getLanguage()->getString("STR_SCIENTISTS_AVAILABLE") << L'\x01' << _base->getAvailableScientists();
	_txtAvailable->setText(ss.str());
	std::wstringstream ss2;
	ss2 << _game->getLanguage()->getString("STR_SCIENTISTS_ALLOCATED") << L'\x01' << _base->getAllocatedScientists();
	_txtAllocated->setText(ss2.str());
	std::wstringstream ss3;
	ss3 << _game->getLanguage()->getString("STR_LABORATORY_SPACE_AVAILABLE") << L'\x01' << _base->getFreeLaboratories();
	_txtSpace->setText(ss3.str());
}
}
