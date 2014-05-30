/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_ALIENTERRORSTATE_H
#define OPENXCOM_ALIENTERRORSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TerrorSite;
class TextButton;
class Window;
class Text;
class GeoscapeState;

/**
 * Displays info on a terror site.
 */
class AlienTerrorState : public State
{
private:
	TerrorSite *_terror;
	GeoscapeState *_state;

	TextButton *_btnIntercept, *_btnCentre, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtCity;
public:
	/// Creates the Ufo Detected state.
	AlienTerrorState(Game *game, TerrorSite *terror, const std::string &city, GeoscapeState *state);
	/// Cleans up the Ufo Detected state.
	~AlienTerrorState();
	/// Handler for clicking the Intercept button.
	void btnInterceptClick(Action *action);
	/// Handler for clicking the Centre on UFO button.
	void btnCentreClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}

#endif
