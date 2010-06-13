/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__PLACEFACILITYSTATE_H
#define OPENXCOM__PLACEFACILITYSTATE_H

#include <sstream>
#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "Button.h"
#include "Window.h"
#include "Text.h"
#include "RuleBaseFacility.h"

/**
 * Window shown when the player tries to
 * build a facility.
 */
class PlaceFacilityState : public State
{
private:
	RuleBaseFacility *_rule;

	Button *_btnCancel;
	Window *_window;
	Text *_txtFacility, *_txtCost, *_numCost, *_txtTime, *_numTime, *_txtMaintenance, *_numMaintenance;
public:
	/// Creates the Place Facility state.
	PlaceFacilityState(Game *game, RuleBaseFacility *rule);
	/// Cleans up the Place Facility state.
	~PlaceFacilityState();
	/// Handler for clicking the Cancel button.
	void btnCancelClick(SDL_Event *ev, int scale);
};

#endif
