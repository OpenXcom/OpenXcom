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
#ifndef OPENXCOM_PLACESTARTFACILITYSTATE_H
#define OPENXCOM_PLACESTARTFACILITYSTATE_H

#include "PlaceFacilityState.h"
#include "SelectStartFacilityState.h"

namespace OpenXcom
{

class Globe;

/**
 * Window shown when the player tries to
 * build a facility.
 */
class PlaceStartFacilityState : public PlaceFacilityState
{
private:
	Globe *_globe;
	SelectStartFacilityState *_select;

public:
	/// Creates the Place Facility state.
	PlaceStartFacilityState(Game *game, Base *base, SelectStartFacilityState *select, RuleBaseFacility *rule);
	/// Cleans up the Place Facility state.
	~PlaceStartFacilityState();
	/// Handler for clicking the base view.
	void viewClick(Action *action);
};

}

#endif
