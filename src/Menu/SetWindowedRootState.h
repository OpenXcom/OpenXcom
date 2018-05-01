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
class TextEdit;
class OptionsVideoState;

/**
 * Asks user new coordinates when he pushes Fixed Borderless Pos button.
 * are changed.
 */
class SetWindowedRootState : public State
{
private:
	OptionsOrigin _origin;
	OptionsVideoState *_optionsVideoState;
	TextButton *_btnOk, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtWindowedModePositionX, *_txtWindowedModePositionY;
	TextEdit *_edtWindowedModePositionX, *_edtWindowedModePositionY;
public:
	/// Creates the Confirm Display Options state.
	SetWindowedRootState(OptionsOrigin origin, OptionsVideoState *optionsVideoState);
	/// Cleans up the Confirm Display Options state.
	~SetWindowedRootState();
	/// Handler for clicking the Ok button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}
