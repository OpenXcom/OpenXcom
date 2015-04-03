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
#ifndef OPENXCOM_UFODETECTEDSTATE_H
#define OPENXCOM_UFODETECTEDSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Ufo;
class TextButton;
class Window;
class Text;
class TextList;
class GeoscapeState;

/**
 * Displays info on a detected UFO.
 */
class UfoDetectedState : public State
{
private:
	Ufo *_ufo;
	GeoscapeState *_state;

	TextButton *_btnIntercept, *_btnCentre, *_btnCancel;
	Window *_window;
	Text *_txtUfo, *_txtDetected, *_txtHyperwave;
	TextList *_lstInfo, *_lstInfo2;
public:
	/// Creates the Ufo Detected state.
	UfoDetectedState(Ufo *ufo, GeoscapeState *state, bool detected, bool hyper);
	/// Cleans up the Ufo Detected state.
	~UfoDetectedState();
	/// Handler for clicking the Intercept button.
	void btnInterceptClick(Action *action);
	/// Handler for clicking the Centre on UFO button.
	void btnCentreClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}

#endif
