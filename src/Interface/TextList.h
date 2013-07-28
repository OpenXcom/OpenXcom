/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifndef OPENXCOM_TEXTLIST_H
#define OPENXCOM_TEXTLIST_H

#include <vector>
#include <map>
#include "../Engine/InteractiveSurface.h"
#include "Text.h"

namespace OpenXcom
{

enum ArrowOrientation { ARROW_VERTICAL, ARROW_HORIZONTAL };

class Font;
class ArrowButton;

/**
 * List of Text's split into columns.
 * Contains a set of Text's that are automatically lined up by
 * rows and columns, like a big table, making it easy to manage
 * them together.
 */
class TextList : public InteractiveSurface
{
private:
	std::vector< std::vector<Text*> > _texts;
	std::vector<int> _columns;
	Font *_big, *_small, *_font;
	unsigned int _scroll, _visibleRows;
	Uint8 _color, _color2;
	std::map<int, TextHAlign> _align;
	bool _dot, _selectable, _condensed, _contrast;
	unsigned int _selRow;
	Surface *_bg, *_selector;
	ArrowButton *_up, *_down;
	int _margin;
	bool _scrolling;
	std::vector<ArrowButton*> _arrowLeft, _arrowRight;
	int _arrowPos, _scrollPos;
	ArrowOrientation _arrowType;
	ActionHandler _leftClick, _leftPress, _leftRelease, _rightClick, _rightPress, _rightRelease;
	bool _allowScrollOnArrowButtons;
	int _arrowsLeftEdge, _arrowsRightEdge;

	/// Updates the arrow buttons.
	void updateArrows();
public:
	/// Creates a text list with the specified size and position.
	TextList(int width, int height, int x = 0, int y = 0);
	/// Cleans up the text list.
	~TextList();
	/// Sets the allowScrollOnArrowButtons.
	void setAllowScrollOnArrowButtons(bool value);
	/// Gets the arrowsLeftEdge.
	int getArrowsLeftEdge();
	/// Gets the arrowsRightEdge.
	int getArrowsRightEdge();
	/// Unpresses the surface.
	void unpress(State *state);
	/// Sets the text color of a certain cell.
	void setCellColor(int row, int column, Uint8 color);
	/// Sets the text color of a certain row.
	void setRowColor(int row, Uint8 color);
	/// Gets the text of a certain cell.
	std::wstring getCellText(int row, int column) const;
	/// Sets the text of a certain cell.
	void setCellText(int row, int column, const std::wstring &text);
	/// Gets the X position of a certain column.
	int getColumnX(int column) const;
	/// Gets the Y position of a certain row.
	int getRowY(int row) const;
	/// Adds a new row to the text list.
	void addRow(int cols, ...);
	/// Sets the columns in the text list.
	void setColumns(int cols, ...);
	/// Sets the palette of the text list.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Sets the fonts of the text list.
	void setFonts(Font *big, Font *small);
	/// Sets the text color of the text list.
	void setColor(Uint8 color);
	/// Gets the text color of the text list.
	Uint8 getColor() const;
	/// Sets the secondary color of the text list.
	void setSecondaryColor(Uint8 color);
	/// Gets the secondary color of the text list.
	Uint8 getSecondaryColor() const;
	/// Sets the text list's high contrast color setting.
	void setHighContrast(bool contrast);
	/// Sets the text horizontal alignment of the text list.
	void setAlign(TextHAlign align, int col = -1);
	/// Sets whether to separate columns with dots.
	void setDot(bool dot);
	/// Sets whether the list is selectable.
	void setSelectable(bool selectable);
	/// Sets the text size to big.
	void setBig();
	/// Sets the text size to small.
	void setSmall();
	/// Sets whether to condense columns instead of a table like layout.
	void setCondensed(bool condensed);
	/// Sets the background for the selector.
	void setBackground(Surface *bg);
	/// Gets the selected row in the list.
	int getSelectedRow() const;
	/// Sets the margin of the text list.
	void setMargin(int margin);
	/// Gets the margin of the text list.
	int getMargin() const;
	/// Sets the arrow color of the text list.
	void setArrowColor(Uint8 color);
	/// Sets the arrow column of the text list.
	void setArrowColumn(int pos, ArrowOrientation type);
	/// Hooks an action handler to a mouse click on the left arrows.
	void onLeftArrowClick(ActionHandler handler);
	/// Hooks an action handler to a mouse press over the left arrows.
	void onLeftArrowPress(ActionHandler handler);
	/// Hooks an action handler to a mouse release over the left arrows.
	void onLeftArrowRelease(ActionHandler handler);
	/// Hooks an action handler to a mouse click on the right arrows.
	void onRightArrowClick(ActionHandler handler);
	/// Hooks an action handler to a mouse press over the right arrows.
	void onRightArrowPress(ActionHandler handler);
	/// Hooks an action handler to a mouse release over the right arrows.
	void onRightArrowRelease(ActionHandler handler);
	/// Clears the list.
	void clearList();
	/// Scrolls the list up.
	void scrollUp(bool toMax);
	/// Scrolls the list down.
	void scrollDown(bool toMax);
	/// Sets the list scrolling.
	void setScrolling(bool scrolling, int scrollPos = 4);
	/// Draws the text onto the text list.
	void draw();
	/// Blits the text list onto another surface.
	void blit(Surface *surface);
	/// Thinks arrow buttons.
	void think();
	/// Handles arrow buttons.
	void handle(Action *action, State *state);
	/// Special handling for mouse presses.
	void mousePress(Action *action, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(Action *action, State *state);
	/// Special handling for mouse clicks.
	void mouseClick(Action *action, State *state);
	/// Special handling for mouse hovering.
	void mouseOver(Action *action, State *state);
	/// Special handling for mouse hovering out.
	void mouseOut(Action *action, State *state);
	/// get the scroll depth
	int getScroll();
	void setAllX(int x);
	void setAllY(int y);
};

}

#endif
