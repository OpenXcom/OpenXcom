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
#include "PlaceLiftState.h"
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
#include "../Ruleset/Ruleset.h"
#include "BasescapeState.h"
#include "SelectStartFacilityState.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Place Lift screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param globe Pointer to the Geoscape globe.
 * @param first Is this a custom starting base?
 */
PlaceLiftState::PlaceLiftState(Base *base, Globe *globe, bool first) : _base(base), _globe(globe), _first(first)
{
	// Create objects
	_view = new BaseView(192, 192, 0, 8);
	_txtTitle = new Text(320, 9, 0, 0);

	// Set palette
	setInterface("placeFacility");

	add(_view, "baseView", "basescape");
	add(_txtTitle, "text", "placeFacility");

	centerAllSurfaces();

	// Set up objects
	_view->setTexture(_game->getResourcePack()->getSurfaceSet("BASEBITS.PCK"));
	_view->setBase(_base);
	for (std::vector<std::string>::const_iterator i = _game->getRuleset()->getBaseFacilitiesList().begin(); i != _game->getRuleset()->getBaseFacilitiesList().end(); ++i)
	{
		if (_game->getRuleset()->getBaseFacility(*i)->isLift())
		{
			_lift = _game->getRuleset()->getBaseFacility(*i);
			break;
		}
	}
	_view->setSelectable(_lift->getSize());
	_view->onMouseClick((ActionHandler)&PlaceLiftState::viewClick);

	_txtTitle->setText(tr("STR_SELECT_POSITION_FOR_ACCESS_LIFT"));
}

/**
 *
 */
PlaceLiftState::~PlaceLiftState()
{

}

/**
 * Processes clicking on facilities.
 * @param action Pointer to an action.
 */
void PlaceLiftState::viewClick(Action *)
{
	BaseFacility *fac = new BaseFacility(_lift, _base);
	fac->setX(_view->getGridX());
	fac->setY(_view->getGridY());
	_base->getFacilities()->push_back(fac);
	_game->popState();
	BasescapeState *bState = new BasescapeState(_base, _globe);
	_game->getSavedGame()->setSelectedBase(_game->getSavedGame()->getBases()->size() - 1);
	_game->pushState(bState);
	if (_first)
	{
		_game->pushState(new SelectStartFacilityState(_base, bState, _globe));
	}
}

}
