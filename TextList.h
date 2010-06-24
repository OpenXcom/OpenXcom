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
#ifndef OPENXCOM__TEXTLIST_H
#define OPENXCOM__TEXTLIST_H

#include <vector>
#include <cstdarg>
#include <cmath>
#include "State_Interactive.h"
#include "Text.h"

/**
 * List of Text's split into columns.
 * Contains a set of Text's that are automatically lined up
 * by rows and columns, like a big table, making it easy to
 * manage them together.
 */
class TextList : public InteractiveSurface
{
private:
	vector< vector<Text*> > _texts;
	vector<int> _columns;
	Font *_big, *_small;
	int _rowY;
	Uint8 _color;
	bool _dot, _selectable;
	unsigned int _selRow;
	Surface *_bg, *_selector;
public:
	/// Creates a text list with the specified size and position.
	TextList(Font *big, Font *small, int width, int height, int x = 0, int y = 0);
	/// Cleans up the text list.
	~TextList();
	/// Gets a certain cell in the text list.
	Text* getCell(int row, int col);
	/// Adds a new row to the text list.
	void addRow(int num, ...);
	/// Sets the number of columns in the text list.
	void setColumns(int num, ...);
	/// Sets the text color of the text list.
	void setColor(Uint8 color);
	/// Sets whether to separate columns with dots.
	void setDot(bool dot);
	/// Sets whether the list is selectable.
	void setSelectable(bool selectable);
	/// Sets the background for the selector.
	void setBackground(Surface *bg);
	/// Gets the selected row in the list.
	int getSelectedRow();
	/// Clears the list.
	void clearList();
	/// Sets the palette of the text list.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Draws the text onto the text list.
	void draw();
	/// Special handling for mouse presses.
	void mousePress(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse clicks.
	void mouseClick(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse hovering.
	void mouseOver(SDL_Event *ev, int scale, State *state);
	/// Special handling for mouse hovering out.
	void mouseOut(SDL_Event *ev, int scale, State *state);
};

#endif