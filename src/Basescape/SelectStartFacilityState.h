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
#ifndef OPENXCOM_SELECTSTARTFACILITYSTATE_H
#define OPENXCOM_SELECTSTARTFACILITYSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class Globe;
class TextButton;
class Window;
class Text;
class TextList;
class RuleBaseFacility;

/**
 * Window shown with all the facilities
 * available to build.
 */
class SelectStartFacilityState : public State
{
private:
	Base *_base;
	Globe *_globe;
	State *_state;
	std::vector<RuleBaseFacility*> _facilities;

	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle;
	TextList *_lstFacilities;
public:
	/// Creates the Build Facilities state.
	SelectStartFacilityState(Game *game, Base *base, State *state, Globe *globe, std::vector<RuleBaseFacility*> Facilities);
	/// Cleans up the Build Facilities state.
	~SelectStartFacilityState();
	/// Populate the build option list
	void PopulateBuildList();
	/// Updates the base stats.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Facilities list.
	void lstFacilitiesClick(Action *action);
	/// Handler for when facility actually built
	void FacilityBuilt();
};

}

#endif
