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
#ifndef OPENXCOM_BUILDFACILITIESSTATE_H
#define OPENXCOM_BUILDFACILITIESSTATE_H

#include <vector>
#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class TextButton;
class Window;
class Text;
class TextList;
class RuleBaseFacility;

/**
 * Window shown with all the facilities
 * available to build.
 */
class BuildFacilitiesState : public State
{
protected:
	Base *_base;
	State *_state;
	std::vector<RuleBaseFacility*> _facilities;

	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle;
	TextList *_lstFacilities;
public:
	/// Creates the Build Facilities state.
	BuildFacilitiesState(Game *game, Base *base, State *state, bool cancellable);
	/// Cleans up the Build Facilities state.
	~BuildFacilitiesState();
	/// Populates the build option list.
	virtual void PopulateBuildList();
	/// Updates the base stats.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Facilities list.
	virtual void lstFacilitiesClick(Action *action);
};

}

#endif
