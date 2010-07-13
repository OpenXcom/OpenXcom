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

#include "InteractiveSurface.h"
#include "Text.h"

class Font;

/**
 * Editable version of Text.
 * Receives keyboard input to allow the player
 * to change the text himself.
 */
class TextEdit : public InteractiveSurface
{
private:
	Text *_text;
	string _value;
	int _i;
public:
	/// Creates a new text with the specified size, position and fonts.
	TextEdit(Font *big, Font *small, int width, int height, int x = 0, int y = 0);
	/// Cleans up the text.
	~TextEdit();
	/// Sets the text size to big.
	void setBig();
	/// Sets the text size to small.
	void setSmall();
	/// Sets the text's string.
	void setText(string text);
	/// Gets the text's string.
	string getText();
	/// Sets the text's wordwrap setting.
	void setWordWrap(bool wrap);
	/// Sets the text's color invert setting.
	void setInvert(bool invert);
	/// Sets the text's alignment.
	void setAlign(TextAlign align);
	/// Sets the text's color.
	void setColor(Uint8 color);
	/// Gets the text's color.
	Uint8 getColor();
	/// Sets the surface's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Blits the text onto another surface.
	void blit(Surface *surface);
	/// Special handling for mouse presses.
	void mousePress(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse clicks.
	void mouseClick(SDL_Event *ev, int scale, State *state);
	/// Special handling for keyboard presses.
	void keyboardPress(SDL_Event *ev, int scale, State *state);
};

#endif
