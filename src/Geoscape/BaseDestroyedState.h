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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_BASEDESTROYEDSTATE_H
#define OPENXCOM_BASEDESTROYEDSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{
class Base;
class Window;
class Text;
class TextButton;

/**
 * Screen that allows the player
 * to pick a target for a craft on the globe.
 */
class BaseDestroyedState : public State
{
private:
	Window *_window;
	Text *_txtMessage;
	TextButton *_btnOk;
	Base *_base;
public:
	/// Creates the Select Destination state.
	BaseDestroyedState(Base *base);
	/// Cleans up the Select Destination state.
	~BaseDestroyedState();
	/// Handler for clicking the Cydonia mission button.
	void btnOkClick(Action *action);

};

}

#endif
