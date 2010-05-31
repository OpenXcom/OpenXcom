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
#ifndef OPENXCOM__BUTTON_H
#define OPENXCOM__BUTTON_H

#include <cmath>
#include "SDL.h"
#include "State_Interactive.h"
#include "Text.h"

/**
 * Coloured labelled button.
 * Drawn to look like a 3D-shaped box with text on top,
 * responds to mouse clicks. Can be attached to a group of
 * buttons to turn it into a radio button (only one button
 * pushed at a time). Pretty much the heart of the interface.
 */
class Button : public InteractiveSurface
{
private:
	Uint8 _color;
	Text *_text;
	Button **_group;
public:
	/// Creates a new button with the specified size and position.
	Button(Font *big, Font *small, int width, int height, int x = 0, int y = 0);
	/// Cleans up the button.
	~Button();
	/// Sets the button's color.
	void setColor(Uint8 color);
	/// Gets the button's color.
	Uint8 getColor();
	/// Sets the button's text.
	void setText(string text);
	/// Gets the button's text.
	string getText();
	/// Sets the button's group.
	void setGroup(Button **group);
	/// Sets the button's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Draws the button.
	void draw();
	/// Handles mouse events.
	void handle(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse presses.
	void mousePress(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(SDL_Event *ev, int scale, State *state);
};

#endif