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

#ifndef __OXC_TOGGLETEXTBUTTON_H
#define __OXC_TOGGLETEXTBUTTON_H


#include "TextButton.h"
#include "../Engine/Action.h"
#include "../Engine/State.h"

namespace OpenXcom
{

class ToggleTextButton :
    public TextButton
{
private:
    bool _isPressed;
    int _invertMid;
    TextButton *_fakeGroup;

public:

    void draw();
    void mousePress(Action *action, State *state);
    void setPressed(bool pressed);
    bool getPressed() const { return _isPressed; }
    void setInvertColor(Uint8 mid);
    ToggleTextButton(int width, int height, int x, int y);
    ~ToggleTextButton(void);
};

}

#endif