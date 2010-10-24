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
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_INTERACTIVE_SURFACE_H
#define OPENXCOM_INTERACTIVE_SURFACE_H

#include "SDL.h"
#include "Surface.h"
#include "State.h"

typedef State &(State::*ActionHandler)(Action*);

/**
 * Surface that the user can interact with.
 * Specialized version of the standard Surface that
 * processes all the various SDL events and turns
 * them into useful interactions with the Surface,
 * so specialized subclasses don't need to worry about it.
 */
class InteractiveSurface : public Surface
{
protected:
	ActionHandler _click, _press, _release, _in, _over, _out, _keyPress, _keyRelease;
	bool _isPressed, _isHovered, _isFocused;
	int _validButton;

public:
	/// Creates a new interactive surface with the specified size and position.
	InteractiveSurface(int width, int height, int x = 0, int y = 0);
	/// Cleans up the interactive surface.
	virtual ~InteractiveSurface();
	/// Sets the surface's visibility.
	void setVisible(bool visible);
	/// Processes any pending events.
	virtual void handle(Action *action, State *state);
	/// Sets focus on this surface.
	virtual void focus();
	/// Hooks an action handler to a mouse click on the surface.
	void onMouseClick(ActionHandler handler);
	/// Hooks an action handler to a mouse press over the surface.
	void onMousePress(ActionHandler handler);
	/// Hooks an action handler to a mouse release over the surface.
	void onMouseRelease(ActionHandler handler);
	/// Hooks an action handler to moving the mouse into the surface.
	void onMouseIn(ActionHandler handler);
	/// Hooks an action handler to moving the mouse over the surface.
	void onMouseOver(ActionHandler handler);
	/// Hooks an action handler to moving the mouse out of the surface.
	void onMouseOut(ActionHandler handler);
	/// Hooks an action handler to pressing a key when the surface is focused.
	void onKeyboardPress(ActionHandler handler);
	/// Hooks an action handler to releasing a key when the surface is focused.
	void onKeyboardRelease(ActionHandler handler);
	/// Processes a mouse button press event.
	virtual void mousePress(Action *action, State *state);
	/// Processes a mouse button release event.
	virtual void mouseRelease(Action *action, State *state);
	/// Processes a mouse click event.
	virtual void mouseClick(Action *action, State *state);
	/// Processes a mouse hover in event.
	virtual void mouseIn(Action *action, State *state);
	/// Processes a mouse hover event.
	virtual void mouseOver(Action *action, State *state);
	/// Processes a mouse hover out event.
	virtual void mouseOut(Action *action, State *state);
	/// Processes a keyboard key press event.
	virtual void keyboardPress(Action *action, State *state);
	/// Processes a keyboard key release event.
	virtual void keyboardRelease(Action *action, State *state);

};

#endif
