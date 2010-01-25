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
#ifndef __TEXT_H__
#define __TEXT_H__

#include "Font.h"
#include "Surface.h"
#include <string>

enum TextAlign { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT };

class Text : public Surface
{
private:
	Font *_big, *_small, *_font;
	string _text;
	bool _wrap, _invert;
	TextAlign _align;
	Uint8 _color;
public:
	Text(Font *big, Font *small, int width, int height, int x = 0, int y = 0);
	~Text();
	void setBig();
	void setSmall();
	void setText(string text);
	void setWordWrap(bool wrap);
	void setInvert(bool invert);
	void setAlign(TextAlign align);
	void setColor(Uint8 color);
	Uint8 getColor();
	void blit(Surface *surface);
};

#endif