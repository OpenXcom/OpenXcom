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
#include "BuildFacilitiesState.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Mod/RuleBaseFacility.h"
#include "../Savegame/SavedGame.h"
#include "PlaceFacilityState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Build Facilities window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param state Pointer to the base state to refresh.
 */
BuildFacilitiesState::BuildFacilitiesState(Base *base, State *state) : _base(base), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 128, 160, 192, 40, POPUP_VERTICAL);
	_btnOk = new TextButton(112, 16, 200, 176);
	_lstFacilities = new TextList(104, 104, 200, 64);
	_txtTitle = new Text(118, 17, 197, 48);

	// Set palette
	setInterface("selectFacility");

	add(_window, "window", "selectFacility");
	add(_btnOk, "button", "selectFacility");
	add(_txtTitle, "text", "selectFacility");
	add(_lstFacilities, "list", "selectFacility");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK05.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BuildFacilitiesState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&BuildFacilitiesState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_INSTALLATION"));

	_lstFacilities->setColumns(1, 104);
	_lstFacilities->setSelectable(true);
	_lstFacilities->setBackground(_window);
	_lstFacilities->setMargin(2);
	_lstFacilities->setWordWrap(true);
	_lstFacilities->setScrolling(true, 0);
	_lstFacilities->onMouseClick((ActionHandler)&BuildFacilitiesState::lstFacilitiesClick);

	PopulateBuildList();
}

/**
 *
 */
BuildFacilitiesState::~BuildFacilitiesState()
{

}

/**
 * Populates the build list from the current "available" facilities.
 */
void BuildFacilitiesState::PopulateBuildList()
{
	const std::vector<std::string> &facilities = _game->getMod()->getBaseFacilitiesList();
	for (std::vector<std::string>::const_iterator i = facilities.begin(); i != facilities.end(); ++i)
	{
		RuleBaseFacility *rule = _game->getMod()->getBaseFacility(*i);
		if (_game->getSavedGame()->isResearched(rule->getRequirements()) && !rule->isLift())
			_facilities.push_back(rule);
	}

	for (std::vector<RuleBaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		_lstFacilities->addRow(1, tr((*i)->getType()).c_str());
	}
}

/**
 * The player can change the selected base
 * or change info on other screens.
 */
void BuildFacilitiesState::init()
{
	_state->init();
	State::init();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BuildFacilitiesState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Places the selected facility.
 * @param action Pointer to an action.
 */
void BuildFacilitiesState::lstFacilitiesClick(Action *)
{
	_game->pushState(new PlaceFacilityState(_base, _facilities[_lstFacilities->getSelectedRow()]));
}

}
