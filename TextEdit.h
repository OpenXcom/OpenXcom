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
#ifndef OPENXCOM__TEXTEDIT_H
#define OPENXCOM__TEXTEDIT_H

#include "SDL.h"
#include "State_Interactive.h"
#include "Text.h"

class TextEdit : public InteractiveSurface
{
private:
	Text *_text;
	string _value;

	void mousePress(SDL_Event *ev, int scale, State *state);
	void mouseRelease(SDL_Event *ev, int scale, State *state);
	void keyboardPress(SDL_Event *ev, int scale, State *state);
	void keyboardRelease(SDL_Event *ev, int scale, State *state);
public:
	TextEdit(Font *big, Font *small, int width, int height, int x = 0, int y = 0);
	~TextEdit();
	void setBig();
	void setSmall();
	void setText(string text);
	string getText();
	void setWordWrap(bool wrap);
	void setInvert(bool invert);
	void setAlign(TextAlign align);
	void setColor(Uint8 color);
	Uint8 getColor();
	void focus();
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	void blit(Surface *surface);
	void handle(SDL_Event *ev, int scale, State *state);
};

#endif