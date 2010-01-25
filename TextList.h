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
#ifndef __TEXTLIST_H__
#define __TEXTLIST_H__

#include <vector>
#include <cstdarg>
#include "Surface.h"
#include "Text.h"

class TextList : public Surface
{
private:
	vector<vector<Text*>> _texts;
	vector<int> _columns;
	Font *_big, *_small;
	int _rowY;
	Uint8 _color;

public:
	TextList(Font *big, Font *small, int width, int height, int x = 0, int y = 0);
	~TextList();
	Text* getCell(int row, int col);
	void addRow(int num, ...);
	void setColumns(int num, ...);
	void setColor(Uint8 color);
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	void blit(Surface *surface);
};

#endif