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
#include "InteractiveSurface.h"
#include "Action.h"

namespace OpenXcom
{

const SDLKey InteractiveSurface::SDLK_ANY = (SDLKey)-1; // using an unused keycode to represent an "any key"

/**
 * Sets up a blank interactive surface with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
InteractiveSurface::InteractiveSurface(int width, int height, int x, int y) : Surface(width, height, x, y), _buttonsPressed(0), _in(0), _over(0), _out(0), _isHovered(false), _isFocused(true), _listButton(false)
{
}

/**
 *
 */
InteractiveSurface::~InteractiveSurface()
{
}

bool InteractiveSurface::isButtonHandled(Uint8 button)
{
	bool handled = (_click.find(0) != _click.end() ||
					_press.find(0) != _press.end() ||
					_release.find(0) != _release.end());
	if (!handled && button != 0)
	{
		handled = (_click.find(button) != _click.end() ||
				   _press.find(button) != _press.end() ||
				   _release.find(button) != _release.end());
	}
	return handled;
}

bool InteractiveSurface::isButtonPressed(Uint8 button) const
{
	if (button == 0)
	{
		return (_buttonsPressed != 0);
	}
	else
	{
		return (_buttonsPressed & SDL_BUTTON(button)) != 0;
	}
}

void InteractiveSurface::setButtonPressed(Uint8 button, bool pressed)
{
	if (pressed)
	{
		_buttonsPressed |= SDL_BUTTON(button);
	}
	else
	{
		_buttonsPressed &= (~SDL_BUTTON(button));
	}
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
			if (_listButton && action->getDetails()->type == SDL_MOUSEMOTION)
			{
				_buttonsPressed = SDL_GetMouseState(0, 0);
				for (Uint8 i = 1; i <= NUM_BUTTONS; ++i)
				{
					if (isButtonPressed(i))
					{
						action->getDetails()->button.button = i;
						mousePress(action, state);
					}
				}
			}
			mouseOver(action, state);
		}
		else
		{
			if (_isHovered)
			{
				_isHovered = false;
				mouseOut(action, state);
				if (_listButton && action->getDetails()->type == SDL_MOUSEMOTION)
				{
					for (Uint8 i = 1; i <= NUM_BUTTONS; ++i)
					{
						if (isButtonPressed(i))
						{
							setButtonPressed(i, false);
						}
						action->getDetails()->button.button = i;
						mouseRelease(action, state);
					}
				}
			}
		}
	}

	if (action->getDetails()->type == SDL_MOUSEBUTTONDOWN)
	{
		if (_isHovered && !isButtonPressed(action->getDetails()->button.button))
		{
			setButtonPressed(action->getDetails()->button.button, true);
			mousePress(action, state);
		}
	}
	else if (action->getDetails()->type == SDL_MOUSEBUTTONUP)
	{
		if (isButtonPressed(action->getDetails()->button.button))
		{
			setButtonPressed(action->getDetails()->button.button, false);
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
 * Changes the surface's focus. Surfaces will only receive
 * keyboard events if focused.
 * @param focus Is it focused?
 */
void InteractiveSurface::setFocus(bool focus)
{
	_isFocused = focus;
}

/**
 * Returns the surface's focus. Surfaces will only receive
 * keyboard events if focused.
 * @return Is it focused?
 */
bool InteractiveSurface::isFocused() const
{
	return _isFocused;
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
		_buttonsPressed = 0;
		SDL_Event ev;
		ev.type = SDL_MOUSEBUTTONUP;
		ev.button.button = SDL_BUTTON_LEFT;
		Action a = Action(&ev, 0.0, 0.0, 0, 0);
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
	std::map<Uint8, ActionHandler>::iterator allHandler = _press.find(0);
	std::map<Uint8, ActionHandler>::iterator oneHandler = _press.find(action->getDetails()->button.button);
	if (allHandler != _press.end())
	{
		ActionHandler handler = allHandler->second;
		(state->*handler)(action);
	}
	if (oneHandler != _press.end())
	{
		ActionHandler handler = oneHandler->second;
		(state->*handler)(action);
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
	std::map<Uint8, ActionHandler>::iterator allHandler = _release.find(0);
	std::map<Uint8, ActionHandler>::iterator oneHandler = _release.find(action->getDetails()->button.button);
	if (allHandler != _release.end())
	{
		ActionHandler handler = allHandler->second;
		(state->*handler)(action);
	}
	if (oneHandler != _release.end())
	{
		ActionHandler handler = oneHandler->second;
		(state->*handler)(action);
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
	std::map<Uint8, ActionHandler>::iterator allHandler = _click.find(0);
	std::map<Uint8, ActionHandler>::iterator oneHandler = _click.find(action->getDetails()->button.button);
	if (allHandler != _click.end())
	{
		ActionHandler handler = allHandler->second;
		(state->*handler)(action);
	}
	if (oneHandler != _click.end())
	{
		ActionHandler handler = oneHandler->second;
		(state->*handler)(action);
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
	std::map<SDLKey, ActionHandler>::iterator allHandler = _keyPress.find(SDLK_ANY);
	std::map<SDLKey, ActionHandler>::iterator oneHandler = _keyPress.find(action->getDetails()->key.keysym.sym);
	if (allHandler != _keyPress.end())
	{
		ActionHandler handler = allHandler->second;
		(state->*handler)(action);
	}
	// Check if Ctrl, Alt and Shift aren't pressed
	bool mod = ((action->getDetails()->key.keysym.mod & (KMOD_CTRL|KMOD_ALT|KMOD_SHIFT)) != 0);
	if (oneHandler != _keyPress.end() && !mod)
	{
		ActionHandler handler = oneHandler->second;
		(state->*handler)(action);
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
	std::map<SDLKey, ActionHandler>::iterator allHandler = _keyRelease.find(SDLK_ANY);
	std::map<SDLKey, ActionHandler>::iterator oneHandler = _keyRelease.find(action->getDetails()->key.keysym.sym);
	if (allHandler != _keyRelease.end())
	{
		ActionHandler handler = allHandler->second;
		(state->*handler)(action);
	}
	// Check if Ctrl, Alt and Shift aren't pressed
	bool mod = ((action->getDetails()->key.keysym.mod & (KMOD_CTRL|KMOD_ALT|KMOD_SHIFT)) != 0);
	if (oneHandler != _keyRelease.end() && !mod)
	{
		ActionHandler handler = oneHandler->second;
		(state->*handler)(action);
	}
}

/**
 * Sets a function to be called every time the surface is mouse clicked.
 * @param handler Action handler.
 * @param button Mouse button to check for. Set to 0 for any button.
 */
void InteractiveSurface::onMouseClick(ActionHandler handler, Uint8 button)
{
	if (handler != 0)
	{
		_click[button] = handler;
	}
	else
	{
		_click.erase(button);
	}
}

/**
 * Sets a function to be called every time the surface is mouse pressed.
 * @param handler Action handler.
 * @param button Mouse button to check for. Set to 0 for any button.
 */
void InteractiveSurface::onMousePress(ActionHandler handler, Uint8 button)
{
	if (handler != 0)
	{
		_press[button] = handler;
	}
	else
	{
		_press.erase(button);
	}
}

/**
 * Sets a function to be called every time the surface is mouse released.
 * @param handler Action handler.
 * @param button Mouse button to check for. Set to 0 for any button.
 */
void InteractiveSurface::onMouseRelease(ActionHandler handler, Uint8 button)
{
	if (handler != 0)
	{
		_release[button] = handler;
	}
	else
	{
		_release.erase(button);
	}
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
 * @param key Keyboard button to check for (note: ignores key modifiers). Set to 0 for any key.
 */
void InteractiveSurface::onKeyboardPress(ActionHandler handler, SDLKey key)
{
	if (handler != 0)
	{
		_keyPress[key] = handler;
	}
	else
	{
		_keyPress.erase(key);
	}
}

/**
 * Sets a function to be called every time a key is released when the surface is focused.
 * @param handler Action handler.
 * @param key Keyboard button to check for (note: ignores key modifiers). Set to 0 for any key.
 */
void InteractiveSurface::onKeyboardRelease(ActionHandler handler, SDLKey key)
{
	if (handler != 0)
	{
		_keyRelease[key] = handler;
	}
	else
	{
		_keyRelease.erase(key);
	}
}

/**
 * Sets a flag for this button to say "i'm a member of a textList" to true.
 */
void InteractiveSurface::setListButton()
{
	_listButton = true;
}

}
