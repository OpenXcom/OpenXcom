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
#ifndef OPENXCOM_PLACEFACILITYSTATE_H
#define OPENXCOM_PLACEFACILITYSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class RuleBaseFacility;
class BaseView;
class TextButton;
class Window;
class Text;

/**
 * Window shown when the player tries to
 * build a facility.
 */
class PlaceFacilityState : public State
{
protected:
	Base *_base;
	State *_state;
	RuleBaseFacility *_rule;

	BaseView *_view;
	Text *_cursorTxtFacility;
	TextButton *_btnCancel;
	Window *_window;
	Text *_txtFacility, *_txtCost, *_numCost, *_txtTime, *_numTime, *_txtMaintenance, *_numMaintenance;
public:
	/// Creates the Place Facility state.
	PlaceFacilityState(Game *game, Base *base, RuleBaseFacility *rule, State *state, int viewCameraPosX, int viewCameraPosY);
	/// Cleans up the Place Facility state.
	~PlaceFacilityState();
	/// Updates the base stats.
	void init();
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the base view.
	virtual void viewLeftClick(Action *action);
	/// Handler for hovering the base view.
	void viewMouseOver(Action *action);
	/// Handler for hovering out of the base view.
	void viewMouseOut(Action *action);
};

}

#endif
