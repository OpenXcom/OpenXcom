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
#include "TextList.h"
#include <cstdarg>
#include <cmath>
#include "../Engine/Action.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "ArrowButton.h"

namespace OpenXcom
{

/**
 * Sets up a blank list with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
TextList::TextList(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _texts(), _columns(), _big(0), _small(0), _font(0), _scroll(0), _visibleRows(0), _color(0), _dot(false), _selectable(false), _condensed(false), _contrast(false),
																								   _selRow(0), _bg(0), _selector(0), _margin(0), _scrolling(true), _arrowLeft(), _arrowRight(), _arrowPos(-1), _scrollPos(4), _arrowType(ARROW_VERTICAL), _leftClick(0), _leftPress(0), _leftRelease(0), _rightClick(0), _rightPress(0), _rightRelease(0)
{
	_allowScrollOnArrowButtons = true;
	_up = new ArrowButton(ARROW_BIG_UP, 13, 14, getX() + getWidth() + _scrollPos, getY() + 1);
	_up->setVisible(false);
	_up->setTextList(this);
	_down = new ArrowButton(ARROW_BIG_DOWN, 13, 14, getX() + getWidth() + _scrollPos, getY() + getHeight() - 12);
	_down->setVisible(false);
	_down->setTextList(this);
}

/**
 * Deletes all the stuff contained in the list.
 */
TextList::~TextList()
{
	for (std::vector< std::vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); ++u)
	{
		for (std::vector<Text*>::iterator v = (*u).begin(); v < (*u).end(); ++v)
		{
			delete *v;
		}
	}
	for (std::vector<ArrowButton*>::iterator i = _arrowLeft.begin(); i < _arrowLeft.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<ArrowButton*>::iterator i = _arrowRight.begin(); i < _arrowRight.end(); ++i)
	{
		delete *i;
	}
	delete _selector;
	delete _up;
	delete _down;
}

/**
 * Sets the allowScrollOnArrowButtons.
 * @param value new value.
 */
void TextList::setAllowScrollOnArrowButtons(bool value)
{
	_allowScrollOnArrowButtons = value;
}

/**
 * Gets the arrowsLeftEdge.
 * @return arrowsLeftEdge.
 */
int TextList::getArrowsLeftEdge()
{
	return _arrowsLeftEdge;
}

/**
 * Gets the arrowsRightEdge.
 * @return arrowsRightEdge.
 */
int TextList::getArrowsRightEdge()
{
	return _arrowsRightEdge;
}

/**
 * Unpresses all the arrow buttons.
 * @param state Pointer to running state.
 */
void TextList::unpress(State *state)
{
	InteractiveSurface::unpress(state);
	for (std::vector<ArrowButton*>::iterator i = _arrowLeft.begin(); i < _arrowLeft.end(); ++i)
	{
		(*i)->unpress(state);
	}
	for (std::vector<ArrowButton*>::iterator i = _arrowRight.begin(); i < _arrowRight.end(); ++i)
	{
		(*i)->unpress(state);
	}
}

/**
 * Changes the color of a specific Text object in the list.
 * @param row Row number.
 * @param column Column number.
 * @param color Text color.
 */
void TextList::setCellColor(int row, int column, Uint8 color)
{
	_texts[row][column]->setColor(color);
	_redraw = true;
}

/**
 * Changes the text color of a whole row in the list.
 * @param row Row number.
 * @param color Text color.
 */
void TextList::setRowColor(int row, Uint8 color)
{
	for (std::vector<Text*>::iterator i = _texts[row].begin(); i < _texts[row].end(); ++i)
	{
		(*i)->setColor(color);
	}
	_redraw = true;
}

/**
 * Returns the text of a specific Text object in the list.
 * @param row Row number.
 * @param column Column number.
 * @return Text string.
 */
std::wstring TextList::getCellText(int row, int column) const
{
	return _texts[row][column]->getText();
}

