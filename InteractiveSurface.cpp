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
#include "InteractiveSurface.h"

/**
 * Sets up a blank interactive surface with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
InteractiveSurface::InteractiveSurface(int width, int height, int x, int y) : Surface(width, height, x, y), _click(0), _press(0), _release(0), _in(0), _over(0), _out(0), _keyPress(0), _keyRelease(0), _isPressed(false), _isHovered(false), _isFocused(false)
{

}

/**
 *
 */
InteractiveSurface::~InteractiveSurface()
{
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
	if (!_visible && _isPressed)
	{
		SDL_Event ev;
		ev.button.button = SDL_BUTTON_LEFT;
		_isPressed = false;
		mouseRelease(&ev, 0, 0);
	}
}

/**
 * Called whenever an SDL_event occurs, and processes it to
 * check if it's relevant to the surface and convert it into
 * a meaningful interaction like a "click", calling the respective
 * handlers.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void InteractiveSurface::handle(SDL_Event *ev, int scale, State *state)
{
	if (!_visible)
		return;

	if ((ev->motion.x >= _x * scale && ev->motion.x < (_x + _width) * scale) &&
		(ev->motion.y >= _y * scale && ev->motion.y < (_y + _height) * scale))
	{
		if (!_isHovered)
		{
			_isHovered = true;
			mouseIn(ev, scale, state);
		}
		mouseOver(ev, scale, state);
	}
	else
	{
		if (_isHovered)
		{
			_isHovered = false;
			mouseOut(ev, scale, state);
		}
	}

	if (!_isPressed && ev->type == SDL_MOUSEBUTTONDOWN)
	{
		if (_isHovered)
		{
			_isPressed = true;
			mousePress(ev, scale, state);
		}
	}
	else if (_isPressed && ev->type == SDL_MOUSEBUTTONUP)
	{
		_isPressed = false;
		mouseRelease(ev, scale, state);
		if (_isHovered)
		{
			mouseClick(ev, scale, state);
		}
	}

	if (_isFocused)
	{
		if (ev->type == SDL_KEYDOWN)
		{
			keyboardPress(ev, scale, state);
		}
		else if (ev->type == SDL_KEYUP)
		{
			keyboardRelease(ev, scale, state);
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
 * Called everytime there's a mouse press over the surface.
 * Allows the surface to have custom functionality for this event,
 * and can be called externally to simulate the event.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void InteractiveSurface::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (_press != 0)
		(state->*_press)(ev, scale);
}

/**
 * Called everytime there's a mouse release over the surface.
 * Allows the surface to have custom functionality for this event,
 * and can be called externally to simulate the event.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void InteractiveSurface::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (_release != 0)
		(state->*_release)(ev, scale);
}

/**
 * Called everytime there's a mouse click on the surface.
 * Allows the surface to have custom functionality for this event,
 * and can be called externally to simulate the event.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void InteractiveSurface::mouseClick(SDL_Event *ev, int scale, State *state)
{
	if (_click != 0)
		(state->*_click)(ev, scale);
}

/**
 * Called everytime the mouse moves into the surface.
 * Allows the surface to have custom functionality for this event,
 * and can be called externally to simulate the event.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void InteractiveSurface::mouseIn(SDL_Event *ev, int scale, State *state)
{
	if (_in != 0)
		(state->*_in)(ev, scale);
}

/**
 * Called everytime the mouse moves over the surface.
 * Allows the surface to have custom functionality for this event,
 * and can be called externally to simulate the event.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void InteractiveSurface::mouseOver(SDL_Event *ev, int scale, State *state)
{
	if (_over != 0)
		(state->*_over)(ev, scale);
}

/**
 * Called everytime the mouse moves out of the surface.
 * Allows the surface to have custom functionality for this event,
 * and can be called externally to simulate the event.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void InteractiveSurface::mouseOut(SDL_Event *ev, int scale, State *state)
{
	if (_out != 0)
		(state->*_out)(ev, scale);
}

/**
 * Called everytime there's a keyboard press when the surface is focused.
 * Allows the surface to have custom functionality for this event,
 * and can be called externally to simulate the event.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void InteractiveSurface::keyboardPress(SDL_Event *ev, int scale, State *state)
{
	if (_keyPress != 0)
		(state->*_keyPress)(ev, scale);
}

/**
 * Called everytime there's a keyboard release over the surface.
 * Allows the surface to have custom functionality for this event,
 * and can be called externally to simulate the event.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void InteractiveSurface::keyboardRelease(SDL_Event *ev, int scale, State *state)
{
	if (_keyRelease != 0)
		(state->*_keyRelease)(ev, scale);
}

/**
 * Sets a function to be called everytime the surface is mouse clicked.
 * @param handler Event handler.
 */
void InteractiveSurface::onMouseClick(EventHandler handler)
{
	_click = handler;
}

/**
 * Sets a function to be called everytime the surface is mouse pressed.
 * @param handler Event handler.
 */
void InteractiveSurface::onMousePress(EventHandler handler)
{
	_press = handler;
}

/**
 * Sets a function to be called everytime the surface is mouse released.
 * @param handler Event handler.
 */
void InteractiveSurface::onMouseRelease(EventHandler handler)
{
	_release = handler;
}

/**
 * Sets a function to be called everytime the mouse moves into the surface.
 * @param handler Event handler.
 */
void InteractiveSurface::onMouseIn(EventHandler handler)
{
	_in = handler;
}

/**
 * Sets a function to be called everytime the mouse moves over the surface.
 * @param handler Event handler.
 */
void InteractiveSurface::onMouseOver(EventHandler handler)
{
	_over = handler;
}

/**
 * Sets a function to be called everytime the mouse moves out of the surface.
 * @param handler Event handler.
 */
void InteractiveSurface::onMouseOut(EventHandler handler)
{
	_out = handler;
}

/**
 * Sets a function to be called everytime a key is pressed when the surface is focused.
 * @param handler Event handler.
 */
void InteractiveSurface::onKeyboardPress(EventHandler handler)
{
	_keyPress = handler;
}

/**
 * Sets a function to be called everytime a key is released when the surface is focused.
 * @param handler Event handler.
 */
void InteractiveSurface::onKeyboardRelease(EventHandler handler)
{
	_keyRelease = handler;
}
