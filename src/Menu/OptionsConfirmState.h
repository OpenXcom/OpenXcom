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
#ifndef OPENXCOM_OPTIONSCONFIRMSTATE_H
#define OPENXCOM_OPTIONSCONFIRMSTATE_H

#include "../Engine/State.h"
#include "OptionsBaseState.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class Timer;

/**
 * Confirmation window when Display Options
 * are changed.
 */
class OptionsConfirmState : public State
{
private:
	OptionsOrigin _origin;
	TextButton *_btnYes, *_btnNo;
	Window *_window;
	Text *_txtTitle, *_txtTimer;
	Timer *_timer;
	int _countdown;
public:
	/// Creates the Confirm Display Options state.
	OptionsConfirmState(OptionsOrigin origin);
	/// Cleans up the Confirm Display Options state.
	~OptionsConfirmState();
	/// Handle timers.
	void think();
	/// Countdown for reverting options.
	void countdown();
	/// Handler for clicking the Yes button.
	void btnYesClick(Action *action);
	/// Handler for clicking the No button.
	void btnNoClick(Action *action);
};

}

#endif
