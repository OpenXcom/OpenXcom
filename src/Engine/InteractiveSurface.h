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
#include <SDL.h>
#include <map>
#include "Surface.h"
#include "State.h"

namespace OpenXcom
{

typedef void (State::* ActionHandler)(Action*);

/**
 * Surface that the user can interact with.
 * Specialized version of the standard Surface that
 * processes all the various SDL events and turns
 * them into useful interactions with the Surface,
 * so specialized subclasses don't need to worry about it.
 */
class InteractiveSurface : public Surface
{
private:
	static const int NUM_BUTTONS = 7;
	static const SDLKey SDLK_ANY;
	Uint8 _buttonsPressed;
protected:
	std::map<Uint8, ActionHandler> _click, _press, _release;
	ActionHandler _in, _over, _out;
	std::map<SDLKey, ActionHandler> _keyPress, _keyRelease;
	bool _isHovered, _isFocused, _listButton;

	/// Is this mouse button pressed?
	bool isButtonPressed(Uint8 button = 0) const;
	/// Is this mouse button event handled?
	virtual bool isButtonHandled(Uint8 button = 0);
	/// Set a mouse button's internal state.
	void setButtonPressed(Uint8 button, bool pressed);
public:
	/// Creates a new interactive surface with the specified size and position.
	InteractiveSurface(int width, int height, int x = 0, int y = 0);
	/// Cleans up the interactive surface.
	virtual ~InteractiveSurface();
	/// Sets the surface's visibility.
	void setVisible(bool visible);
	/// Processes any pending events.
	virtual void handle(Action *action, State *state);
	/// Sets the focus of this surface.
	virtual void setFocus(bool focus);
	/// Gets the focus of this surface.
	bool isFocused() const;
	/// Unpresses the surface.
	virtual void unpress(State *state);
	/// Hooks an action handler to a mouse click on the surface.
	void onMouseClick(ActionHandler handler, Uint8 button = SDL_BUTTON_LEFT);
	/// Hooks an action handler to a mouse press over the surface.
	void onMousePress(ActionHandler handler, Uint8 button = 0);
	/// Hooks an action handler to a mouse release over the surface.
	void onMouseRelease(ActionHandler handler, Uint8 button = 0);
	/// Hooks an action handler to moving the mouse into the surface.
	void onMouseIn(ActionHandler handler);
	/// Hooks an action handler to moving the mouse over the surface.
	void onMouseOver(ActionHandler handler);
	/// Hooks an action handler to moving the mouse out of the surface.
	void onMouseOut(ActionHandler handler);
	/// Hooks an action handler to pressing a key when the surface is focused.
	void onKeyboardPress(ActionHandler handler, SDLKey key = SDLK_ANY);
	/// Hooks an action handler to releasing a key when the surface is focused.
	void onKeyboardRelease(ActionHandler handler, SDLKey key = SDLK_ANY);
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
	/// Check this surface to see if it's a textlist button.
	void setListButton();
};

}
