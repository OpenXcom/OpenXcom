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
#include "SelectStartFacilityState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Savegame/SavedGame.h"
#include "PlaceStartFacilityState.h"
#include "BasescapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Build Facilities window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param state Pointer to the base state to refresh.
 */
SelectStartFacilityState::SelectStartFacilityState(Game *game, Base *base, State *state, Globe *globe, std::vector<RuleBaseFacility*> Facilities) : State(game), _base(base), _state(state), _globe(globe), _facilities(Facilities)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 128, 160, 192, 40, POPUP_VERTICAL);
	_btnOk = new TextButton(112, 16, 200, 176);
	_txtTitle = new Text(118, 16, 197, 48);
	_lstFacilities = new TextList(100, 96, 200, 64);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_lstFacilities);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SelectStartFacilityState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(13));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_INSTALLATION"));

	_lstFacilities->setColor(Palette::blockOffset(13)+5);
	_lstFacilities->setArrowColor(Palette::blockOffset(13)+5);
	_lstFacilities->setColumns(1, 100);
	_lstFacilities->setSelectable(true);
	_lstFacilities->setBackground(_window);
	_lstFacilities->setMargin(2);
	_lstFacilities->onMouseClick((ActionHandler)&SelectStartFacilityState::lstFacilitiesClick);

	PopulateBuildList();
}

/**
 *
 */
SelectStartFacilityState::~SelectStartFacilityState()
{

}

/**
 * Populates the build list from the current "available" facilities
 */
void SelectStartFacilityState::PopulateBuildList()
{
	_lstFacilities->clearList();
	for (std::vector<RuleBaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		_lstFacilities->addRow(1, _game->getLanguage()->getString((*i)->getType()).c_str());
	}
}


/**
 * The player can change the selected base
 * or change info on other screens.
 */
void SelectStartFacilityState::init()
{
	_state->init();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SelectStartFacilityState::btnOkClick(Action *action)
{
	_game->popState();
	// _game->pushState(new BasescapeState(_game, _base, _globe));
}

/**
 * Places the selected facility.
 * @param action Pointer to an action.
 */
void SelectStartFacilityState::lstFacilitiesClick(Action *action)
{
	_game->pushState(new PlaceStartFacilityState(_game, _base, this, _facilities[_lstFacilities->getSelectedRow()]));
}

/**
 * Callback from PlaceStartFacilityState
 * Removes placed facility from list
 */
void SelectStartFacilityState::FacilityBuilt()
{
	_facilities.erase(_facilities.begin() + _lstFacilities->getSelectedRow());
	if( _facilities.size() == 0 )
	{
		_game->popState();
	} else {
		PopulateBuildList();
	}
}

}
