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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class Globe;
class InteractiveSurface;
class Timer;
class Window;
class Text;
class TextButton;

/**
 * Screen that allows the player
 * to place a new base on the globe.
 */
class BuildNewBaseState : public State
{
private:
	Base *_base;
	Globe *_globe;
	InteractiveSurface *_btnRotateLeft, *_btnRotateRight, *_btnRotateUp, *_btnRotateDown, *_btnZoomIn, *_btnZoomOut;
	Window *_window;
	Text *_txtTitle;
	TextButton *_btnCancel;
	Timer *_hoverTimer;
	bool _first;
	bool _oldshowradar;
	double _oldlat,_oldlon;
	int _mousex, _mousey;
public:
	/// Creates the Build New Base state.
	BuildNewBaseState(Base *base, Globe *globe, bool first);
	/// Cleans up the Build New Base state.
	~BuildNewBaseState();
	/// Resets globe.
	void init();
	/// Runs the timer.
	void think();
	/// Handles actions.
	void handle(Action *action);
	/// Handler for clicking the globe.
	void globeClick(Action *action);
	/// Handler for mouse hovering the globe.
	void globeHover(Action *action);
	/// Handler for redrawing hover (delayed)
	void hoverRedraw(void);
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
	/// Let the state know the window has been resized.
	void resize(int &dX, int &dY);
};

}
