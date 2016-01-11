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

class TextButton;
class Window;
class Text;
class Craft;
class GeoscapeState;

/**
 * Window displayed when a craft starts running out of fuel
 * (only has exactly enough to make it back to base).
 */
class LowFuelState : public State
{
private:
	Craft *_craft;
	GeoscapeState *_state;

	TextButton *_btnOk, *_btnOk5Secs;
	Window *_window;
	Text *_txtTitle, *_txtMessage;
public:
	/// Creates the Low Fuel state.
	LowFuelState(Craft *craft, GeoscapeState *state);
	/// Cleans up the Low Fuel state.
	~LowFuelState();
	/// Handler for clicking the Ok button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Ok - 5 Secs button.
	void btnOk5SecsClick(Action *action);
};

}
