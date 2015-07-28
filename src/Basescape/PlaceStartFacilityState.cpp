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
#include "PlaceStartFacilityState.h"
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Text.h"
#include "BaseView.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Menu/ErrorMessageState.h"
#include "SelectStartFacilityState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Place Facility window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param select Pointer to the selection state.
 * @param rule Pointer to the facility ruleset to build.
 */
PlaceStartFacilityState::PlaceStartFacilityState(Base *base, SelectStartFacilityState *select, RuleBaseFacility *rule) : PlaceFacilityState(base, rule), _select(select)
{
	_view->onMouseClick((ActionHandler)&PlaceStartFacilityState::viewClick);
	_numCost->setText(tr("STR_NONE"));
	_numTime->setText(tr("STR_NONE"));
}

/**
 *
 */
PlaceStartFacilityState::~PlaceStartFacilityState()
{

}

/**
 * Processes clicking on facilities.
 * @param action Pointer to an action.
 */
void PlaceStartFacilityState::viewClick(Action *)
{
	if (!_view->isPlaceable(_rule))
	{
		_game->popState();
		_game->pushState(new ErrorMessageState(tr("STR_CANNOT_BUILD_HERE"), _palette, _game->getRuleset()->getInterface("basescape")->getElement("errorMessage")->color, "BACK01.SCR", _game->getRuleset()->getInterface("basescape")->getElement("errorPalette")->color));
	}
	else
	{
		BaseFacility *fac = new BaseFacility(_rule, _base);
		fac->setX(_view->getGridX());
		fac->setY(_view->getGridY());
		_base->getFacilities()->push_back(fac);
		_game->popState();
		_select->facilityBuilt();
	}
}

}
