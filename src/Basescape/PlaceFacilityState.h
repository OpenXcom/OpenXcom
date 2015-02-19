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
	RuleBaseFacility *_rule;

	BaseView *_view;
	TextButton *_btnCancel;
	Window *_window;
	Text *_txtFacility, *_txtCost, *_numCost, *_txtTime, *_numTime, *_txtMaintenance, *_numMaintenance;
public:
	/// Creates the Place Facility state.
	PlaceFacilityState(Base *base, RuleBaseFacility *rule);
	/// Cleans up the Place Facility state.
	~PlaceFacilityState();
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the base view.
	void viewClick(Action *action);
};

}

#endif
