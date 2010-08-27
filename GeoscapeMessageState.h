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
#ifndef OPENXCOM__GEOSCAPEMESSAGESTATE_H
#define OPENXCOM__GEOSCAPEMESSAGESTATE_H

#include <string>
#include "State.h"

using namespace std;

class TextButton;
class Window;
class Text;

/**
 * Generic window used to display information messages
 * when the player is on the Geoscape.
 */
class GeoscapeMessageState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtMessage;
public:
	/// Creates the Geoscape Message state.
	GeoscapeMessageState(Game *game, string str);
	/// Cleans up the Geoscape Message state.
	~GeoscapeMessageState();
	/// Handler for clicking the OK button.
	void btnOkClick(SDL_Event *ev, int scale);
};

#endif
