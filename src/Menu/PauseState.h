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
#include "OptionsBaseState.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;

/**
 * Options window shown for loading/saving/quitting the game.
 * Not to be confused with the Game Options window
 * for changing game settings during runtime.
 */
class PauseState : public State
{
private:
	OptionsOrigin _origin;
	TextButton *_btnLoad, *_btnSave, *_btnAbandon, *_btnOptions, *_btnCancel;
	Window *_window;
	Text *_txtTitle;
public:
	/// Creates the Pause state.
	PauseState(OptionsOrigin origin);
	/// Cleans up the Pause state.
	~PauseState();
	/// Handler for clicking the Load Game button.
	void btnLoadClick(Action *action);
	/// Handler for clicking the Save Game button.
	void btnSaveClick(Action *action);
	/// Handler for clicking the Abandon Game button.
	void btnAbandonClick(Action *action);
	/// Handler for clicking the Game Options button.
	void btnOptionsClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}
