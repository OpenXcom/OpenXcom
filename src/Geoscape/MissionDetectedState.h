#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Engine/State.h"

namespace OpenXcom
{

class MissionSite;
class TextButton;
class Window;
class Text;
class GeoscapeState;

/**
 * Displays info on a detected mission site.
 */
class MissionDetectedState : public State
{
private:
	MissionSite *_mission;
	GeoscapeState *_state;

	TextButton *_btnIntercept, *_btnCenter, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtCity;
public:
	/// Creates the Mission Detected state.
	MissionDetectedState(MissionSite *mission, GeoscapeState *state);
	/// Cleans up the Mission Detected state.
	~MissionDetectedState();
	/// Handler for clicking the Intercept button.
	void btnInterceptClick(Action *action);
	/// Handler for clicking the Center on Site button.
	void btnCenterClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}
