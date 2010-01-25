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
#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "SDL.h"
#include "Surface.h"
#include "State.h"
#include "Text.h"

enum ButtonState { STATE_NORMAL, STATE_PRESSED };

typedef State &(State::*EventHandler)(SDL_Event *);

class Button : public Surface
{
private:
	Uint8 _color;
	ButtonState _state;
	Text *_text;
	EventHandler _click, _press, _release;

public:
	Button(Font *big, Font *small, int width, int height, int x = 0, int y = 0);
	~Button();
	void handle(SDL_Event *ev, int scale, State *state);
	void setColor(Uint8 color);
	Uint8 getColor();
	void setText(string text);
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	void blit(Surface *surface);
	void onClick(EventHandler handler);
	void onPress(EventHandler handler);
	void onRelease(EventHandler handler);
};

#endif