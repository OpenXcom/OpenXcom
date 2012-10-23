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
#include "InteractiveSurface.h"
#include "Action.h"

namespace OpenXcom
{

/**
 * Sets up a blank interactive surface with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
InteractiveSurface::InteractiveSurface(int width, int height, int x, int y) : Surface(width, height, x, y), _press(0), _release(0), _in(0), _over(0), _out(0), _keyPress(0), _keyRelease(0), _isHovered(false), _isFocused(false)
{
	_clicks = new ActionHandler[NUM_BUTTONS+1];
	_buttonsPressed = new bool[NUM_BUTTONS+1];
	for (int i = 0; i <= NUM_BUTTONS; ++i)
	{
		_clicks[i] = 0;
		_buttonsPressed[i] = false;
	}
}

/**
 *
 */
InteractiveSurface::~InteractiveSurface()
{
	delete[] _clicks;
	delete[] _buttonsPressed;
}

bool InteractiveSurface::isButtonPressed()
{
	for (int i = 0; i <= NUM_BUTTONS; ++i)
	{
		if (_buttonsPressed[i])
			return true;
	}
	return false;
}

/**
 * Changes the visibility of the surface. A hidden surface
 * isn't blitted nor receives events.
 * @param visible New visibility.
 */
void InteractiveSurface::setVisible(bool visible)
{
	Surface::setVisible(visible);
	// Unpress button if it was hidden
	if (!_visible)
	{
		unpress(0);
	}
}

/**
 * Called whenever an action occurs, and processes it to
 * check if it's relevant to the surface and convert it
 * into a meaningful interaction like a "click", calling
 * the respective handlers.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void InteractiveSurface::handle(Action *action, State *state)
{
	if (!_visible || _hidden)
		return;

	action->setSender(this);

	if (action->getDetails()->type == SDL_MOUSEBUTTONUP || action->getDetails()->type == SDL_MOUSEBUTTONDOWN)
	{
		action->setMouseAction(action->getDetails()->button.x, action->getDetails()->button.y, getX(), getY());
	}
	else if (action->getDetails()->type == SDL_MOUSEMOTION)
	{
		action->setMouseAction(action->getDetails()->motion.x, action->getDetails()->motion.y, getX(), getY());
	}

	if (action->isMouseAction())
	{
		if ((action->getAbsoluteXMouse() >= getX() && action->getAbsoluteXMouse() < getX() + getWidth()) &&
			(action->getAbsoluteYMouse() >= getY() && action->getAbsoluteYMouse() < getY() + getHeight()))
		{
			if (!_isHovered)
			{
				_isHovered = true;
				mouseIn(action, state);
			}
			mouseOver(action, state);
		}
		else
		{
			if (_isHovered)
			{
				_isHovered = false;
				mouseOut(action, state);
			}
		}
	}

	if (action->getDetails()->type == SDL_MOUSEBUTTONDOWN)
	{
		if (_isHovered && !_buttonsPressed[action->getDetails()->button.button])
		{
			_buttonsPressed[action->getDetails()->button.button] = true;
			mousePress(action, state);
		}
	}
	else if (action->getDetails()->type == SDL_MOUSEBUTTONUP)
	{
		if (_buttonsPressed[action->getDetails()->button.button])
		{
			_buttonsPressed[action->getDetails()->button.button] = false;
			mouseRelease(action, state);
			if (_isHovered)
			{
				mouseClick(action, state);
			}
		}
	}

	if (_isFocused)
	{
		if (action->getDetails()->type == SDL_KEYDOWN)
		{
			keyboardPress(action, state);
		}
		else if (action->getDetails()->type == SDL_KEYUP)
		{
			keyboardRelease(action, state);
		}
	}
}

/**
 * Marks ths surface as focused. Surfaces will only receive
 * keyboard events if focused.
 */
void InteractiveSurface::focus()
{
	_isFocused = true;
}

/**
 * Simulates a "mouse button release". Used in circumstances
 * where the surface is unpressed without user input.
 * @param state Pointer to running state.
 */
void InteractiveSurface::unpress(State *state)
{
	if (isButtonPressed())
	{
		for (int i = 0; i <= NUM_BUTTONS; ++i)
		{
			_buttonsPressed[i] = false;
		}
		SDL_Event ev;
		ev.type = SDL_MOUSEBUTTONUP;
		ev.button.button = SDL_BUTTON_LEFT;
		Action a = Action(&ev, 0.0, 0.0);
		mouseRelease(&a, state);
	}
}

/**
 * Called every time there's a mouse press over the surface.
 * Allows the surface to have custom functionality for this action,
 * and can be called externally to simulate the action.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void InteractiveSurface::mousePress(Action *action, State *state)
{
	if (_press != 0)
	{
		(state->*_press)(action);
	}
}

/**
 * Called every time there's a mouse release over the surface.
 * Allows the surface to have custom functionality for this action,
 * and can be called externally to simulate the action.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void InteractiveSurface::mouseRelease(Action *action, State *state)
{
	if (_release != 0)
	{
		(state->*_release)(action);
	}
}

/**
 * Called every time there's a mouse click on the surface.
 * Allows the surface to have custom functionality for this action,
 * and can be called externally to simulate the action.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void InteractiveSurface::mouseClick(Action *action, State *state)
{
	if (_clicks[0] != 0)
	{
		(state->*_clicks[0])(action);
	}
	if (_clicks[action->getDetails()->button.button] != 0)
	{
		(state->*_clicks[action->getDetails()->button.button])(action);
	}
}

/**
 * Called every time the mouse moves into the surface.
 * Allows the surface to have custom functionality for this action,
 * and can be called externally to simulate the action.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void InteractiveSurface::mouseIn(Action *action, State *state)
{
	if (_in != 0)
	{
		(state->*_in)(action);
	}
}

/**
 * Called every time the mouse moves over the surface.
 * Allows the surface to have custom functionality for this action,
 * and can be called externally to simulate the action.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void InteractiveSurface::mouseOver(Action *action, State *state)
{
	if (_over != 0)
	{
		(state->*_over)(action);
	}
}

/**
 * Called every time the mouse moves out of the surface.
 * Allows the surface to have custom functionality for this action,
 * and can be called externally to simulate the action.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void InteractiveSurface::mouseOut(Action *action, State *state)
{
	if (_out != 0)
	{
		(state->*_out)(action);
	}
}

/**
 * Called every time there's a keyboard press when the surface is focused.
 * Allows the surface to have custom functionality for this action,
 * and can be called externally to simulate the action.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void InteractiveSurface::keyboardPress(Action *action, State *state)
{
	if (_keyPress != 0)
	{
		(state->*_keyPress)(action);
	}
}

/**
 * Called every time there's a keyboard release over the surface.
 * Allows the surface to have custom functionality for this action,
 * and can be called externally to simulate the action.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void InteractiveSurface::keyboardRelease(Action *action, State *state)
{
	if (_keyRelease != 0)
	{
		(state->*_keyRelease)(action);
	}
}

/**
 * Sets a function to be called every time the surface is mouse clicked.
 * @param handler Action handler.
 * @param button Mouse button to check for. Set to 0 for any button.
 */
void InteractiveSurface::onMouseClick(ActionHandler handler, Uint8 button)
{
	_clicks[button] = handler;
}

/**
 * Sets a function to be called every time the surface is mouse pressed.
 * @param handler Action handler.
 */
void InteractiveSurface::onMousePress(ActionHandler handler)
{
	_press = handler;
}

/**
 * Sets a function to be called every time the surface is mouse released.
 * @param handler Action handler.
 */
void InteractiveSurface::onMouseRelease(ActionHandler handler)
{
	_release = handler;
}

/**
 * Sets a function to be called every time the mouse moves into the surface.
 * @param handler Action handler.
 */
void InteractiveSurface::onMouseIn(ActionHandler handler)
{
	_in = handler;
}

/**
 * Sets a function to be called every time the mouse moves over the surface.
 * @param handler Action handler.
 */
void InteractiveSurface::onMouseOver(ActionHandler handler)
{
	_over = handler;
}

/**
 * Sets a function to be called every time the mouse moves out of the surface.
 * @param handler Action handler.
 */
void InteractiveSurface::onMouseOut(ActionHandler handler)
{
	_out = handler;
}

/**
 * Sets a function to be called every time a key is pressed when the surface is focused.
 * @param handler Action handler.
 */
void InteractiveSurface::onKeyboardPress(ActionHandler handler)
{
	_keyPress = handler;
}

/**
 * Sets a function to be called every time a key is released when the surface is focused.
 * @param handler Action handler.
 */
void InteractiveSurface::onKeyboardRelease(ActionHandler handler)
{
	_keyRelease = handler;
}

}
