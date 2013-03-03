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

#include "ToggleTextButton.h"
#include "TextButton.h"
#include "../Engine/Sound.h"


namespace OpenXcom
{

ToggleTextButton::ToggleTextButton(int width, int height, int x, int y) : TextButton(width, height, x, y), _fakeGroup(0)
{
    isPressed = false;
    TextButton::setGroup(&_fakeGroup);
}


ToggleTextButton::~ToggleTextButton(void)
{
}

/// handle mouse clicks by toggling the button state; use _fakeGroup to trick TextButton into drawing the right thing
void ToggleTextButton::mousePress(Action *action, State *state)
{
    if (soundPress != 0)
        soundPress->play();

    if (action->getDetails()->button.button == SDL_BUTTON_LEFT || action->getDetails()->button.button == SDL_BUTTON_RIGHT)
    {
        isPressed = !isPressed;
        _fakeGroup = isPressed ? this : 0;
    }

    InteractiveSurface::unpress(state);
    draw();
}

/// set the isPressed state of the button and force it to redraw
void ToggleTextButton::setPressed(bool pressed)
{
    isPressed = pressed;
    _fakeGroup = isPressed ? this : 0;
    _redraw = true;
}

}
