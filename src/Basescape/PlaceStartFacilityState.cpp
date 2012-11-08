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
#include "PlaceStartFacilityState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "BaseView.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Savegame/SavedGame.h"
#include "../Menu/ErrorMessageState.h"
#include "BasescapeState.h"
#include "SelectStartFacilityState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Place Facility window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param rule Pointer to the facility ruleset to build.
 */
PlaceStartFacilityState::PlaceStartFacilityState(Game *game, Base *base, SelectStartFacilityState *select, RuleBaseFacility *rule) : PlaceFacilityState(game, base, rule), _select(select)
{
	_view->onMouseClick((ActionHandler)&PlaceStartFacilityState::viewClick);
	_numCost->setText(_game->getLanguage()->getString("STR_NONE"));
	_numTime->setText(_game->getLanguage()->getString("STR_NONE"));
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
void PlaceStartFacilityState::viewClick(Action *action)
{
	if (!_view->isPlaceable(_rule))
	{
		_game->popState();
		_game->pushState(new ErrorMessageState(_game, "STR_CANNOT_BUILD_HERE", Palette::blockOffset(15)+1, "BACK01.SCR", 6));
	}
	else
	{
		BaseFacility *fac = new BaseFacility(_rule, _base);
		fac->setX(_view->getGridX());
		fac->setY(_view->getGridY());
		_base->getFacilities()->push_back(fac);
		_game->popState();
		_select->FacilityBuilt();
	}
}

}
