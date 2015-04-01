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
#ifndef OPENXCOM_MULTIPLETARGETSSTATE_H
#define OPENXCOM_MULTIPLETARGETSSTATE_H

#include "../Engine/State.h"
#include <vector>

namespace OpenXcom
{

class TextButton;
class Window;
class Craft;
class Target;
class GeoscapeState;

/**
 * Displays a list of possible targets.
 */
class MultipleTargetsState : public State
{
private:
	static const int MARGIN = 10;
	static const int SPACING = 4;
	static const int BUTTON_HEIGHT = 16;

	std::vector<Target*> _targets;
	Craft *_craft;
	GeoscapeState *_state;

	Window *_window;
	std::vector<TextButton*> _btnTargets;
public:
	/// Creates the Multiple Targets state.
	MultipleTargetsState(std::vector<Target*> targets, Craft *craft, GeoscapeState *state);
	/// Cleans up the Multiple Targets state.
	~MultipleTargetsState();
	/// Updates the window.
	void init();
	/// Popup for a target.
	void popupTarget(Target *target);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Targets list.
	void btnTargetClick(Action *action);
};

}

#endif
