/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_UFOHYPERDETECTEDSTATE_H
#define OPENXCOM_UFOHYPERDETECTEDSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Ufo;
class TextButton;
class Window;
class Text;
class TextList;
class GeoscapeState;
class Region;
class RuleRegion;

/**
 * Displays info on a detected UFO.
 */
class UfoHyperDetectedState : public State
{
private:
	Ufo *_ufo;
	GeoscapeState *_state;
	bool _detected;

	TextButton *_btnCentre, *_btnCancel;
	Window *_window;
	Text *_txtUfo, *_txtDetected, *_txtDetected2;
	TextList *_lstInfo, *_lstInfo2;
public:
	/// Creates the Ufo Detected state.
	UfoHyperDetectedState(Game *game, Ufo *ufo, GeoscapeState *state, bool detected);
	/// Cleans up the Ufo Detected state.
	~UfoHyperDetectedState();
	/// Updates the palette.
	void init();
	/// Handler for clicking the Centre on UFO button.
	void btnCentreClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}

#endif
