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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_SELECTDESTINATIONSTATE_H
#define OPENXCOM_SELECTDESTINATIONSTATE_H

#include "../Engine/State.h"
#include "../Savegame/ScriptedEvent.h"
#include "../Ruleset/RuleScriptedEvent.h"

namespace OpenXcom
{

class Craft;
class Globe;
class InteractiveSurface;
class Timer;
class Window;
class Text;
class TextButton;

/**
 * Screen that allows the player
 * to pick a target for a craft on the globe.
 */
class SelectDestinationState : public State
{
private:
	Craft *_craft;
	Globe *_globe;
	ScriptedEvent *_scriptedEvent;
	InteractiveSurface *_btnRotateLeft, *_btnRotateRight, *_btnRotateUp, *_btnRotateDown, *_btnZoomIn, *_btnZoomOut;
	Window *_window;
	Text *_txtTitle;
	TextButton *_btnCancel, *_btnEventDestination;
public:
	/// Creates the Select Destination state.
	SelectDestinationState(Craft *craft, Globe *globe);
	/// Cleans up the Select Destination state.
	~SelectDestinationState();
	/// Resets globe.
	void init();
	/// Runs the timer.
	void think();
	/// Handles actions.
	void handle(Action *action);
	/// Handler for clicking the globe.
	void globeClick(Action *action);
	/// Handler for pressing the Rotate Left arrow.
	void btnRotateLeftPress(Action *action);
	/// Handler for releasing the Rotate Left arrow.
	void btnRotateLeftRelease(Action *action);
	/// Handler for pressing the Rotate Right arrow.
	void btnRotateRightPress(Action *action);
	/// Handler for releasing the Rotate Right arrow.
	void btnRotateRightRelease(Action *action);
	/// Handler for pressing the Rotate Up arrow.
	void btnRotateUpPress(Action *action);
	/// Handler for releasing the Rotate Up arrow.
	void btnRotateUpRelease(Action *action);
	/// Handler for pressing the Rotate Down arrow.
	void btnRotateDownPress(Action *action);
	/// Handler for releasing the Rotate Down arrow.
	void btnRotateDownRelease(Action *action);
	/// Handler for left-clicking the Zoom In icon.
	void btnZoomInLeftClick(Action *action);
	/// Handler for right-clicking the Zoom In icon.
	void btnZoomInRightClick(Action *action);
	/// Handler for left-clicking the Zoom Out icon.
	void btnZoomOutLeftClick(Action *action);
	/// Handler for right-clicking the Zoom Out icon.
	void btnZoomOutRightClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Cydonia mission button.
	void btnEventDestinationClick(Action *action);
	/// Let the state know the window has been resized.
	void resize(int &dX, int &dY);

};

}

#endif