/**
 * Changes the text of a specific Text object in the list.
 * @param row Row number.
 * @param column Column number.
 * @param text Text string.
 */
void TextList::setCellText(int row, int column, const std::wstring &text)
{
	_texts[row][column]->setText(text);
	_redraw = true;
}

/**
 * Returns the X position of a specific text column in the list.
 * @param column Column number.
 * @return X position in pixels.
 */
int TextList::getColumnX(int column) const
{
	return getX() + _texts[0][column]->getX();
}

/**
 * Returns the Y position of a specific text row in the list.
 * @param row Row number.
 * @return Y position in pixels.
 */
int TextList::getRowY(int row) const
{
	return getY() + _texts[row][0]->getY();
}

/**
 * Adds a new row of text to the list, automatically creating
 * the required Text objects lined up where they need to be.
 * @param cols Number of columns.
 * @param ... Text for each cell in the new row.
 */
void TextList::addRow(int cols, ...)
{
	va_list args;
	va_start(args, cols);
	std::vector<Text*> temp;
	int rowX = 0;

	for (int i = 0; i < cols; ++i)
	{
		// Place text
		Text* txt = new Text(_columns[i], _font->getHeight(), _margin + rowX, getY());
		txt->setPalette(this->getPalette());
		txt->setFonts(_big, _small);
		txt->setColor(_color);
		txt->setSecondaryColor(_color2);
		if (_align[i])
		{
			txt->setAlign(_align[i]);
		}
		txt->setHighContrast(_contrast);
		if (_font == _big)
		{
			txt->setBig();
		}
		else
		{
			txt->setSmall();
		}
		txt->setText(va_arg(args, wchar_t*));

		// Places dots between text
		if (_dot && i < cols - 1)
		{
			std::wstring buf = txt->getText();
			int w = txt->getTextWidth();
			while (w < _columns[i])
			{
				w += _font->getChar('.')->getCrop()->w + _font->getSpacing();
				buf += '.';
			}
			txt->setText(buf);
		}

		temp.push_back(txt);
		if (_condensed)
		{
			rowX += txt->getTextWidth();
		}
		else
		{
			rowX += _columns[i];
		}
	}
	_texts.push_back(temp);

	// Place arrow buttons
	if (_arrowPos != -1)
	{
		ArrowShape shape1, shape2;
		if (_arrowType == ARROW_VERTICAL)
		{
			shape1 = ARROW_SMALL_UP;
			shape2 = ARROW_SMALL_DOWN;
		}
		else
		{
			shape1 = ARROW_SMALL_LEFT;
			shape2 = ARROW_SMALL_RIGHT;
		}
		ArrowButton *a1 = new ArrowButton(shape1, 11, 8, getX() + _arrowPos, getY());
		a1->setListButton();
		a1->setPalette(this->getPalette());
		a1->setColor(_up->getColor());
		a1->onMouseClick(_leftClick, 0);
		a1->onMousePress(_leftPress);
		a1->onMouseRelease(_leftRelease);
		_arrowLeft.push_back(a1);
		ArrowButton *a2 = new ArrowButton(shape2, 11, 8, getX() + _arrowPos + 12, getY());
		a2->setListButton();
		a2->setPalette(this->getPalette());
		a2->setColor(_up->getColor());
		a2->onMouseClick(_rightClick, 0);
		a2->onMousePress(_rightPress);
		a2->onMouseRelease(_rightRelease);
		_arrowRight.push_back(a2);
	}

	_redraw = true;
	va_end(args);
	updateArrows();
}

/**
 * Changes the columns that the list contains.
 * While rows can be unlimited, columns need to be specified
 * since they can have various widths for lining up the text.
 * @param cols Number of columns.
 * @param ... Width of each column.
 */
void TextList::setColumns(int cols, ...)
{
	va_list args;
	va_start(args, cols);

	for (int i = 0; i < cols; ++i)
	{
		_columns.push_back(va_arg(args, int));
	}

	va_end(args);
}

