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
#include "TextList.h"

/**
 * Sets up a blank list with the specified size and position.
 * @param big Pointer to the big-size font.
 * @param small Pointer to the small-size font.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
TextList::TextList(Font *big, Font *small, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _texts(), _columns(), _big(big), _small(small), _rowY(0), _color(0), _dot(false), _selectable(false), _selRow(0), _bg(0)
{
	_selector = new Surface(width, 8, 0, 0);
	_selector->setVisible(false);
}

/**
 * Deletes all the child Text's contained by the list.
 */
TextList::~TextList()
{
	for (vector< vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); u++) {
		for (vector<Text*>::iterator v = (*u).begin(); v < (*u).end(); v++) {
			delete (*v);
		}
	}
}

/**
 * Returns a pointer to a certain Text object in the list.
 * Useful for customizing or getting values off an individual cell.
 * @note If a Text is changed, a redraw needs to be manually requested.
 * @param row Row number.
 * @param col Column number.
 * @return Pointer to the requested Text.
 */
Text* TextList::getCell(int row, int col)
{
	return _texts[row][col];
}

/**
 * Adds a new row of text to the list, automatically creating
 * the required Text objects lined up where they need to be.
 * @param num Number of columns.
 * @param ... Text for each cell in the new row.
 */
void TextList::addRow(int num, ...)
{
	va_list args;
	va_start(args, num);
	vector<Text*> temp;
	int rowX = 0;

	for (int i = 0; i < num; i++)
	{
		Text* txt = new Text(_big, _small, _columns[i], _small->getHeight(), rowX, _rowY);
		txt->setPalette(this->getPalette());
		
		string buf = va_arg(args, char*);
		// Places dots between text
		if (_dot && i < num - 1)
		{
			int w = 0;
			for (string::iterator c = buf.begin(); c < buf.end(); c++)
			{
				if (*c == ' ')
					w += _small->getWidth() / 2;
				else
					w += _small->getChar(*c)->getCrop()->w + _small->getSpacing();
			}
			while (w < _columns[i])
			{
				w += _small->getChar('.')->getCrop()->w + _small->getSpacing();
				buf += '.';
			}
		}
		txt->setText(buf);

		txt->setColor(_color);
		txt->setSmall();
		temp.push_back(txt);
		rowX += _columns[i];
	}
	_texts.push_back(temp);
	_rowY += _small->getHeight() + _small->getSpacing();
	draw();

	va_end(args);
}

/**
 * Changes the number of columns that the list contains.
 * While rows can be unlimited, columns need to be specified
 * since they can have variable widths for lining up the text.
 * @param num Number of columns.
 * @param ... Width of each column.
 */
void TextList::setColumns(int num, ...)
{
	va_list args;
	va_start(args, num);

	for (int i = 0; i < num; i++)
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
	for (vector< vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); u++)
	{
		for (vector<Text*>::iterator v = u->begin(); v < u->end(); v++)
		{
			(*v)->setPalette(colors, firstcolor, ncolors);
		}
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
 * Returns the currently selected row if the text
 * list is selectable.
 * @return Selected row.
 */
int TextList::getSelectedRow()
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
	_selector->setPalette(_bg->getPalette());
}

/**
 * Removes all the rows currently stored in the list.
 */
void TextList::clearList()
{
	for (vector< vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); u++)
	{
		for (vector<Text*>::iterator v = u->begin(); v < u->end(); v++)
		{
			delete (*v);
		}
		u->clear();
	}
	_texts.clear();
	_rowY = 0;
}

/**
 * Draws the text list and all the text contained within.
 */
void TextList::draw()
{
	clear();
	_selector->blit(this);
	for (vector< vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); u++)
	{
		for (vector<Text*>::iterator v = (*u).begin(); v < (*u).end(); v++)
		{
            (*v)->blit(this);
        }
    }
}

/**
 * Ignores any mouse clicks that aren't on a row with the left mouse button.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void TextList::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (_selectable && ev->button.button == SDL_BUTTON_LEFT)
	{
		if (_selRow < _texts.size())
			InteractiveSurface::mousePress(ev, scale, state);
	}
}

/*
 * Ignores any mouse clicks that aren't on a row with the left mouse button.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void TextList::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (_selectable && ev->button.button == SDL_BUTTON_LEFT)
	{
		if (_selRow < _texts.size())
			InteractiveSurface::mouseRelease(ev, scale, state);
	}
}

/**
 * Ignores any mouse clicks that aren't on a row with the left mouse button.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void TextList::mouseClick(SDL_Event *ev, int scale, State *state)
{
	if (_selectable && ev->button.button == SDL_BUTTON_LEFT)
	{
		if (_selRow < _texts.size())
			InteractiveSurface::mouseClick(ev, scale, state);
	}
}

/**
 * Selects the row the mouse is over.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void TextList::mouseOver(SDL_Event *ev, int scale, State *state)
{
	if (_selectable)
	{
		double y = ev->button.y - _y * scale;
		_selRow = (int)floor(y / (8.0 * scale));

		if (_selRow < _texts.size())
		{
			if (!_selector->getVisible() || _selector->getY() != _selRow * 8)
			{
				_selector->setX(_x - _bg->getX());
				_selector->setY(_y - _bg->getY() + _selRow * 8);
				_selector->copy(_bg);
				_selector->setX(0);
				_selector->setY(_selRow * 8);
				_selector->offset(-10, Palette::backPos);
				_selector->setVisible(true);
				draw();
			}
		}
		else
		{
			if (_selector->getVisible())
			{
				_selector->setVisible(false);
				draw();
			}
		}
	}

	InteractiveSurface::mouseOver(ev, scale, state);
}

/**
 * Deselects the row.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void TextList::mouseOut(SDL_Event *ev, int scale, State *state)
{
	if (_selectable)
	{
		_selector->setVisible(false);
		draw();
	}

	InteractiveSurface::mouseOut(ev, scale, state);
}