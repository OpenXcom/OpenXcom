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
#ifndef OPENXCOM_CRAFTERRORSTATE_H
#define OPENXCOM_CRAFTERRORSTATE_H

#include <string>
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class GeoscapeState;

/**
 * Window used to notify the player when
 * an error occurs with a craft procedure.
 */
class CraftErrorState : public State
{
private:
	GeoscapeState *_state;
	TextButton *_btnOk, *_btnOk5Secs;
	Window *_window;
	Text *_txtMessage;
public:
	/// Creates the Craft Error state.
	CraftErrorState(GeoscapeState *state, const std::wstring &msg);
	/// Cleans up the Craft Error state.
	~CraftErrorState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the OK 5 Secs button.
	void btnOk5SecsClick(Action *action);
};

}

#endif