/**
 * Replaces a certain amount of colors in the palette of all
 * the text contained in the list.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void TextList::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	for (std::vector< std::vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); ++u)
	{
		for (std::vector<Text*>::iterator v = u->begin(); v < u->end(); ++v)
		{
			(*v)->setPalette(colors, firstcolor, ncolors);
		}
	}
	for (std::vector<ArrowButton*>::iterator i = _arrowLeft.begin(); i < _arrowLeft.end(); ++i)
	{
		(*i)->setPalette(colors, firstcolor, ncolors);
	}
	for (std::vector<ArrowButton*>::iterator i = _arrowRight.begin(); i < _arrowRight.end(); ++i)
	{
		(*i)->setPalette(colors, firstcolor, ncolors);
	}
	if (_selector != 0)
	{
		_selector->setPalette(colors, firstcolor, ncolors);
	}
	_up->setPalette(colors, firstcolor, ncolors);
	_down->setPalette(colors, firstcolor, ncolors);
}

/**
 * Changes the various fonts of the text in the list
 * and calculates the selector and visible amount of rows.
 * @param big Pointer to large-size font.
 * @param small Pointer to small-size font.
 */
void TextList::setFonts(Font *big, Font *small)
{
	_big = big;
	_small = small;
	_font = small;

	delete _selector;
	_selector = new Surface(getWidth(), _font->getHeight() + _font->getSpacing(), getX(), getY());
	_selector->setPalette(getPalette());
	_selector->setVisible(false);

	for (int y = 0; y < getHeight(); y += _font->getHeight() + _font->getSpacing())
	{
		_visibleRows++;
	}
}

/**
 * Changes the color of the text in the list. This doesn't change
 * the color of existing text, just the color of text added from then on.
 * @param color Color value.
 */
void TextList::setColor(Uint8 color)
{
	_color = color;
	_up->setColor(color);
	_down->setColor(color);
}

/**
 * Returns the color of the text in the list.
 * @return Color value.
 */
Uint8 TextList::getColor() const
{
	return _color;
}

/**
 * Changes the secondary color of the text in the list.
 * @param color Color value.
 */
void TextList::setSecondaryColor(Uint8 color)
{
	_color2 = color;
}

/**
 * Returns the secondary color of the text in the list.
 * @return Color value.
 */
Uint8 TextList::getSecondaryColor() const
{
	return _color2;
}

/**
 * Enables/disables high contrast color. Mostly used for
 * Battlescape text.
 * @param contrast High contrast setting.
 */
void TextList::setHighContrast(bool contrast)
{
	_contrast = contrast;
}

/**
 * Changes the horizontal alignment of the text in the list. This doesn't change
 * the alignment of existing text, just the alignment of text added from then on.
 * @param align Horizontal alignment.
 * @param col the column to set the alignment for (defaults to -1, meaning "all")
 */
void TextList::setAlign(TextHAlign align, int col)
{
	if (col == -1)
	{
		for (size_t i = 0; i <= _columns.size() - 1; ++i)
		{
			_align[i] = align;
		}
	}
	else
	{
		_align[col] = align;
	}
}

/**
 * If enabled, the text in different columns will be separated by dots.
 * Otherwise, it will only be separated by blank space.
 * @param dot True for dots, False for spaces.
 */
void TextList::setDot(bool dot)
{
	_dot = dot;
}

/**
 * If enabled, the list will respond to player input,
 * highlighting selected rows and receiving clicks.
 * @param selectable Selectable setting.
 */
void TextList::setSelectable(bool selectable)
{
	_selectable = selectable;
}

/**
 * Changes the text list to use the big-size font.
 */
void TextList::setBig()
{
	_font = _big;

	delete _selector;
	_selector = new Surface(getWidth(), _font->getHeight() + _font->getSpacing(), getX(), getY());
	_selector->setPalette(getPalette());
	_selector->setVisible(false);

	for (int y = 0; y < getHeight(); y += _font->getHeight() + _font->getSpacing())
	{
		_visibleRows++;
	}
}

