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
 * @param globe Pointer to the globe to refresh.
 * @param facilities List of facilities.
 */
SelectStartFacilityState::SelectStartFacilityState(Game *game, Base *base, State *state, Globe *globe, std::vector<RuleBaseFacility*> facilities) : BuildFacilitiesState(game, base, state, false), _globe(globe)
{
	_facilities = facilities;
	_lstFacilities->onMouseClick((ActionHandler)&SelectStartFacilityState::lstFacilitiesClick);
	populateBuildList();
}

/**
 *
 */
SelectStartFacilityState::~SelectStartFacilityState()
{

}

/**
 * Populates the build list from the current "available" facilities.
 */
void SelectStartFacilityState::populateBuildList()
{
	_lstFacilities->clearList();
	for (std::vector<RuleBaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		_lstFacilities->addRow(1, _game->getLanguage()->getString((*i)->getType()).c_str());
	}
}

/**
 * Places the selected facility.
 * @param action Pointer to an action.
 */
void SelectStartFacilityState::lstFacilitiesClick(Action *)
{
	_game->pushState(new PlaceStartFacilityState(_game, _base, this, _facilities[_lstFacilities->getSelectedRow()]));
}

/**
 * Callback from PlaceStartFacilityState.
 * Removes placed facility from the list.
 */
void SelectStartFacilityState::facilityBuilt()
{
	_facilities.erase(_facilities.begin() + _lstFacilities->getSelectedRow());
	if( _facilities.size() == 0 )
	{
		_game->popState();
		_game->popState(); // return to geoscape, force timer to start.
	} else {
		populateBuildList();
	}
}

}
