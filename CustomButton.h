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
#ifndef OPENXCOM__CUSTOMBUTTON_H
#define OPENXCOM__CUSTOMBUTTON_H

#include "SDL.h"
#include "State_Interactive.h"

/**
 * Regular graphic that works like a button.
 * Unlike the regular Button, this button doesn't draw
 * anything on its own. It takes an existing graphic and
 * treats it as a button, inverting colors when necessary.
 * This is necessary for special buttons like in the Geoscape.
 */
class CustomButton : public InteractiveSurface
{
private:
	Uint8 _color;
	CustomButton **_group;
public:
	/// Creates a new custom button with the specified size and position.
	CustomButton(int width, int height, int x = 0, int y = 0);
	/// Cleans up the custom button.
	~CustomButton();
	/// Sets the custom button's color.
	void setColor(Uint8 color);
	/// Gets the custom button's color.
	Uint8 getColor();
	/// Sets the custom button's group.
	void setGroup(CustomButton **group);
	/// Blits the custom button onto a surface.
	void blit(Surface *surface);
	/// Handles mouse events.
	void handle(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse presses.
	void mousePress(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(SDL_Event *ev, int scale, State *state);
};

#endif