/**
 * Changes the text list to use the small-size font.
 */
void TextList::setSmall()
{
	_font = _small;

	delete _selector;
	_selector = new Surface(getWidth(), _font->getHeight() + _font->getSpacing(), getX(), getY());
	_selector->setPalette(getPalette());
	_selector->setVisible(false);

	for (int y = 0; y < getHeight(); y += _font->getHeight() + _font->getSpacing())
	{
		_visibleRows++;
	}
}

/**
 * If enabled, the columns will match the text width.
 * Otherwise, they will have a fixed width.
 * @param condensed True for condensed layout, False for table layout.
 */
void TextList::setCondensed(bool condensed)
{
	_condensed = condensed;
}

/**
 * Returns the currently selected row if the text
 * list is selectable.
 * @return Selected row.
 */
int TextList::getSelectedRow() const
{
	return _selRow;
}

/**
 * Changes the surface used to draw the background of the selector.
 * @param bg New background.
 */
void TextList::setBackground(Surface *bg)
{
	_bg = bg;
}

/**
 * Changes the horizontal margin placed around the text.
 * @param margin Margin in pixels.
 */
void TextList::setMargin(int margin)
{
	_margin = margin;
}

/**
 * Returns the margin of the text in the list.
 * @return Margin in pixels.
 */
int TextList::getMargin() const
{
	return _margin;
}

/**
 * Changes the color of the arrow buttons in the list.
 * @param color Color value.
 */
void TextList::setArrowColor(Uint8 color)
{
	_up->setColor(color);
	_down->setColor(color);
}

/**
 * Sets the position of the column of arrow buttons
 * in the text list.
 * @param pos X in pixels (-1 to disable).
 * @param type Arrow orientation type.
 */
void TextList::setArrowColumn(int pos, ArrowOrientation type)
{
	_arrowPos = pos;
	_arrowType = type;
	_arrowsLeftEdge = getX() + _arrowPos;
	_arrowsRightEdge = _arrowsLeftEdge + 12 + 11;
}

/**
 * Sets a function to be called every time the left arrows are mouse clicked.
 * @param handler Action handler.
 */
void TextList::onLeftArrowClick(ActionHandler handler)
{
	_leftClick = handler;
	for (std::vector<ArrowButton*>::iterator i = _arrowLeft.begin(); i < _arrowLeft.end(); ++i)
	{
		(*i)->onMouseClick(handler, 0);
	}
}

/**
 * Sets a function to be called every time the left arrows are mouse pressed.
 * @param handler Action handler.
 */
void TextList::onLeftArrowPress(ActionHandler handler)
{
	_leftPress = handler;
	for (std::vector<ArrowButton*>::iterator i = _arrowLeft.begin(); i < _arrowLeft.end(); ++i)
	{
		(*i)->onMousePress(handler);
	}
}

/**
 * Sets a function to be called every time the left arrows are mouse released.
 * @param handler Action handler.
 */
void TextList::onLeftArrowRelease(ActionHandler handler)
{
	_leftRelease = handler;
	for (std::vector<ArrowButton*>::iterator i = _arrowLeft.begin(); i < _arrowLeft.end(); ++i)
	{
		(*i)->onMouseRelease(handler);
	}
}

/**
 * Sets a function to be called every time the right arrows are mouse clicked.
 * @param handler Action handler.
 */
void TextList::onRightArrowClick(ActionHandler handler)
{
	_rightClick = handler;
	for (std::vector<ArrowButton*>::iterator i = _arrowRight.begin(); i < _arrowRight.end(); ++i)
	{
		(*i)->onMouseClick(handler, 0);
	}
}

/**
 * Sets a function to be called every time the right arrows are mouse pressed.
 * @param handler Action handler.
 */
