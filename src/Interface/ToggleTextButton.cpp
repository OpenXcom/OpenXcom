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

#include "ToggleTextButton.h"
#include "TextButton.h"


namespace OpenXcom
{

ToggleTextButton::ToggleTextButton(int width, int height, int x, int y) : TextButton(width, height, x, y), _originalColor(-1), _invertedColor(-1), _fakeGroup(0)
{
	_isPressed = false;
	TextButton::setGroup(&_fakeGroup);
}


ToggleTextButton::~ToggleTextButton(void)
{
}

/// handle mouse clicks by toggling the button state; use _fakeGroup to trick TextButton into drawing the right thing
void ToggleTextButton::mousePress(Action *action, State *state)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT || action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		_isPressed = !_isPressed;
		_fakeGroup = _isPressed ? this : 0; // this is the trick that makes TextButton stick
		if (_isPressed && _invertedColor > -1) TextButton::setColor(_invertedColor);
		else TextButton::setColor(_originalColor);
	}

	InteractiveSurface::mousePress(action, state); // skip TextButton's code as it will try to set *_group
	draw();
}

/// set the _isPressed state of the button and force it to redraw
void ToggleTextButton::setPressed(bool pressed)
{
	_isPressed = pressed;
	_fakeGroup = _isPressed ? this : 0;
	if (_isPressed && _invertedColor > -1) TextButton::setColor(_invertedColor);
	else TextButton::setColor(_originalColor);
	_redraw = true;
}

void ToggleTextButton::setColor(Uint8 color)
{
	_originalColor = color;
	TextButton::setColor(color);
}

/// When this is set, Surface::invert() is called with the value from mid when it's time to invert the button
void ToggleTextButton::setInvertColor(Uint8 color)
{
	_invertedColor = color;
	_fakeGroup = 0;
	_redraw = true;
}

/// handle draw() in case we need to paint the button a garish color
void ToggleTextButton::draw()
{
	if (_invertedColor > -1) _fakeGroup = 0; // nevermind, TextButton. We'll invert the surface ourselves.
	TextButton::draw();

	if (_invertedColor > -1 && _isPressed)
	{
		this->invert(_invertedColor + 4);
	}
}




}
