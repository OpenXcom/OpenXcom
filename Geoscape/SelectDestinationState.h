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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_SELECTDESTINATIONSTATE_H
#define OPENXCOM_SELECTDESTINATIONSTATE_H

#include "../Engine/State.h"

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
	InteractiveSurface *_btnRotateLeft, *_btnRotateRight, *_btnRotateUp, *_btnRotateDown, *_btnZoomIn, *_btnZoomOut;
	Window *_window;
	Text *_txtTitle;
	TextButton *_btnCancel;
public:
	/// Creates the Select Destination state.
	SelectDestinationState(Game *game, Craft *craft, Globe *globe);
	/// Cleans up the Select Destination state.
	~SelectDestinationState();
	/// Updates the palette.
	void init();
	/// Runs the timer.
	void think();
	/// Handles events.
	void handle(SDL_Event *ev, int scale);
	/// Handler for clicking the globe.
	void globeClick(SDL_Event *ev, int scale);
	/// Handler for pressing the Rotate Left arrow.
	void btnRotateLeftPress(SDL_Event *ev, int scale);
	/// Handler for releasing the Rotate Left arrow.
	void btnRotateLeftRelease(SDL_Event *ev, int scale);
	/// Handler for pressing the Rotate Right arrow.
	void btnRotateRightPress(SDL_Event *ev, int scale);
	/// Handler for releasing the Rotate Right arrow.
	void btnRotateRightRelease(SDL_Event *ev, int scale);
	/// Handler for pressing the Rotate Up arrow.
	void btnRotateUpPress(SDL_Event *ev, int scale);
	/// Handler for releasing the Rotate Up arrow.
	void btnRotateUpRelease(SDL_Event *ev, int scale);
	/// Handler for pressing the Rotate Down arrow.
	void btnRotateDownPress(SDL_Event *ev, int scale);
	/// Handler for releasing the Rotate Down arrow.
	void btnRotateDownRelease(SDL_Event *ev, int scale);
	/// Handler for clicking the Zoom In icon.
	void btnZoomInClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Zoom Out icon.
	void btnZoomOutClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(SDL_Event *ev, int scale);
};

#endif