void TextList::onRightArrowPress(ActionHandler handler)
{
	_rightPress = handler;
	for (std::vector<ArrowButton*>::iterator i = _arrowRight.begin(); i < _arrowRight.end(); ++i)
	{
		(*i)->onMousePress(handler);
	}
}

/**
 * Sets a function to be called every time the right arrows are mouse released.
 * @param handler Action handler.
 */
void TextList::onRightArrowRelease(ActionHandler handler)
{
	_rightRelease = handler;
	for (std::vector<ArrowButton*>::iterator i = _arrowRight.begin(); i < _arrowRight.end(); ++i)
	{
		(*i)->onMouseRelease(handler);
	}
}

/**
 * Removes all the rows currently stored in the list.
 */
void TextList::clearList()
{
	for (std::vector< std::vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); ++u)
	{
		for (std::vector<Text*>::iterator v = u->begin(); v < u->end(); ++v)
		{
			delete (*v);
		}
		u->clear();
	}
	_texts.clear();
	_redraw = true;
}

/**
 * Scrolls the text in the list up by one row or to the top.
 * @param toMax If true then scrolls to the top of the list. false => one row up
 */
void TextList::scrollUp(bool toMax)
{
	if (!_scrolling)
		return;
	if (_texts.size() > _visibleRows && _scroll > 0)
	{
		if (toMax) _scroll=0; else _scroll--;
		_redraw = true;
	}
	updateArrows();
}

/**
 * Scrolls the text in the list down by one row or to the bottom.
 * @param toMax If true then scrolls to the bottom of the list. false => one row down
 */
void TextList::scrollDown(bool toMax)
{
	if (!_scrolling)
		return;
	if (_texts.size() > _visibleRows && _scroll < _texts.size() - _visibleRows)
	{
		if (toMax) _scroll=_texts.size()-_visibleRows; else _scroll++;
		_redraw = true;
	}
	updateArrows();
}

/**
 * Updates the visibility of the arrow buttons according to
 * the current scroll position.
 */
void TextList::updateArrows()
{
	_up->setVisible((_texts.size() > _visibleRows && _scroll > 0));
	_down->setVisible((_texts.size() > _visibleRows && _scroll < _texts.size() - _visibleRows));
}

/**
 * Changes whether the list can be scrolled.
 * @param scrolling True to allow scrolling, false otherwise.
 * @param scrollPos Custom X position for the scroll buttons.
 */
void TextList::setScrolling(bool scrolling, int scrollPos)
{
	_scrolling = scrolling;
	if (scrollPos != _scrollPos)
	{
		_scrollPos = scrollPos;
		_up->setX(getX() + getWidth() + _scrollPos);
		_down->setX(getX() + getWidth() + _scrollPos);
	}
}

/**
 * Draws the text list and all the text contained within.
 */
void TextList::draw()
{
	Surface::draw();
	for (unsigned int i = _scroll; i < _texts.size() && i < _scroll + _visibleRows; ++i)
	{
		for (std::vector<Text*>::iterator j = _texts[i].begin(); j < _texts[i].end(); ++j)
		{
			(*j)->setY((i - _scroll) * (_font->getHeight() + _font->getSpacing()));
			(*j)->blit(this);
		}
	}
}

/**
 * Blits the text list and selector.
 * @param surface Pointer to surface to blit onto.
 */
void TextList::blit(Surface *surface)
{
	if (_visible && !_hidden)
	{
		_selector->blit(surface);
	}
	Surface::blit(surface);
	if (_visible && !_hidden)
	{
		_up->blit(surface);
		_down->blit(surface);
		if (_arrowPos != -1)
		{
			for (unsigned int i = _scroll; i < _texts.size() && i < _scroll + _visibleRows; ++i)
			{
				_arrowLeft[i]->setY(getY() + (i - _scroll) * (_font->getHeight() + _font->getSpacing()));
				_arrowLeft[i]->blit(surface);
				_arrowRight[i]->setY(getY() + (i - _scroll) * (_font->getHeight() + _font->getSpacing()));
				_arrowRight[i]->blit(surface);
			}
		}
	}
}

