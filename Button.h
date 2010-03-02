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

class Button : public InteractiveSurface
{
private:
	Uint8 _color;
	Text *_text;
	Button **_group;

	void mousePress(SDL_Event *ev, int scale, State *state);
	void mouseRelease(SDL_Event *ev, int scale, State *state);
public:
	Button(Font *big, Font *small, int width, int height, int x = 0, int y = 0);
	~Button();
	void setColor(Uint8 color);
	Uint8 getColor();
	void setText(string text);
	string getText();
	void setGroup(Button **group);
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	void blit(Surface *surface);
	void handle(SDL_Event *ev, int scale, State *state);
};

#endif