/**
 * Passes events to arrow buttons.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextList::handle(Action *action, State *state)
{
	InteractiveSurface::handle(action, state);
	_up->handle(action, state);
	_down->handle(action, state);
	if (_arrowPos != -1)
	{
		for (unsigned int i = _scroll; i < _texts.size() && i < _scroll + _visibleRows; ++i)
		{
			_arrowLeft[i]->handle(action, state);
			_arrowRight[i]->handle(action, state);
		}
	}
}

/**
 * Passes ticks to arrow buttons.
 */
void TextList::think()
{
	InteractiveSurface::think();
	_up->think();
	_down->think();
	for (std::vector<ArrowButton*>::iterator i = _arrowLeft.begin(); i < _arrowLeft.end(); ++i)
	{
		(*i)->think();
	}
	for (std::vector<ArrowButton*>::iterator i = _arrowRight.begin(); i < _arrowRight.end(); ++i)
	{
		(*i)->think();
	}
}

/**
 * Ignores any mouse clicks that aren't on a row.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextList::mousePress(Action *action, State *state)
{
	bool allowScroll = _allowScrollOnArrowButtons;
	if (!allowScroll)
	{
		allowScroll = (action->getAbsoluteXMouse() < _arrowsLeftEdge || action->getAbsoluteXMouse() > _arrowsRightEdge);
	}
	if (allowScroll)
	{
		if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP) scrollUp(false);
		else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN) scrollDown(false);
	}
	if (_selectable)
	{
		if (_selRow < _texts.size())
		{
			InteractiveSurface::mousePress(action, state);
		}
	}
	else
	{
		InteractiveSurface::mousePress(action, state);
	}
}

/*
 * Ignores any mouse clicks that aren't on a row.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextList::mouseRelease(Action *action, State *state)
{
	if (_selectable)
	{
		if (_selRow < _texts.size())
		{
			InteractiveSurface::mouseRelease(action, state);
		}
	}
	else
	{
		InteractiveSurface::mouseRelease(action, state);
	}
}

/**
 * Ignores any mouse clicks that aren't on a row.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextList::mouseClick(Action *action, State *state)
{
	if (_selectable)
	{
		if (_selRow < _texts.size())
		{
			InteractiveSurface::mouseClick(action, state);
		}
	}
	else
	{
		InteractiveSurface::mouseClick(action, state);
	}
}

/**
 * Selects the row the mouse is over.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextList::mouseOver(Action *action, State *state)
{
	if (_selectable)
	{
		int h = _font->getHeight() + _font->getSpacing();
		_selRow = _scroll + (int)floor(action->getRelativeYMouse() / (h * action->getYScale()));

		if (_selRow < _texts.size())
		{
			_selector->setY(getY() + (_selRow - _scroll) * h);
			_selector->copy(_bg);
			if (_contrast)
				_selector->offset(-10, 1);
			else
				_selector->offset(-10, Palette::backPos);
			_selector->setVisible(true);
		}
		else
		{
			_selector->setVisible(false);
		}
	}

	InteractiveSurface::mouseOver(action, state);
}

/**
 * Deselects the row.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void TextList::mouseOut(Action *action, State *state)
{
	if (_selectable)
	{
		_selector->setVisible(false);
	}

	InteractiveSurface::mouseOut(action, state);
}
/*
 * get the scroll depth.
 * @return scroll depth.
 */
int TextList::getScroll()
{
	return _scroll;
}

void TextList::setAllX(int x)
{
	_x = x;
	_up->setX(getX() + getWidth() + _scrollPos);
	_down->setX(getX() + getWidth() + _scrollPos);
	_selector->setX(x);
}

void TextList::setAllY(int y)
{
	_y = y;
	_up->setY(getY() + 1);
	_down->setY(getY() + getHeight() - 12);
	_selector->setY(y);
}
}
